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
#include <l0/EventMgrPrimExp.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l0/lrt/event.h>
#include <sync/bitvector.h>
#include <lrt/string.h>

STATIC_ASSERT(LRT_EVENT_NUM_EVENTS % 8 == 0,
              "num allocatable events isn't divisible by 8");
static uint8_t alloc_table[LRT_EVENT_NUM_EVENTS / 8];

// define a set of routines to manipulate a bitvector
DEF_BITVEC(event, LRT_EVENT_NUM_EVENTS);

CObject(EventMgrPrimExpImp){
  CObjInterface(EventMgrPrimExp) *ft;

  // bitvector in each rep of events pending
  struct event_bvs corebv;

  // array of event mgrs
  EventMgrPrimExpImpRef *reps;

  // for now, make this share descriptor tables, may replicate
  // later
  struct lrt_event_descriptor *lrt_event_table_ptr;
  // the root for this object
  CObjEBBRootMultiRef theRoot;
  EventLoc eventLoc;
};

static inline EventMgrPrimExpImpRef
findTarget(EventMgrPrimExpImpRef self, EventLoc loc)
{
  RepListNode *node;
  EBBRep * rep = NULL;

  while (1) {
    rep = (EBBRep *)self->reps[loc];
    if (rep == NULL) {
      for (node = self->theRoot->ft->nextRep(self->theRoot, 0, &rep);
           node != NULL;
           node = self->theRoot->ft->nextRep(self->theRoot, node, &rep)) {
        LRT_Assert(rep != NULL);
        if (((EventMgrPrimExpImpRef)rep)->eventLoc == loc) break;
      }
      self->reps[loc] = (EventMgrPrimExpImpRef)rep;
    }
    // FIXME: handle case that rep doesn't yet exist
    if (rep != NULL) {
      return (EventMgrPrimExpImpRef)rep;;
    }
    lrt_printf("x");
  }
  LRT_Assert(0);		/* can't get here */
}

static EBBRC
EventMgrPrimExpImp_allocEventNo(EventMgrPrimRef _self, EventNo *eventNoPtr)
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
EventMgrPrimExpImp_freeEventNo(EventMgrPrimRef _self, EventNo eventNo)
{
  __sync_fetch_and_and(&alloc_table[eventNo / 8], ~(1 << (eventNo % 8)));
  return EBBRC_OK;
}

static EBBRC
EventMgrPrimExpImp_bindEvent(EventMgrPrimRef _self, EventNo eventNo,
          EBBId handler, EBBFuncNum fn)
{
  EventMgrPrimExpImpRef self = (EventMgrPrimExpImpRef)_self;
  self->lrt_event_table_ptr[eventNo].id = handler;
  self->lrt_event_table_ptr[eventNo].fnum = fn;
  return EBBRC_OK;
}

static EBBRC
EventMgrPrimExpImp_routeIRQ(EventMgrPrimRef _self, IRQ *isrc, EventNo eventNo,
                      enum EventLocDesc desc, EventLoc el)
{
  LRT_Assert(0);
  return EBBRC_OK;
}

static EBBRC
EventMgrPrimExpImp_triggerEvent_nobv(EventMgrPrimRef _self, EventNo eventNo,
                                     enum EventLocDesc desc, EventLoc el)
{
  LRT_Assert(el < lrt_num_event_loc());
  lrt_event_trigger_event(eventNo, desc, el);
  return EBBRC_OK;
}

static EBBRC
EventMgrPrimExpImp_dispatchEvent(EventMgrPrimRef _self, EventNo eventNo)
{
  EventMgrPrimExpImpRef self = (EventMgrPrimExpImpRef)_self;
  struct lrt_event_descriptor *desc = &self->lrt_event_table_ptr[eventNo];
  lrt_trans_id id = desc->id;
  lrt_trans_func_num fnum = desc->fnum;

  //this infrastructure should be pulled out of this file
  lrt_trans_rep_ref ref = lrt_trans_id_dref(id);
  ref->ft[fnum](ref);
  return EBBRC_OK;
}

