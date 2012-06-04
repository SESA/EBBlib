/*
 * Copyright (C) 2012 by Project SESA, Boston University
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

#include <stdbool.h>
#include <stdint.h>

#include <l0/EventMgrPrim.h>
#include <l1/App.h>
#include <lrt/exit.h>
#include <lrt/io.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/cobj/CObjEBBUtils.h>
// include this to get configuration flags
#include <l0/lrt/event.h>

static inline uint64_t
rdtscp(void)
{
  uint32_t lo, hi;
  asm volatile("rdtscp"
               : "=a" (lo),
                 "=d" (hi)
               :
               :
               "ecx");
  return (uint64_t)hi << 32 | lo;
}

CObject(EventTiming) {
  CObjInterface(EventTiming) *ft;
};

CObjInterface(EventTiming) {
  CObjImplements(App);
  EBBRC (*loopLocalEvent) (EventTimingRef self);
  EBBRC (*roundRobinEvent) (EventTimingRef self);
};

uint64_t t0, t1, t2;
int count;
EventNo ev;

EBBRC runNextTest();

static void
resetCounters() {
  lrt_event_dispatched_events = 0;
  lrt_event_bv_dispatched_events = 0;
}

static void
printCounters() {
  lrt_printf("\t dispatched %ld dispatched bv %ld\n",
	     (long int)(lrt_event_dispatched_events), 
	     (long int)(lrt_event_bv_dispatched_events));
}
      

EventLoc next;

static EBBRC
EventTiming_roundRobinEvent(EventTimingRef self)
{
  if (next != MyEventLoc()) {
    // duplicate event
    return EBBRC_OK;
  }

  if (count == -1) {
    resetCounters();
    t0 = rdtscp();
  }
    

  int numcores = NumEventLoc();
  if (MyEventLoc() == 0) {
    if (count >= numcores*1000) {
      t1 = rdtscp();
      lrt_printf("\t count %d\n", count);
      lrt_printf("\t round robin total %ld per lp %ld\n",
		 (long int)(t1 - t0), (long int)(t1 - t0)/(count));
      printCounters();
      runNextTest();
      return EBBRC_OK;
    }
  }

  next = NextEventLoc(next);
  count++;
  COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev,
	       EVENT_LOC_SINGLE, next);
  return EBBRC_OK;
}

static EBBRC
EventTiming_loopLocalEvent(EventTimingRef self)
{
  if (count == 1000) {
    t1 = rdtscp();
    lrt_printf("\t local total %ld per lp %ld\n",
	       (long int)(t1 - t0), (long int)((t1 - t0)/(1000)));
    printCounters();
    runNextTest();
    return EBBRC_OK;
  } else if (count == 0) {
    t0 = rdtscp();
  }
  count++;

  COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev,
               EVENT_LOC_SINGLE, MyEventLoc());

  return EBBRC_OK;
}

COBJ_EBBType(TestObj) {
  EBBRC (*nullCall) (TestObjRef self);
};

CObject(TestObjImp)
{
  COBJ_EBBFuncTbl(TestObj);
};

static EBBRC
TestObjImp_nullCall(TestObjRef _self)
{
  return EBBRC_OK;
}

CObjInterface(TestObj) TestObjImp_ftable = {
  .nullCall = TestObjImp_nullCall
};

void
TestObjImpSetFT(TestObjImpRef o)
{
  o->ft = &(TestObjImp_ftable);
}

TestObjId toid;

EBBRC
testObjImpCreate()
{
  TestObjImpRef repRef;
  CObjEBBRootSharedRef rootRef;
  EBBRC rc;

  rc = EBBPrimMalloc(sizeof(TestObjImp), &repRef, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);
  TestObjImpSetFT(repRef);

  rc = CObjEBBRootSharedCreate(&rootRef, (EBBRepRef)repRef);
  LRT_RCAssert(rc);

  rc = EBBAllocPrimId((EBBId *)&toid);
  LRT_RCAssert(rc);

  rc = EBBBindPrimId((EBBId)toid, CObjEBBMissFunc, (EBBMissArg)rootRef);
  LRT_RCAssert(rc);

  return EBBRC_OK;
}


EBBRC
testEBBCall()
{
  int i;
  EBBRC rc;
  
  rc = testObjImpCreate();
  LRT_RCAssert(rc);

  t0 = rdtscp();
  COBJ_EBBCALL(toid, nullCall);
  t1 = rdtscp();
  for(i=0;i<1000;i++) {
    COBJ_EBBCALL(toid, nullCall);
  }
  t2 = rdtscp();
  lrt_printf("\t first EBB call %ld subsequent avg %ld\n",
	     (long int)(t1 - t0), (long int)(t2 - t1)/(1000));
  return EBBRC_OK;
}

EBBRC
testTimerOverhead()
{
  int i;
  t0 = rdtscp();
  for(i=0;i<10000;i++) {
    t1= rdtscp();
  }
  t2 = rdtscp();
  lrt_printf("\t avt time to read timer %ld\n",
	     (long int)(t2 - t0)/(10000));
  return EBBRC_OK;
}

/*
 * tests executed in context of event
 * initiated by previous tests
 */
