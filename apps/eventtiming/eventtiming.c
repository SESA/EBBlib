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

uint64_t t0, t1;
int count;
EventNo ev;

static EBBRC
EventTiming_roundRobinEvent(EventTimingRef self)
{
  if (count == 32 * 1000) {
    t1 = rdtscp();
    lrt_printf("eventtiming: round robin total %ld per lp %ld\n",
	       t1 - t0, (t1 - t0)/(32*1000));
    lrt_exit(0);
  } else if (count == 0) {
    t0 = rdtscp();
  }
  count++;

  COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev,
               EVENT_LOC_SINGLE, NextEventLoc(MyEventLoc()));

  return EBBRC_OK;
}
static EBBRC
EventTiming_loopLocalEvent(EventTimingRef self)
{
  if (count == 1000) {
    t1 = rdtscp();
    lrt_printf("eventtiming: local total %ld per lp %ld\n",
	       t1 - t0, (t1 - t0)/(1000));
    count = -1;
    EBBRC rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, ev, (EBBId)theAppId,
                            COBJ_FUNCNUM_FROM_TYPE(CObjInterface(EventTiming),
                                                   roundRobinEvent));
    LRT_RCAssert(rc);
  } else if (count == 0) {
    t0 = rdtscp();
  }
  count++;

  COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev,
               EVENT_LOC_SINGLE, MyEventLoc());

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

  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, ev, (EBBId)theAppId,
                    COBJ_FUNCNUM_FROM_TYPE(CObjInterface(EventTiming), loopLocalEvent));
  LRT_RCAssert(rc);

  rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev,
                    EVENT_LOC_SINGLE, MyEventLoc());

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
