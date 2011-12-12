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
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>



extern void trans_init(void);

static EBBRC 
ResetEventHandler_handleEvent(void *_self)
{
  EBB_LRT_printf("On reset event\n");
  // then invoke a method of BootInfo object on first message
  // this object should gather boot information (sysfacts and boot args)
  // and then get full blown primitive l0 EBBS up (perhaps by a hot swap)
  EBB_LRT_printf("%s: ADD REST OF INIT CODE HERE!\n", __func__);
  sleep(10);
  LRT_EBBAssert(0);
  return 0;
};
static EBBRC
ResetEventHandler_init(void *_self)
{
  return 0;
};

CObject(ResetEventHandler) {
  CObjInterface(EventHandler) *ft;
  CObjEBBRootMultiRef theRoot;	
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
InitResetEventHandler()
{
  CObjEBBRootMultiImpRef rootRef;
  EventHandlerId id;
  static EventHandlerId theResetEventHandlerId=0;

  if (__sync_bool_compare_and_swap(&theResetEventHandlerId, (EventHandlerId)0,
				   (EventHandlerId)-1)) {
    CObjEBBRootMultiImpCreate(&rootRef, ResetEventHandler_createRep);
    id = (EventHandlerId)EBBIdAlloc();
    EBBAssert(id != NULL);

    EBBIdBind((EBBId)id, CObjEBBMissFunc, (EBBMissArg) rootRef);
    theResetEventHandlerId = id;
  } else {
    while (((volatile uintptr_t)theResetEventHandlerId)==-1);
  }
  return theResetEventHandlerId;
};


/* 
 * Three main EBB's are EBBMgrPrim, EventMgrPrim EBBMemMgrPrim    
 * There creation and initialization are interdependent and requires 
 * fancy footwork 
 */
void
EBB_init()
{
  EBBRC rc;
  EventHandlerId ehid; 

  rc = EBBMemMgrPrimInit();
  EBBRCAssert(rc);

  // FIXME: no error?
  EBBMgrPrimInit();

  rc = EventMgrPrimImpInit();
  EBBRCAssert(rc);

  ehid = InitResetEventHandler();
  EBBAssert(ehid != NULL);

  COBJ_EBBCALL(theEventMgrPrimId, registerIPIHandler, ehid);
  COBJ_EBBCALL(theEventMgrPrimId, dispatchIPI, MyEL());
  // will fall through
}

void
l0_start(void)
{
  EBB_LRT_printf("%s: started!\n", __func__);
  trans_init();
  EBB_init();
}

