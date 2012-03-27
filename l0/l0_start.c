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
#include <l0/lrt/pic.h>
#include <lrt/assert.h>
#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/MemMgr.h> 
#include <l0/MemMgrPrim.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l1/MsgMgr.h>
#include <l1/L1.h>
#include <l1/L1Prim.h>

extern void trans_init(void);

CObject(ResetEventHandler) {
  COBJ_EBBFuncTbl(EventHandler);

  CObjEBBRootMultiRef theRoot;	
  uintptr_t startInfo;
};

static EBBRC 
ResetEventHandler_handleEvent(EventHandlerRef _self)
{
  EBBRC rc;
  ResetEventHandlerRef self = (ResetEventHandlerRef) _self;
  
  lrt_pic_ackipi();

  // call the next layer startup
  rc = L1PrimInit();
  EBBRCAssert(rc);

  COBJ_EBBCALL(theL1Id, start, self->startInfo);

  lrt_pic_enableipi();

  return 0;
};

static EBBRC
ResetEventHandler_init(EventHandlerRef _self, uintptr_t startInfo)
{
  ResetEventHandlerRef self = (ResetEventHandlerRef)_self;
  self->startInfo = startInfo;
  return 0;
};


static CObjInterface(EventHandler) ResetEventHandler_ftable = {
  .handleEvent = ResetEventHandler_handleEvent,
  .init = ResetEventHandler_init
};

static EBBRep *
ResetEventHandler_createRep(CObjEBBRootMultiRef _self) {
  ResetEventHandlerRef repRef;
  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  repRef->ft = &ResetEventHandler_ftable;
  repRef->theRoot = _self;
  //  initGTable(EventMgrPrimErrMF, 0);
  return (EBBRep *)repRef;
}

static EventHandlerId
InitResetEventHandler(uintptr_t startInfo)
{
  EBBRC rc;
  static EventHandlerId theResetEventHandlerId=0;

  if (__sync_bool_compare_and_swap(&theResetEventHandlerId, (EventHandlerId)0,
				   (EventHandlerId)-1)) {
    EBBId id;
    CObjEBBRootMultiImpRef rootRef;
    rc = CObjEBBRootMultiImpCreate(&rootRef,
				  ResetEventHandler_createRep);
    EBBRCAssert(rc);
    rc = EBBAllocPrimId(&id);
    EBBRCAssert(rc);
    rc = CObjEBBBind(id, rootRef); 
    EBBRCAssert(rc);
    theResetEventHandlerId = (EventHandlerId)id;
  } else {
    while (((volatile uintptr_t)theResetEventHandlerId)==-1);
  }
  rc = COBJ_EBBCALL(theResetEventHandlerId, init, startInfo);
  EBBRCAssert(rc);
  return theResetEventHandlerId;
};


/* 
 * Three main EBB's are EBBMgrPrim, EventMgrPrim EBBMemMgrPrim    
 * There creation and initialization are interdependent and requires 
 * fancy footwork 
 */
void
EBB_init(uintptr_t startInfo)
{
  EBBRC rc;
  EventHandlerId ehid; 

  rc = EBBMemMgrPrimInit();
  EBBRCAssert(rc);

  rc = EBBMgrPrimInit();
  EBBRCAssert(rc);

  // At this point EBBMgr and Ebb Calls should be working
  //  NOMORE USE OF TRANS OR BOOT INTERFACES TO THESE THINGS
  //  AND MOST CODE SHOULD BE ON Ebb's

  rc = EventMgrPrimImpInit();
  EBBRCAssert(rc);

  ehid = InitResetEventHandler(startInfo);
  EBBAssert(ehid != NULL);

  // JA: FIXME:  IS THIS FIRST REAL EBB CALL BELOW ... SHOULD BE EXPLICITLY MARKED
  //             AND THE FACTS THAT THAT DEPENDS ON CLEARLY STATED

  // this sets up, just on the local processor, IPI to temporarily
  // the first reset event.  The handleEvent will do all 
  // the subsequent initialization, now on an event
  COBJ_EBBCALL(theEventMgrPrimId, registerIPIHandler, ehid);
  COBJ_EBBCALL(theEventMgrPrimId, dispatchIPI, MyEL());
  // will fall through
}

void
l0_start(uintptr_t startInfo)
{
  trans_init();
  EBB_init(startInfo);
}

