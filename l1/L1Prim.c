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
#include <l1/MsgMgr.h>
#include <l1/MsgMgrPrim.h>
#include <l1/L1.h>
#include <l1/L1Prim.h>
#include <l1/App.h>
#include <lrt/startinfo.h>
#include <lrt/misc.h>

#define JAKLUDGE 1

CObject(L1Prim) {
  CObjInterface(L1) *ft;
  CObjEBBRootMultiRef myRoot;
  char *si;
  intptr_t sisize;
  CObjectDefine(MsgHandler) startMH;
  MsgHandlerId startMHId;
};

#ifdef JAKLUDGE
EBBRC
L1Prim_MsgHandler_testMH(MsgHandlerRef _self)
{
  return EBBRC_OK;
}
#endif

EBBRC
L1Prim_MsgHandler_startMH(MsgHandlerRef _self, uintptr_t startinfo)
{
  EBBRC rc;
  L1PrimRef self = ContainingCOPtr(_self,L1Prim,startMH);  

  // we are done with the startMH we can destroy it
  // This should induce destruction of the temporary Root we 
  // created (which of course in this case should not destroy 
  // the rep which is actually the L1Prim Rep)
  EBBDestroyPrimId((EBBId)self->startMHId);

  // Add code to make a copy of our start info here
  if (startinfo) {
    self->sisize = lrt_startinfo_size();
    rc = EBBPrimMalloc(self->sisize,
		       &(self->si), EBB_MEM_DEFAULT);
    EBBRCAssert(rc);
    memcpy(self->si, (char *)startinfo, self->sisize);
  } else {
    self->si = NULL;
  }

  if (__sync_bool_compare_and_swap(&theApp, (AppId)0,
				   (AppId)-1)) {  
    EBBId id;
    CObjEBBRootMultiImpRef appRoot;
    // create App instance and invoke its start
    rc = CObjEBBRootMultiImpCreate(&appRoot, App_createRep);
    EBBRCAssert(rc);
    rc = EBBAllocPrimId(&id);
    EBBRCAssert(rc);
    rc = CObjEBBBind(id, appRoot); 
    EBBRCAssert(rc);
    theApp = (AppId)id;
  }

  // WE ARE NOW DONE WITH L1 INITIALIZATION : 
  //    We now  hand-over the start up msg to the appliation
  //    From this point on everything should be messages/events that are handled
  //    by appliation level Ebb's

  return COBJ_EBBCALL(theApp, start);
}

EBBRC
L1Prim_start(L1Ref _self, uintptr_t startinfo)
{
  EBBRC rc;
  CObjEBBRootSharedRef rootRef;
  L1PrimRef self = (L1PrimRef) _self;

  // initialize the message handler, this will take over the
  // IPI on this core. 
  rc = MsgMgrPrim_Init();
  EBBRCAssert(rc);

  // We now allocate a temporary EBB that can be deleted once
  // we are on the message has been delivered
  rc = CObjEBBRootSharedCreate(&rootRef, 
			       (EBBRepRef) &(self->startMH));
  EBBRCAssert(rc);
  rc = EBBAllocPrimId((EBBId *)&(self->startMHId));
  EBBRCAssert(rc);
  rc = CObjEBBBind((EBBId)self->startMHId, rootRef);
  EBBRCAssert(rc);

#ifdef JAKLUDGE
  rc = COBJ_EBBCALL(self->startMHId, msg0);
  EBBRCAssert(rc);
#endif

  // continue startup for this EL as a message to myself here (on this EL)
  rc = COBJ_EBBCALL(theMsgMgrId, 
		    msg1, MyEL(), self->startMHId, startinfo);
  EBBRCAssert(rc);

  return EBBRC_OK;
}

CObjInterface(L1) L1Prim_ftable = {
  .start = L1Prim_start,
  {
#ifdef JAKLUDGE
    .msg0 = L1Prim_MsgHandler_testMH,
#endif
    .msg1 = L1Prim_MsgHandler_startMH
  }
};

void
setL1PrimFT(L1PrimRef o)
{
  o->ft = &L1Prim_ftable;
  o->startMH.ft = &(L1Prim_ftable.MsgHandler_if);
}

EBBRepRef
L1Prim_createRep(CObjEBBRootMultiRef _self)
{
  L1PrimRef rep;
  EBBRC rc;

  rc = EBBPrimMalloc(sizeof(L1Prim), &rep, EBB_MEM_DEFAULT);
  EBBRCAssert(rc);
  setL1PrimFT(rep);
  // all other initialization logic we leave to the start method

  return (EBBRepRef) rep;
}

EBBRC
L1PrimInit(void)
{
  EBBRC rc = EBBRC_OK;

  if (__sync_bool_compare_and_swap(&theL1Id, (L1Id)0,
				   (L1Id)-1)) {
    CObjEBBRootMultiImpRef rootRef;
    EBBId id;
    rc = CObjEBBRootMultiImpCreate(&rootRef, 
				   L1Prim_createRep);
    EBBRCAssert(rc);
    rc = EBBAllocPrimId(&id);
    EBBRCAssert(rc);
    rc = CObjEBBBind(id, rootRef); 
    EBBRCAssert(rc);
    theL1Id = (L1Id)id;
  } else {
    while (((volatile uintptr_t)theL1Id)==-1);
  }

  return rc;  
}
