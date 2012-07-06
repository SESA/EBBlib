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
#include <inttypes.h>

#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <lrt/io.h>
#include <l0/lrt/trans.h>
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
#include <l0/lrt/event.h>

STATIC_ASSERT(LRT_EVENT_NUM_EVENTS % 8 == 0,
              "num allocatable events isn't divisible by 8");
static uint8_t alloc_table[LRT_EVENT_NUM_EVENTS / 8];

CObject(EventMgrPrimImp){
  CObjInterface(EventMgrPrim) *ft;

  // for now, make this share descriptor tables, may replicate
  // later
  struct lrt_event_descriptor *lrt_event_table_ptr;
};

static EBBRC
EventMgrPrim_allocEventNo(EventMgrPrimRef _self, EventNo *eventNoPtr)
{
  int i;
  //we start from the beginning and just find the first
  // unallocated event
  for (i = 0; i < LRT_EVENT_NUM_EVENTS; i++) {
    uint8_t res = __sync_fetch_and_or(&alloc_table[i / 8], 1 << (i % 8));
    if (!(res & (1 << (i % 8)))) {
      break;
    }
  }
  if (i >= LRT_EVENT_NUM_EVENTS) {
    return EBBRC_OUTOFRESOURCES;
  }
  *eventNoPtr = i;
  return EBBRC_OK;
}

static EBBRC
EventMgrPrim_freeEventNo(EventMgrPrimRef _self, EventNo eventNo)
{
  __sync_fetch_and_and(&alloc_table[eventNo / 8], ~(1 << (eventNo % 8)));
  return EBBRC_OK;
}

// no locking, if you bind while event in flight you are an idiot
static EBBRC
EventMgrPrim_bindEvent(EventMgrPrimRef _self, EventNo eventNo,
                       EBBId handler, EBBFuncNum fn)
{
  EventMgrPrimImpRef self = (EventMgrPrimImpRef)_self;
  self->lrt_event_table_ptr[eventNo].id = handler;
  self->lrt_event_table_ptr[eventNo].fnum = fn;
  return EBBRC_OK;
}

static EBBRC
EventMgrPrim_routeIRQ(EventMgrPrimRef _self, IRQ *isrc, EventNo eventNo,
                      enum EventLocDesc desc, EventLoc el)
{
  lrt_event_route_irq(isrc, eventNo, desc, el);
  return EBBRC_OK;
}

static EBBRC
EventMgrPrim_triggerEvent(EventMgrPrimRef _self, EventNo eventNo,
                          enum EventLocDesc desc, EventLoc el)
{
  LRT_Assert(el < lrt_num_event_loc());
  lrt_event_trigger_event(eventNo, desc, el);
  return EBBRC_OK;
}

static EBBRC
EventMgrPrim_dispatchEvent(EventMgrPrimRef _self, EventNo eventNo)
{
  LRT_Assert(0);		/* shouldn't be called */
  return EBBRC_OK;
}

static EBBRC
EventMgrPrim_enableInterrupts(EventMgrPrimRef _self)
{
  EventMgrPrimImpRef self = (EventMgrPrimImpRef)_self;
  int rc;

  lrt_event_halt();
  rc = lrt_event_get_event_nonblock();
  LRT_Assert(rc >= 0);

  struct lrt_event_descriptor *desc = &self->lrt_event_table_ptr[rc];
  lrt_trans_id id = desc->id;
  lrt_trans_func_num fnum = desc->fnum;

  //this infrastructure should be pulled out of this file
  lrt_trans_rep_ref ref = lrt_trans_id_dref(id);
  ref->ft[fnum](ref);
  return EBBRC_OK;
}

CObjInterface(EventMgrPrim) EventMgrPrimImp_ftable = {
  .allocEventNo = EventMgrPrim_allocEventNo,
  .freeEventNo = EventMgrPrim_freeEventNo,
  .bindEvent = EventMgrPrim_bindEvent,
  .routeIRQ = EventMgrPrim_routeIRQ,
  .triggerEvent = EventMgrPrim_triggerEvent,
  .enableInterrupts = EventMgrPrim_enableInterrupts,
  .dispatchEvent = EventMgrPrim_dispatchEvent
};

static void
EventMgrPrimSetFT(EventMgrPrimImpRef o)
{
  o->ft = &EventMgrPrimImp_ftable;
}

static EBBRep *
EventMgrPrimImp_createRep(CObjEBBRootMultiRef root)
{
  EventMgrPrimImpRef repRef;
  EBBRC rc;
  rc = EBBPrimMalloc(sizeof(EventMgrPrimImp), &repRef, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  EventMgrPrimSetFT(repRef);

  // note we get here with the root object locked, and we are assuming tht
  // in searching for/allocating the event_table.  When we parallelize
  // rep creation this will fail
  EBBRep *rep;
  root->ft->nextRep(root, 0, &rep);
  if (rep != NULL) {
    repRef->lrt_event_table_ptr = ((EventMgrPrimImpRef)rep)->lrt_event_table_ptr;
  } else {
    // allocate the table; reminder this is locked at root
    rc = EBBPrimMalloc(sizeof(struct lrt_event_descriptor)*LRT_EVENT_NUM_EVENTS,
                       &repRef->lrt_event_table_ptr, EBB_MEM_DEFAULT);
  }
  return (EBBRep *)repRef;
}

EBBRC
EventMgrPrimImpInit(void)
{
  EBBRC rc;
  static CObjEBBRootMultiImpRef rootRef;

  if (__sync_bool_compare_and_swap(&theEventMgrPrimId, (EventMgrPrimId)0,
                                   (EventMgrPrimId)-1)) {
    EBBId id;
     rc = CObjEBBRootMultiImpCreate(&rootRef, EventMgrPrimImp_createRep);
    LRT_RCAssert(rc);
    rc = EBBAllocPrimId(&id);
    LRT_RCAssert(rc);
    rc = EBBBindPrimId(id, CObjEBBMissFunc, (EBBMissArg)rootRef);
    LRT_RCAssert(rc);
    theEventMgrPrimId = (EventMgrPrimId)id;
  } else {
    while ((*(volatile uintptr_t *)&theEventMgrPrimId)==-1);
  }
  return EBBRC_OK;
};
