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
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/MsgMgr.h>
#include <l1/MsgMgrPrim.h>
#include <l1/App.h>

// write a simple message handler
// make call to the message handler
// figure out number of cores, and send messages between them

static EBBRC 
MsgHandlerTst_msg0(MsgHandlerRef _self)
{
  // ack that we are handing interrupt
  return EBBRC_OK;
};
static EBBRC 
MsgHandlerTst_msg1(MsgHandlerRef _self, uintptr_t a1)
{
  // ack that we are handing interrupt
  lrt_printf("[%s]", __func__);
  return EBBRC_OK;
};

static EBBRC 
MsgHandlerTst_msg2(MsgHandlerRef _self, uintptr_t numtosend, uintptr_t id)
{
  uintptr_t nxt = 0;
  MsgHandlerId sid = (MsgHandlerId)id;
  EBBRC rc;

  numtosend--;
  if (numtosend <= 0) {
    lrt_exit(0);
  }

  LRT_Assert(numtosend > 0);
  nxt = NextEventLoc(MyEventLoc());
  
  do {
    rc = COBJ_EBBCALL(theMsgMgrId, msg2, nxt, sid, numtosend, id);
    //if (rc == EBBRC_NOTFOUND) lrt_printf("*");

  } while (rc == EBBRC_NOTFOUND);
  return EBBRC_OK;
};
static EBBRC 
MsgHandlerTst_msg3(MsgHandlerRef _self, uintptr_t a1, uintptr_t a2, 
		   uintptr_t a3)
{
  // ack that we are handing interrupt
  lrt_printf("[%s]", __func__);
  return EBBRC_OK;
};


CObject(MsgHandlerTst) {
  CObjInterface(MsgHandler) *ft;
  CObjEBBRootMultiRef theRoot;	
};


static CObjInterface(MsgHandler) MsgHandlerTst_ftable = {
  .msg0 = MsgHandlerTst_msg0,
  .msg1 = MsgHandlerTst_msg1,
  .msg2 = MsgHandlerTst_msg2,
  .msg3 = MsgHandlerTst_msg3,
};

static EBBRep *
MsgHandlerTst_createRep(CObjEBBRootMultiRef _self) {
  MsgHandlerTstRef repRef;
  EBBPrimMalloc(sizeof(MsgHandlerTst), &repRef, EBB_MEM_DEFAULT);
  repRef->ft = &MsgHandlerTst_ftable;
  repRef->theRoot = _self;
  return (EBBRep *)repRef;
}

static MsgHandlerId
InitMsgHandlerTst()
{
  static MsgHandlerId theMsgHandlerTstId=0;

  if (__sync_bool_compare_and_swap(&theMsgHandlerTstId, (MsgHandlerId)0,
				   (MsgHandlerId)-1)) {
    EBBRC rc;
    CObjEBBRootMultiImpRef rootRef;
    EBBId id;
    rc = CObjEBBRootMultiImpCreate(&rootRef, MsgHandlerTst_createRep);
    LRT_RCAssert(rc);
    rc = EBBAllocPrimId(&id);
    LRT_RCAssert(rc);
    rc = CObjEBBBind(id, rootRef); 
    LRT_RCAssert(rc);
    theMsgHandlerTstId = (MsgHandlerId)id;
  } else {
    while (((volatile uintptr_t)theMsgHandlerTstId)==-1);
  }
  return theMsgHandlerTstId;
};


CObject(MsgTst) {
  CObjInterface(App) *ft;
};


EBBRC 
MsgTst_start(AppRef _self)
{
  MsgHandlerId id = InitMsgHandlerTst();
  int numtosend = 100;
  EBBRC rc;

  lrt_printf("MsgTst, core %d number of cores %d", MyEventLoc(), 
	     NumEventLoc());

  // initialize the message handler, this will take over the
  // IPI on this core. 
  rc = MsgMgrPrim_Init();
  LRT_RCAssert(rc);

  if (MyEventLoc() != 0) {
    lrt_printf("MsgTst, core %d returning to event loop", MyEventLoc());
    return EBBRC_OK;
  }
  
  // kick off message send
  COBJ_EBBCALL(theMsgMgrId, msg2, 0, id, numtosend, (uintptr_t)id);

  return EBBRC_OK;
  
}

CObjInterface(App) MsgTst_ftable = {
  .start = MsgTst_start
};

APP(MsgTst, APP_START_ALL);
