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

#include <config.h>

#include <stdbool.h>
#include <stdint.h>

#include <arch/cpu.h>
#include <l0/EventMgrPrim.h>
#include <l1/App.h>
#include <lrt/exit.h>
#include <lrt/io.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/cobj/CObjEBBUtils.h>
// include this to get configuration flags
#include <l0/lrt/event.h>

int verbose=0;

CObject(EventTiming) {
  CObjInterface(EventTiming) *ft;
};

CObjInterface(EventTiming) {
  CObjImplements(App);
  EBBRC (*loopEvent) (EventTimingRef self);
};

static const int max_iteration=100;
static const int max_count=1000;


EventNo ev;

EBBRC runNextTest();

int bogus_events=0;
static void
resetCounters() {
  bogus_events = 0;
}

static void
printCounters() {
  if (bogus_events) {
    lrt_printf("\t got %d bogus events\n", bogus_events);
  }
}

// controls for event loop
EventLoc next;			/* next core to wake up */
EventLoc ping_r;		/* remote core for ping test */
enum {LOCAL, LOCAL_INT, RR_ALL, PING} event_loop_type;

// stuff used everywhere
int numcores;

// variables used in loopEvent
int count;
int iteration;

static EBBRC
EventTiming_loopEvent(EventTimingRef self)
{
  static uint64_t t0, t1;
  static uint64_t max_avg, min_avg, total, tot_count;
#ifdef LRT_EVENT_COLLECT_INT_TIMING
  // for low level monitoring fine grained timers
  static uint64_t min1, min2, min3;
  static uint64_t max1, max2, max3;
  static uint64_t avg1, avg2, avg3;
#endif

  if (next != MyEventLoc()) {
    bogus_events++;
    return EBBRC_OK;
  }
  if (count == -1) {
    if (iteration == -1) {
      // start of the entire experiment
      resetCounters();
      max_avg = total = tot_count = 0;
      min_avg = UINT64_MAX;
      count = 0;
      iteration = 0;
      // set any initial conditions based on experiment
      LRT_Assert(MyEventLoc() == 0);
      switch(event_loop_type) {
      case LOCAL_INT:
#ifdef LRT_EVENT_COLLECT_INT_TIMING
	min1 = min2 = min3 = UINT64_MAX;
	max1 = max2 = max3 = 0;
	avg1 = avg2 = avg3 = 0;
	lrt_printf("eventtiming: running local fine grained timing test\n");
	lrt_event_use_bitvector_local=1;
	lrt_event_use_bitvector_remote=0;
	lrt_event_collect_int_timing = 1;
	next = 0;
#endif
	break;
      case LOCAL:
	break;
      case PING:
	break;
      case RR_ALL:
	break;
      }
    } 
    t0 = rdtscp();
  } else {
#ifdef LRT_EVENT_COLLECT_INT_TIMING
    if (lrt_event_collect_int_timing) {
      uint64_t d;
      if ((tint1<tint0)||(tint2<tint1)||(tint3<tint2)) { 
	lrt_printf("1 - %ld - %ld - %ld - %ld\n", 
		   (unsigned long)tint0, 
		   (unsigned long)tint1, 
		   (unsigned long)tint2, 
		   (unsigned long)tint3);
      }
      LRT_Assert(tint1>=tint0);
      LRT_Assert(tint2>=tint1);
      LRT_Assert(tint3>=tint2);
      d = tint1 - tint0;
      if (d < min1) min1 = d;
      if (d > max1) max1 = d;
      avg1 += d;
      d = tint2 - tint1;
      if (d < min2) min2 = d;
      if (d > max2) max2 = d;
      avg2 += d;
      d = tint3 - tint2;
      if (d < min3) min3 = d;
      if (d > max3) max3 = d;
      avg3 += d;
    }
#endif
  }
    
  if (count == max_count) {	/* done an iteration */
#ifdef LRT_EVENT_COLLECT_INT_TIMING
  if (lrt_event_collect_int_timing) {
    lrt_printf("\texp 1 min = %ld, max = %ld, avg = %ld\n", 
	       (long int)min1, (long int)max1, (long int)avg1/max_count);
    lrt_printf("\texp 2 min = %ld, max = %ld, avg = %ld\n", 
	       (long int)min2, (long int)max2, (long int)(avg2/max_count));
    lrt_printf("\texp 3 min = %ld, max = %ld, avg = %ld\n", 
	       (long int)min3, (long int)max3, (long int)avg3/max_count);
    lrt_exit(0);
  }
#endif
    uint64_t ctot, cavg;
    t1 = rdtscp();
    LRT_Assert(t1>t0);
    
    ctot = t1-t0;
    cavg = ctot/count;

    // fix global stats
    tot_count += count;
    total += ctot;
    if (max_avg < cavg) max_avg = cavg;
    if (min_avg > cavg) min_avg = cavg;

    if (iteration == max_iteration) { // done it all
      if (verbose) {
	lrt_printf("eventtiming: ran ping to core %ld BV disabled\n",
		   (long int)ping_r);
	lrt_printf("\t"
		   " tot_count %ld "
		   " total %ld \n"
		   "\t"
		   " avg %ld "
		   " max_avg %ld "
		   " min_avg %ld \n",
		   (long int)tot_count, (long int)total, 
		   (long int)(total/tot_count), (long int)max_avg, 
		   (long int)min_avg );
	printCounters();
      } else {
	// denser printing
	lrt_printf("\t ping to core %ld "
		   " avg %ld "
		   " max_avg %ld "
		   " min_avg %ld \n",
		   (long int)ping_r, 
		   (long int)(total/tot_count), (long int)max_avg, 
		   (long int)min_avg );
      }
      runNextTest();
      return EBBRC_OK;
    }

    // setup for next iteration
    count = -1;
    iteration++;
    next = 0;
    COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev, EVENT_LOC_SINGLE, 0);
    return EBBRC_OK;
  }

  switch(event_loop_type) {
  case LOCAL_INT:
  case LOCAL:
    next = 0;
    break;
  case PING:
    if (MyEventLoc() == 0) {
      next = ping_r;
    } else {
      next = 0;
    }
    break;
  case RR_ALL:
    next = NextEventLoc(next);
    break;
  }

  count++;
