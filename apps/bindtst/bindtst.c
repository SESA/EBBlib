/*
 * Copyright (C) 2011 by Project SESA, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <config.h>
#include <inttypes.h>

#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <lrt/io.h>
#include <l0/lrt/trans.h>
#include <lrt/exit.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <lrt/assert.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/App.h>
#include <sync/barrier.h>

/************ CODE TO BE MOVE ONCE HAPPY **********/

typedef struct {
  EBBMissFunc mf;
  EBBMissArg arg;
} EBBInstance;

#define NULLID (0)

#define EBBRC_NULL (-6)

EBBRC nullmissmf(EBBRep **repptr, EBBLTrans *lt, FuncNum fn, EBBMissArg a)
{
  return EBBRC_NULL;
}

static inline EBBRC Bind(EBBId id, EBBInstance i)
{
  return EBBBindPrimId(id, i.mf, i.arg);
}

struct {
  EBBId NULLId;
  EBBInstance NULLInst;
} L0Info = { .NULLId = NULLID, .NULLInst = { nullmissmf, 0} };

EBBRC
L0InfoInit(void)
{
  EBBRC rc;
  /* NULLId should likely be a hardcoded constant id rather than an
     allocated Id */
  rc = EBBAllocPrimId((EBBId *)&(L0Info.NULLId));
  LRT_RCAssert(rc);

  rc = Bind((EBBId)L0Info.NULLId, L0Info.NULLInst);
  LRT_RCAssert(rc);

  return EBBRC_OK;
}

static inline void
CObjEBBInstance(EBBInstance *i, void *rootRef)
{
  i->mf = CObjEBBMissFunc;
  i->arg = (uintptr_t)rootRef;
}

/************** ACTUAL TEST CODE ****************/

#define EBBCALL COBJ_EBBCALL

COBJ_EBBType(Service) {
  EBBRC (*op) (ServiceRef _self);
};

struct {
  ServiceId theId;
} ServiceInfo = { NULLID };

CObject(SrvImp0)
{
  COBJ_EBBFuncTbl(Service);
};

static EBBRC
SrvImp0_op(ServiceRef _self)
{
  lrt_printf("%s: _self=%p : Executed!\n", __func__, _self);
  return 100;
}

CObjInterface(Service) SrvImp0_ftable = {
  .op = SrvImp0_op
};

void
SrvImp0SetFT(SrvImp0Ref o)
{
  o->ft = &(SrvImp0_ftable);
}

EBBRC
SrvImp0Create(EBBInstance *i)
{
  SrvImp0Ref repRef;
  CObjEBBRootSharedRef rootRef;
  EBBRC rc;

  rc = EBBPrimMalloc(sizeof(SrvImp0), &repRef, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);
  SrvImp0SetFT(repRef);

  rc = CObjEBBRootSharedCreate(&rootRef, (EBBRepRef)repRef);
  LRT_RCAssert(rc);

  CObjEBBInstance(i, rootRef);

  return EBBRC_OK;
}

CObject(SrvImp1)
{
  COBJ_EBBFuncTbl(Service);
};

static EBBRC
SrvImp1_op(ServiceRef _self)
{
  lrt_printf("%s: _self=%p : Executed!\n", __func__, _self);
  return 200;
}

CObjInterface(Service) SrvImp1_ftable = {
  .op = SrvImp1_op
};

void
SrvImp1SetFT(SrvImp1Ref o)
{
  o->ft = &(SrvImp1_ftable);
}

EBBRC
SrvImp1Create(EBBInstance *i)
{
  SrvImp1Ref repRef;
  CObjEBBRootSharedRef rootRef;
  EBBRC rc;

  rc = EBBPrimMalloc(sizeof(SrvImp1), &repRef, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);
  SrvImp1SetFT(repRef);

  rc = CObjEBBRootSharedCreate(&rootRef, (EBBRepRef)repRef);
  LRT_RCAssert(rc);

  CObjEBBInstance(i, rootRef);

  return EBBRC_OK;
}

CObject(BindTst) {
  CObjInterface(App) *ft;
};

EBBInstance s0Inst;
EBBInstance s1Inst;