EBBRC
runNextTest()
{
  static int testStage = 0;
  count = 0;
  EBBRC rc;
  resetCounters();
  switch(testStage) {
  case 0: 
    // tests that run to completion
    testStage++;
    lrt_printf("eventtiming: base overheads\n");
    testEBBCall();
    testTimerOverhead();
    // falls into tests that create events
    // and call runNextTest
  case 1: 
    testStage++;
    rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, ev, (EBBId)theAppId,
		      COBJ_FUNCNUM_FROM_TYPE(CObjInterface(EventTiming), 
					     loopLocalEvent));
    LRT_RCAssert(rc);
    
    lrt_event_use_bitvector_local=0;
    count = 0;
    lrt_printf("eventtiming: running local event loop with BV disabled\n");
    rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev,
		      EVENT_LOC_SINGLE, MyEventLoc());
    LRT_RCAssert(rc);
    break;
  case 2:
    testStage++;
    lrt_event_use_bitvector_local=1;
    count = 0;
    lrt_printf("eventtiming: running local event loop with BV enabled\n");
    rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev,
		      EVENT_LOC_SINGLE, MyEventLoc());
    LRT_RCAssert(rc);
    break;
  case 3:
    testStage++;
    count = -1;
    next = 0;
    lrt_event_use_bitvector_local=0;
    lrt_event_use_bitvector_remote=0;
    lrt_printf("eventtiming: running remote event loop with BV disabled\n");
    rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, ev, (EBBId)theAppId,
                            COBJ_FUNCNUM_FROM_TYPE(CObjInterface(EventTiming),
                                                   roundRobinEvent));
    LRT_RCAssert(rc);
    rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev,
		      EVENT_LOC_SINGLE, MyEventLoc());
    {
      //block for a while to let other cores halt
      uint64_t time = rdtscp();
      while ((rdtscp() - time) < 1000000)
	;
    }
    LRT_RCAssert(rc);
    break;
  case 4:
    testStage++;
    count = 0;
    resetCounters();
    lrt_event_use_bitvector_remote=1;
    lrt_printf("eventtiming: running remote event loop with BV enabled\n");
    rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, ev, (EBBId)theAppId,
                            COBJ_FUNCNUM_FROM_TYPE(CObjInterface(EventTiming),
                                                   roundRobinEvent));
    LRT_RCAssert(rc);
    rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev,
		      EVENT_LOC_SINGLE, MyEventLoc());
    LRT_RCAssert(rc);
    break;
  default:
    lrt_exit(0);
    break;
  }
  return EBBRC_OK;
}


static EBBRC
EventTiming_start(AppRef _self)
{

  //block for a while to let other cores halt
  uint64_t time = rdtscp();
  lrt_printf("eventtiming: started\n");
  while ((rdtscp() - time) < 1000000)
    ;

  EBBRC rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &ev);
  LRT_RCAssert(rc);

  runNextTest();

  return EBBRC_OK;
}


CObjInterface(EventTiming) EventTiming_ftable = {
  .App_if = {
    .start = EventTiming_start
  },
  .loopLocalEvent = EventTiming_loopLocalEvent,
  .roundRobinEvent = EventTiming_roundRobinEvent
};

APP(EventTiming, APP_START_ONE);