#ifdef LRT_EVENT_COLLECT_INT_TIMING
  if (lrt_event_collect_int_timing) {
    tint0 = rdtscp();
  }
#endif
  COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev,
	       EVENT_LOC_SINGLE, next);
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
  int i, j;
  EBBRC rc;
  static uint64_t t0, t1;
  static uint64_t max_avg, min_avg, total;
  
  rc = testObjImpCreate();
  LRT_RCAssert(rc);

  t0 = rdtscp();
  COBJ_EBBCALL(toid, nullCall);
  t1 = rdtscp();
  
  lrt_printf("\t first EBB call %ld\n", (long int)(t1 - t0));

  for(i=0;i<max_iteration;i++) {
    uint64_t ctot, cavg;
    t0 = rdtscp();
    for(j=0;j<max_count;j++) {
      COBJ_EBBCALL(toid, nullCall);
    }
    t1 = rdtscp();
    ctot = t1-t0;
    cavg = ctot/max_count;
    if (i == 0) {
      max_avg = min_avg = cavg;
      total = ctot;
    } else {
      if (max_avg < cavg) max_avg = cavg;
      if (min_avg > cavg) min_avg = cavg;
      total += ctot;
    }
  }
  uint64_t tot_count = max_iteration * max_count;
  lrt_printf("\t"
	     " tot_count %ld "
	     " total %ld \n"
	     "\t"
	     " avg %ld "
	     " max_avg %ld "
	     " min_avg %ld \n",
	     (long int)tot_count, (long int)total, 
	     (long int)(total/tot_count), (long int)max_avg, 
	     (long int)min_avg );
  
  return EBBRC_OK;
}

EBBRC
testTimerOverhead()
{
  int i, j;
  static uint64_t t0, t1;
  static uint64_t t3 __attribute__((unused));
  static uint64_t max_avg, min_avg, total;
  
  for(i=0;i<max_iteration;i++) {
    uint64_t ctot, cavg;
    t0 = rdtscp();
    for(j=0;j<max_count;j++) {
      t3 = rdtscp();
    }
    t1 = rdtscp();
    ctot = t1-t0;
    cavg = ctot/max_count;
    if (i == 0) {
      max_avg = min_avg = cavg;
      total = ctot;
    } else {
      if (max_avg < cavg) max_avg = cavg;
      if (min_avg > cavg) min_avg = cavg;
      total += ctot;
    }
  }
  uint64_t tot_count = max_iteration * max_count;
  lrt_printf("\t"
	     " tot_count %ld "
	     " total %ld \n"
	     "\t"
	     " avg %ld "
	     " max_avg %ld "
	     " min_avg %ld \n",
	     (long int)tot_count, (long int)total, 
	     (long int)(total/tot_count), (long int)max_avg, 
	     (long int)min_avg );
  return EBBRC_OK;
}

