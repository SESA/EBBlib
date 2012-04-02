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

#include <stdint.h>
#include <stddef.h>

#include <lrt/assert.h>
#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/EBBMgrPrimBoot.h>
#include <l0/EventMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l0/lrt/mem.h>
#include <l0/L0.h>
#include <l0/L0Prim.h>
#include <l1/MsgMgr.h>
#include <l1/MsgMgrPrim.h>
#include <l1/L1.h>
#include <l1/L1Prim.h>

CObject(L0Prim) {
  COBJ_EBBFuncTbl(L0);
  uintptr_t startInfo;
};

static EBBRC
L0Prim_version(L0Ref _self, uint64_t *v)
{
  char *version = (char *)v;

  version[0]='E';
  version[1]='b';
  version[2]='b';
  version[3]='O';
  version[4]='S';
  version[5]='0';
  version[6]='0';
  version[7]=0;

  return EBBRC_OK;
}

static EBBRC 
L0Prim_startEvent(L0Ref _self)
{
  L0PrimRef self = (L0PrimRef) _self;
  EBBRC rc;

  lrt_pic_ackipi();

  // call the next layer startup
  rc = L1PrimInit();
  EBBRCAssert(rc);

  COBJ_EBBCALL(theL1Id, start, self->startInfo);

  lrt_pic_enableipi();

  return 0;
};

static EBBRC
L0Prim_start(L0Ref _self, uintptr_t startInfo)
{
  L0PrimRef self = (L0PrimRef)_self;
  EBBRC rc;

  self->startInfo = startInfo;

  // this sets up, just on the local processor, IPI to temporarily
  // the first reset event.  The handleEvent will do all 
  // the subsequent initialization, now on an event
  rc = COBJ_EBBCALL(theEventMgrPrimId, registerIPIHandler, 
		    (EventHandlerId)theL0Id, COBJ_FUNCNUM(self, startEvent));
  EBBRCAssert(rc);

  rc = COBJ_EBBCALL(theEventMgrPrimId, dispatchIPI, MyEL());
  EBBRCAssert(rc);

  return 0;
};


static CObjInterface(L0) L0Prim_ftable = {
  .version    = L0Prim_version,
  .start      = L0Prim_start,
  .startEvent = (GenericEventFunc)L0Prim_startEvent
};

static EBBRep *
L0Prim_createRep(CObjEBBRootMultiRef _self) {
  L0PrimRef repRef;
  EBBPrimMalloc(sizeof(L0Prim), &repRef, EBB_MEM_DEFAULT);
  repRef->ft = &L0Prim_ftable;
  return (EBBRep *)repRef;
}

EBBRC
L0PrimInit(void)
{
  EBBRC rc = EBBRC_OK;

  if (__sync_bool_compare_and_swap(&theL0Id, (EventHandlerId)0,
				   (EventHandlerId)-1)) {
    EBBId id;
    CObjEBBRootMultiImpRef rootRef;
    rc = CObjEBBRootMultiImpCreate(&rootRef,
				  L0Prim_createRep);
    EBBRCAssert(rc);
    rc = EBBAllocPrimId(&id);
    EBBRCAssert(rc);
    rc = CObjEBBBind(id, rootRef); 
    EBBRCAssert(rc);
    theL0Id = (L0Id)id;
  } else {
    while (((volatile uintptr_t)theL0Id)==-1);
  }

  return rc;
};