volatile int cores=0;
struct barrier_s bar;


EBBRC
BindTst_start(AppRef _self)
{
  EBBRC rc;
  int passed=1;
  int master=0;
  int sense=0;			/* sense of thread for barrier */

  if (MyEventLoc() == 0) {
    master = 1;
    init_barrier(&bar, NumEventLoc());

    // last thing; initialize cores will unblock everyone else
    cores = NumEventLoc();
  } else {
    // everyone but first core blocks here
    while (*(volatile int *)&cores == 0){}
  }

  lrt_printf("bindtst: START, core %d of cores %d\n", MyEventLoc(), cores);

  barrier(&bar, &sense); 

  if (master) {
    // This code should be move out when we are happy
    rc = L0InfoInit();
    LRT_RCAssert(rc);
  }

  barrier(&bar, &sense); 

  rc = EBBCALL((ServiceId)L0Info.NULLId, op);
  LRT_Assert(rc == EBBRC_NULL);

  if (master) {
    rc = SrvImp0Create(&s0Inst);
    LRT_RCAssert(rc);

    rc = SrvImp1Create(&s1Inst);
    LRT_RCAssert(rc);

    lrt_printf("%s: ServiceInfo.theId=%p\n", __func__, ServiceInfo.theId);

#if 0
    // this segfaults as expected.  But maybe we should make NULLId really be a
    // valid  id that is bound to the NULLInst
    rc = EBBCALL(ServiceIds.theId, op);
    LRT_RCAssert(rc);
#endif

    rc = EBBAllocPrimId((EBBId *)(void *)&(ServiceInfo.theId));
    LRT_RCAssert(rc);
    lrt_printf("%s: After EBBAllocPrimId: ServiceInfo.theId=%p\n",
	       __func__, ServiceInfo.theId);

#if 0
    // this segfaults as expected.  But maybe it should not maybe we should bind
    // an allocated id to a NULLInst
    rc = EBBCALL(ServiceInfo.theId, op);
    lrt_printf("%s: no bind EBBCALL(ServiceIds.theId, op)=%ld\n", __func__, rc);
#endif
  }

  // bind service to L0Info (null)
  if (master) {
    rc = Bind((EBBId)ServiceInfo.theId, L0Info.NULLInst);
    LRT_RCAssert(rc);
  }

  barrier(&bar, &sense); 

  rc = EBBCALL(ServiceInfo.theId, op);
  if (!(passed == 1 && rc == EBBRC_NULL)) passed = 0;
  lrt_printf("%s: Bind to L0Info.Inst: EBBCALL(ServiceIds.theId, op)=%" PRIdPTR "\n",
	     __func__, rc);

  barrier(&bar, &sense); 
  // bind service to s0Inst
  if (master) {
    rc = Bind((EBBId)ServiceInfo.theId, s0Inst);
    LRT_RCAssert(rc);
  }

  barrier(&bar, &sense); 

  rc = EBBCALL(ServiceInfo.theId, op);
  if (!(passed == 1 && rc == 100)) passed = 0;
  lrt_printf("%s: Bind to s0Inst: EBBCALL(ServiceIds.theId, op)=%" PRIdPTR "\n",
	     __func__, rc);

  barrier(&bar, &sense); 

  if (master) {
    // bind service to s1Inst
    rc = Bind((EBBId)ServiceInfo.theId, s1Inst);
    LRT_RCAssert(rc);
  }

  barrier(&bar, &sense); 

  rc = EBBCALL(ServiceInfo.theId, op);
  if (!(passed == 1 && rc == 200)) passed = 0;
  lrt_printf("%s: Bind to s1Inst: EBBCALL(ServiceIds.theId, op)=%" PRIdPTR "\n",
	     __func__, rc);

  if (passed) lrt_printf("bindtst: PASSED\n");
  else lrt_printf("bindtst: FAILED\n");

  barrier(&bar, &sense); 

  if (master) lrt_exit(0);
  return EBBRC_OK;
}

CObjInterface(App) BindTst_ftable = {
  .start = BindTst_start
};

APP(BindTst, APP_START_ALL);