/*
 * tests executed in context of event
 * initiated by previous tests
 */
static EBBRC 
runPingTest(EventLoc core)
{
  EBBRC rc;
  if (core >= numcores) {
    runNextTest();
    return EBBRC_OK;
  }
  event_loop_type = PING;
  ping_r = core;
  rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev, EVENT_LOC_SINGLE, 0);
  LRT_RCAssert(rc);
  return EBBRC_OK;
}

EBBRC
runNextTest()
{
  static int nextStage = 0;
#ifdef LRT_EVENT_COLLECT_INT_TIMING
  if (nextStage < 42) {
    nextStage = 42;
  }
#endif

  int curStage;
  EBBRC rc;
  count = -1;
  iteration = -1;
  next = 0;
  resetCounters();
  curStage = nextStage;
  nextStage++;
  switch(curStage) {
  case 0: 
    // tests that run to completion
    lrt_printf("eventtiming: base overheads\n");
    lrt_printf("    EBB CALL OVERHEAD:\n");
    testEBBCall();
    lrt_printf("    TIMER OVERHEAD:\n");
    testTimerOverhead();
    // falls into tests that create events
    // and call runNextTest
    nextStage++;
  case 1: 
    event_loop_type = LOCAL;
    // lrt_event_use_bitvector_local=0;
    lrt_printf("eventtiming: running local event loop with BV disabled\n");
    rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev, EVENT_LOC_SINGLE, 0);
    LRT_RCAssert(rc);
    break;
  case 2:
    event_loop_type = LOCAL;
    //lrt_event_use_bitvector_local=1;
    lrt_printf("eventtiming: running local event loop with BV enabled\n");
    rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev, EVENT_LOC_SINGLE, 0);
    LRT_RCAssert(rc);
    break;
  case 3:
    lrt_printf("--------------- ping tests --------------\n");
    verbose = 0;
    runPingTest(curStage-2);
    break;
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
  case 16:
  case 17:
  case 18:
  case 19:
  case 20:
  case 21:
  case 22:
  case 23:
  case 24:
  case 25:
  case 26:
  case 27:
  case 28:
  case 29:
  case 30:
  case 31:
  case 32:
  case 33:
  case 34:
  case 35:
  case 36:
  case 37:
  case 38:
  case 39:
    runPingTest(curStage-2);
    break;

  case 40:
    verbose = 1;		/* get loud again */
    event_loop_type = RR_ALL;
    //lrt_event_use_bitvector_local=0;
    //lrt_event_use_bitvector_remote=0;
    lrt_printf("eventtiming: running remote event loop with BV disabled\n");
    rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev, EVENT_LOC_SINGLE, 0);
    LRT_RCAssert(rc);
    break;
  case 41:
    event_loop_type = RR_ALL;
    //lrt_event_use_bitvector_local=1;
    //lrt_event_use_bitvector_remote=1;
    lrt_printf("eventtiming: running remote event loop with BV enabled\n");
    rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev, EVENT_LOC_SINGLE, 0);
    LRT_RCAssert(rc);
    break;
  case 42:
#ifdef LRT_EVENT_COLLECT_INT_TIMING
    event_loop_type = LOCAL_INT;
    rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev, EVENT_LOC_SINGLE, 0);
    LRT_RCAssert(rc);
    break;
#endif
  default:
    lrt_printf("exiting with test number %d\n", curStage);
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
  numcores = NumEventLoc();
  verbose = 1;

  lrt_printf("eventtiming: started\n");
  while ((rdtscp() - time) < 1000000) ;
  EBBRC rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &ev);
  LRT_RCAssert(rc);

  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, ev, (EBBId)theAppId,
		    COBJ_FUNCNUM_FROM_TYPE(CObjInterface(EventTiming),
					   loopEvent));
  LRT_RCAssert(rc);

  runNextTest();

  return EBBRC_OK;
}


CObjInterface(EventTiming) EventTiming_ftable = {
  .App_if = {
    .start = EventTiming_start
  },
  .loopEvent = EventTiming_loopEvent,
};

APP(EventTiming, APP_START_ONE);
