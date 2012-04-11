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
#include <lrt/io.h>
#include <l0/lrt/types.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/cobj/cobj.h>
#include <l0/cobj/CObjEBB.h>
#include <lrt/assert.h>
#include <l0/EBBMgrPrim.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootShared.h>

#include <net/EthTypeMgr.h>
#include <net/EthMgr.h>
#include <net/EthMgrPrim.h>

#include <l0/lrt/pic.h>
#include <net/lrt/ethlib.h>
#include <strings.h>

#define VERBOSE_PR(...) ( lrt_printf(__VA_ARGS__) )

#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

#define NUMETHTYPES (1<<(sizeof(uint16_t) * 8))

CObject(EthMgrPrim) {
  COBJ_EBBFuncTbl(EthMgr);

  EthTypeMgrId typeMgrs[NUMETHTYPES];
  uintptr_t ev;
  uintptr_t rcnt;
};


static EBBRC
EthMgrPrim_init(void *_self)
{
  return EBBRC_GENERIC_FAILURE;
}

static EBBRC
EthMgrPrim_bind(void *_self, uint16_t type, EthTypeMgrId id)
{
  return EBBRC_GENERIC_FAILURE;
}
  
static EBBRC 
EthMgrPrim_inEvent(EthMgrPrimRef self)
{
  ethlib_nic_readpkt();
  self->rcnt++;
  return EBBRC_OK;
}
  
CObjInterface(EthMgr) EthMgrPrim_ftable = {
  // base functions of ethernet manager
  .init = EthMgrPrim_init,
  .bind = EthMgrPrim_bind,
  
  .inEvent = (GenericEventFunc)EthMgrPrim_inEvent
};

static inline void 
EthMgrPrimSetFT(EthMgrPrimRef o) 
{ 
  o->ft = &EthMgrPrim_ftable; 
}


EBBRC
EthMgrPrimCreate(EthMgrId *id, char *nic) 
{
  EBBRC rc;
  EthMgrPrimRef repRef;
  CObjEBBRootSharedRef rootRef;
  lrt_pic_src nicisrc, nicosrc;

  rc = EBBPrimMalloc(sizeof(EthMgrPrim), &repRef, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  EthMgrPrimSetFT(repRef);
  bzero(repRef->typeMgrs, sizeof(repRef->typeMgrs));
  repRef->rcnt=0;

  rc = CObjEBBRootSharedCreate(&rootRef, (EBBRepRef)repRef);
  LRT_RCAssert(rc);

  rc = EBBAllocPrimId((EBBId *)id);
  LRT_RCAssert(rc);

  rc = CObjEBBBind((EBBId)*id, rootRef); 
  LRT_RCAssert(rc);

  // setup our events and there handling
  rc = EBBCALL(theEventMgrPrimId, allocEventNo, &(repRef->ev));
  LRT_RCAssert(rc);

  if (nic) {
    rc = ethlib_nic_init(nic, &nicisrc, &nicosrc);
    if (EBBRC_SUCCESS(rc)) {
      rc = EBBCALL(theEventMgrPrimId, registerHandler, repRef->ev, 
		   (EventHandlerId)*id, COBJ_FUNCNUM(repRef, inEvent),
		   &nicisrc);
      LRT_RCAssert(rc);

      rc = EBBCALL(theEventMgrPrimId, eventEnable, repRef->ev);
      LRT_RCAssert(rc);
    }
  }
  return EBBRC_OK;
}
