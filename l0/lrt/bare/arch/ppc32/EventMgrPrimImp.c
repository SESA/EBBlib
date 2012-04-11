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
#include <stdbool.h>

#include <l0/EBBMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/MemMgrPrim.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/lrt/bare/arch/ppc32/pic.h>

static EventHandlerId IPI_handler;
static FuncNum IPI_fn;

CObject(EventMgrPrimImp){
  CObjInterface(EventMgrPrim) *ft;
};

EventMgrPrimId theEventMgrPrimId=0;

static EBBRC
EventMgrPrim_dispatchIPI(EventMgrPrimRef _self, EvntLoc el)
{
  if (el != MyEL()) {
    lrt_printf("%s: sending remote IPI to node %" PRIdPTR "\n", 
		   __func__,
		   el);
  }
  lrt_pic_ipi(el);
  return EBBRC_OK;
}

/*
 * This should go away once we have proper implementation of vector
 * function that should inline this.  Should also buy stack here...
 */
static EBBRC
EventMgrPrim_dispatchEventLocal(EventMgrPrimRef _self, uintptr_t eventNo) 
{
  lrt_printf("%s: NYI\n", __func__);
  LRT_Assert(0);
  return EBBRC_OK;   
}

static EBBRC
EventMgrPrim_ackIPI(EventMgrPrimRef _self)
{
  lrt_pic_ackipi();
  return EBBRC_OK;
}

static EBBRC
EventMgrPrim_enableIPI(EventMgrPrimRef _self)
{
  lrt_pic_enableipi();
  return EBBRC_OK;
}

static EBBRC
EventMgrPrim_registerHandler(EventMgrPrimRef _self, uintptr_t eventNo, 
			     EventHandlerId handler,  FuncNum fn,
			     lrt_pic_src *isrc)
{
  lrt_printf("%s: NYI\n", __func__);
  LRT_Assert(0);
  return EBBRC_OK;   
}

static EBBRC
EventMgrPrim_eventEnable(EventMgrPrimRef _self, uintptr_t eventNo)
{
  lrt_printf("%s: NYI\n", __func__);
  LRT_Assert(0);
  return EBBRC_OK;   
}

static EBBRC
EventMgrPrim_eventDisable(EventMgrPrimRef _self, uintptr_t eventNo)
{
  lrt_printf("%s: NYI\n", __func__);
  LRT_Assert(0);
  return EBBRC_OK;   
}

/* 
 * IPI is different from other handlers, since it is a purely local
 * operation, i.e., both at the pic and in the EventMgr we are going
 * to a different handler for IPIs on each processor.  This is
 * necessary since we remap IPIs on different processors as we go
 * through the initialization.  I assume, for now, that all other
 * interrupts are globally allocated and are the same on all ELs
 * (Event Locations).
 */
static EBBRC
EventMgrPrim_registerIPIHandler(EventMgrPrimRef _self, 
				EventHandlerId handler, FuncNum fn)
{
  IPI_handler = handler;
  IPI_fn = fn;

  return EBBRC_OK;
}

static EBBRC 
EventMgrPrim_allocEventNo(EventMgrPrimRef _self, uintptr_t *eventNoPtr)
{
  lrt_printf("%s: NYI\n", __func__);
  LRT_Assert(0);
  return EBBRC_OK;   
}

CObjInterface(EventMgrPrim) EventMgrPrimImp_ftable = {
  .registerHandler = EventMgrPrim_registerHandler, 
  .eventEnable = EventMgrPrim_eventEnable,
  .eventDisable = EventMgrPrim_eventDisable,
  .registerIPIHandler = EventMgrPrim_registerIPIHandler, 
  .allocEventNo = EventMgrPrim_allocEventNo, 
  .dispatchIPI = EventMgrPrim_dispatchIPI,
  .ackIPI = EventMgrPrim_ackIPI,
  .enableIPI = EventMgrPrim_enableIPI,
  .dispatchEventLocal = EventMgrPrim_dispatchEventLocal
};

static void
EventMgrPrimSetFT(EventMgrPrimImpRef o)
{
  o->ft = &EventMgrPrimImp_ftable;
}

static EBBRep *
EventMgrPrimImp_createRepAssert(CObjEBBRootMultiRef root) 
{
  LRT_Assert(0);
  return NULL;
}

static EventMgrPrimImpRef
EventMgrPrimImp_createRep(CObjEBBRootMultiImpRef root) 
{
  EventMgrPrimImpRef repRef;
  EBBRC rc;

  rc = EBBPrimMalloc(sizeof(EventMgrPrimImp), &repRef, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);
  EventMgrPrimSetFT(repRef);
  return repRef;
}

static void
external_interrupt_handler(void)
{
  //FIXME: Assume IPI
  LRT_Assert(IPI_handler != NULL);
  
  if (IPI_fn==NOFUNCNUM) COBJ_EBBCALL(IPI_handler, handleEvent); 
  else COBJ_EBBCALL_FUNCNUM(GenericEventFunc, (EBBBaseId)IPI_handler, IPI_fn);
}

EBBRC
EventMgrPrimImpInit(void)
{
  EBBRC rc;
  static CObjEBBRootMultiImpRef rootRef;
  EventMgrPrimImpRef repRef;
  EvntLoc myel;

  EBBId id;
  rc = CObjEBBRootMultiImpCreate(&rootRef, EventMgrPrimImp_createRepAssert);
  LRT_RCAssert(rc);
  rc = EBBAllocPrimId(&id);
  LRT_RCAssert(rc);
  rc = EBBBindPrimId(id, CObjEBBMissFunc, (EBBMissArg)rootRef);
  LRT_RCAssert(rc);
  theEventMgrPrimId = (EventMgrPrimId)id;

  // It makes no sense to handle miss on this object lazily, since it will 
  // always be invoked on every node, everything is in an event
  repRef = EventMgrPrimImp_createRep(rootRef);
  myel = MyEL();
  
  rootRef->ft->addRepOn((CObjEBBRootMultiRef)rootRef, myel, (EBBRep *)repRef);

  lrt_pic_mapvec(IV_external, external_interrupt_handler);
  return EBBRC_OK;
};
