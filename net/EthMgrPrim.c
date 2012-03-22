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

#define VERBOSE_PR(...) ( EBB_LRT_printf(__VA_ARGS__) )

#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

#define NUMETHTYPES (1<<(sizeof(uint16_t) * 8))

CObject(EvHdlr) {
  CObjInterface(EventHandler) *ft;
}; 

CObject(EthMgrPrim) {
  CObjInterface(EthMgr) *ft;
  EthTypeMgrId typeMgrs[NUMETHTYPES];
  CObjectDefine(EvHdlr) evHdlr;
  EventHandlerId hdlrId;
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
EthMgrPrim_handleEvent(void *_self)
{
  EthMgrPrim *self = ContainingCOPtr(_self,EthMgrPrim,evHdlr);
  ethlib_nic_readpkt();
  self->rcnt++;
  return EBBRC_OK;
}
  
CObjInterface(EthMgr) EthMgrPrim_ftable = {
  // base functions of ethernet manager
  .init = EthMgrPrim_init,
  .bind = EthMgrPrim_bind,
  {// the implementation of the event handler functions
    .handleEvent = EthMgrPrim_handleEvent
  }
};

static inline void 
EthMgrPrimSetFT(EthMgrPrimRef o) 
{ 
  o->ft = &EthMgrPrim_ftable; 
  o->evHdlr.ft = &(EthMgrPrim_ftable.EventHandler_if);
}


EBBRC
EthMgrPrimCreate(EthMgrId *id, char *nic) 
{
  EBBRC rc;
  EthMgrPrimRef repRef;
  CObjEBBRootSharedRef rootRef;
  lrt_pic_src nicisrc;

  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  
  CObjEBBRootSharedSetFT(rootRef);
  EthMgrPrimSetFT(repRef);

  bzero(repRef->typeMgrs, sizeof(repRef->typeMgrs));
  repRef->rcnt=0;

  rootRef->ft->init(rootRef, (EBBRep *)repRef);
  
  rc = EBBAllocPrimId((EBBId *)id);
  EBBRCAssert(rc);

  rc = CObjEBBBind((EBBId)*id, rootRef); 
  EBBRCAssert(rc);

  // setup the EthMgr on a second id that services the EventHander Interface
  // FIXME: take a look at ~/SESA/EBBlib/l0/cobj/cobjtest.c to clean up multiple inheritence
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  CObjEBBRootSharedSetFT(rootRef);
  rootRef->ft->init(rootRef, (EBBRep *)&(repRef->evHdlr));
  rc = EBBAllocPrimId((EBBId *)&(repRef->hdlrId));
  EBBRCAssert(rc);
  rc = CObjEBBBind((EBBId)repRef->hdlrId, rootRef);
  EBBRCAssert(rc);
 
  rc = EBBCALL(theEventMgrPrimId, allocEventNo, &(repRef->ev));
  EBBRCAssert(rc);

  if (nic) {
    rc = ethlib_nic_init(nic, &nicisrc);
    if (EBBRC_SUCCESS(rc)) {
      rc = EBBCALL(theEventMgrPrimId, registerHandler, repRef->ev, 
		   repRef->hdlrId, nicisrc);
      EBBRCAssert(rc);
    }
  }
  return EBBRC_OK;
}