static EBBRC
EventMgrPrimExpImp_enableInterrupts_nobv(EventMgrPrimRef _self)
{
  asm volatile ("sti\n\t"
                "hlt\n\t"
                "cli"
                ::
                : "rax", "rcx", "rdx", "rsi",
                  "rdi", "r8", "r9", "r10", "r11");
  return EBBRC_OK;
}

static EBBRC
EventMgrPrimExpImp_enableInterrupts_withbv(EventMgrPrimRef _self)
{
  // first check local bitvector, then block at low level
  EventMgrPrimExpImpRef self = (EventMgrPrimExpImpRef)_self;
  int el = event_get_unset_bit_bv(&self->corebv);
  if (el != -1) {
    return EventMgrPrimExpImp_dispatchEvent(_self, el);
  } else {
    // okay, really enable interrupts
    return EventMgrPrimExpImp_enableInterrupts_nobv(_self);
  }
}

static EBBRC
EventMgrPrimExpImp_triggerEvent_withlbv(EventMgrPrimRef _self, EventNo eventNo,
                                  enum EventLocDesc desc, EventLoc el)
{
  EventMgrPrimExpImpRef self = (EventMgrPrimExpImpRef)_self;
  LRT_Assert(el < lrt_num_event_loc());

  if (el == lrt_my_event_loc()) {
    event_set_bit_bv(&self->corebv, eventNo) ;
    return EBBRC_OK;
  }

  // okay, its a remote bit, do the standard operation
  return EventMgrPrimExpImp_triggerEvent_nobv(_self, eventNo, desc, el);
}

static EBBRC
EventMgrPrimExpImp_triggerEvent_withabv(EventMgrPrimRef _self, EventNo eventNo,
                                        enum EventLocDesc desc, EventLoc el)
{
  int local = 1;

  EventMgrPrimExpImpRef rep = (EventMgrPrimExpImpRef)_self;
  LRT_Assert(el < lrt_num_event_loc());

  // if its not local, find the remote rep
  if (el != lrt_my_event_loc()) {
    rep = findTarget(rep, el);
    local = 0;
  }

  event_set_bit_bv(&rep->corebv, eventNo) ;

  // don't wakeup if local
  if (local) return EBBRC_OK;

  // okay, its a remote bit, do the standard operation
  return EventMgrPrimExpImp_triggerEvent_nobv(_self, eventNo, desc, el);
}


static EBBRC EventMgrPrimExpImp_enableBitvectorLocal(EventMgrPrimExpRef self);
static EBBRC EventMgrPrimExpImp_enableBitvectorAll(EventMgrPrimExpRef self);
static EBBRC EventMgrPrimExpImp_disableBitvector(EventMgrPrimExpRef self);
static EBBRC EventMgrPrimExpImp_enablePoll(EventMgrPrimExpRef self);
static EBBRC EventMgrPrimExpImp_disablePoll(EventMgrPrimExpRef self);

CObjInterface(EventMgrPrimExp) EventMgrPrimExpImp_ftable_nobv = {
  {
    .allocEventNo = EventMgrPrimExpImp_allocEventNo,
    .freeEventNo = EventMgrPrimExpImp_freeEventNo,
    .bindEvent = EventMgrPrimExpImp_bindEvent,
    .routeIRQ = EventMgrPrimExpImp_routeIRQ,
    .triggerEvent = EventMgrPrimExpImp_triggerEvent_nobv,
    .enableInterrupts = EventMgrPrimExpImp_enableInterrupts_nobv,
    .dispatchEvent = EventMgrPrimExpImp_dispatchEvent
  },
  .enableBitvectorLocal = EventMgrPrimExpImp_enableBitvectorLocal,
  .disableBitvector = EventMgrPrimExpImp_disableBitvector,
  .enableBitvectorAll = EventMgrPrimExpImp_enableBitvectorAll,
  .disableBitvector = EventMgrPrimExpImp_disableBitvector,
  .enablePoll = EventMgrPrimExpImp_enablePoll,
  .disablePoll = EventMgrPrimExpImp_disablePoll,
};

static EBBRC
EventMgrPrimExpImp_enableBitvectorLocal(EventMgrPrimExpRef self)
{
  self->ft->EventMgrPrim_if.triggerEvent =
    EventMgrPrimExpImp_triggerEvent_withlbv;
  self->ft->EventMgrPrim_if.enableInterrupts =
    EventMgrPrimExpImp_enableInterrupts_withbv;
  return EBBRC_OK;
}

static EBBRC
EventMgrPrimExpImp_enableBitvectorAll(EventMgrPrimExpRef self)
{
  self->ft->EventMgrPrim_if.triggerEvent =
    EventMgrPrimExpImp_triggerEvent_withabv;
  self->ft->EventMgrPrim_if.enableInterrupts =
    EventMgrPrimExpImp_enableInterrupts_withbv;
  return EBBRC_OK;
}

static EBBRC
EventMgrPrimExpImp_enablePoll(EventMgrPrimExpRef self)
{
  LRT_Assert("NYI\n");
  return EBBRC_OK;
}

static EBBRC
EventMgrPrimExpImp_disableBitvector(EventMgrPrimExpRef self)
{
  self->ft->EventMgrPrim_if.triggerEvent =
    EventMgrPrimExpImp_triggerEvent_nobv;
  self->ft->EventMgrPrim_if.enableInterrupts =
    EventMgrPrimExpImp_enableInterrupts_nobv;
  return EBBRC_OK;
}

static EBBRC
EventMgrPrimExpImp_disablePoll(EventMgrPrimExpRef self)
{
  return EBBRC_OK;
}


static void
EventMgrPrimSetFT(EventMgrPrimExpImpRef o)
{
  o->ft = &EventMgrPrimExpImp_ftable_nobv;
}

static EBBRep *
EventMgrPrimExpImp_createRep(CObjEBBRootMultiRef root)
{
  EventMgrPrimExpImpRef repRef;
  EBBRC rc;
  rc = EBBPrimMalloc(sizeof(EventMgrPrimExpImp), &repRef, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  EventMgrPrimSetFT(repRef);
  repRef->theRoot = root;
  repRef->eventLoc = lrt_my_event_loc();

  // FIXME: put in bitvenctor
  bzero(&repRef->corebv, sizeof(repRef->corebv));

  // allocate memory for array of reps
  rc = EBBPrimMalloc(sizeof(repRef->reps)*lrt_num_event_loc(), &repRef->reps,
                     EBB_MEM_DEFAULT);

  // note we get here with the root object locked, and we are assuming tht
  // in searching for/allocating the event_table.  When we parallelize
  // rep creation this will fail
  EBBRep *rep;
  root->ft->nextRep(root, 0, &rep);
  if (rep != NULL) {
    repRef->lrt_event_table_ptr = ((EventMgrPrimExpImpRef)rep)->lrt_event_table_ptr;
  } else {
    // allocate the table; reminder this is locked at root
    rc = EBBPrimMalloc(sizeof(struct lrt_event_descriptor)*LRT_EVENT_NUM_EVENTS,
                       &repRef->lrt_event_table_ptr, EBB_MEM_DEFAULT);
  }

  return (EBBRep *)repRef;
}

EBBRC
EventMgrPrimExpInit(void)
{
  EBBRC rc;
  static CObjEBBRootMultiImpRef rootRef;

  if (__sync_bool_compare_and_swap(&theEventMgrPrimId, (EventMgrPrimId)0,
                                   (EventMgrPrimId)-1)) {
    EBBId id;
     rc = CObjEBBRootMultiImpCreate(&rootRef, EventMgrPrimExpImp_createRep);
    LRT_RCAssert(rc);
    rc = EBBAllocPrimId(&id);
    LRT_RCAssert(rc);
    rc = EBBBindPrimId(id, CObjEBBMissFunc, (EBBArg)rootRef);
    LRT_RCAssert(rc);
    theEventMgrPrimId = (EventMgrPrimId)id;
  } else {
    while ((*(volatile uintptr_t *)&theEventMgrPrimId)==-1);
  }
  return EBBRC_OK;
};
