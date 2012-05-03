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
#include <l1/App.h>
#include <lrt/io.h>
#include <l0/EventMgrPrim.h>

CObject(EventTst) {
  CObjInterface(EventTst) *ft;
};

CObjInterface(EventTst) {
  CObjImplements(App);
  EBBRC (*inEvent) (EventTstRef _self);
};

#define TABSIZE 200

static void
test_allocate()
{
  lrt_printf("EventTst: alloc test started\n");

  struct used {
    EventNo ev;
    int used;
  } tab[TABSIZE];
  EBBRC rc;
 
  for(int i=0; i<100; i++) {
    if (tab[i].used == 0) {
      rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &tab[i].ev);
      LRT_RCAssert(rc);

      tab[i].used = 1;
      lrt_printf("\tEventTst, got event %d\n", tab[i].ev);
    }
  }
  for(int i=0; i<10; i++) {
    if (tab[i].used) {
      rc = COBJ_EBBCALL(theEventMgrPrimId, freeEventNo, tab[i].ev);
      LRT_RCAssert(rc);
      tab[i].used = 0;
    }
  }
  for(int i=0; i<5; i++) {
    if (tab[i].used == 0) {
      rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &tab[i].ev);
      LRT_RCAssert(rc);
      tab[i].used = 1;
      lrt_printf("\tEventTst, got event %d\n", tab[i].ev);
    }
  }
  for(int i=0; i<TABSIZE; i++) {
    if (tab[i].used) {
      rc = COBJ_EBBCALL(theEventMgrPrimId, freeEventNo, tab[i].ev);
      LRT_RCAssert(rc);
      tab[i].used = 0;
    }
  }
    
  lrt_printf("EventTst: alloctest succeeded\n");
}


static EventNo 
test_bind(EventTstRef self)
{
  EBBRC rc;
  EventNo ev;
  lrt_printf("EventTst: bindtest started\n");
  rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &ev);
  LRT_RCAssert(rc);
  
  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, ev, (EBBId)theAppId, 
		    COBJ_FUNCNUM(self, inEvent));

  LRT_RCAssert(rc);
  lrt_printf("EventTst: bindtest succeeded\n");

  return ev;
}

static void
test_trigger(EventNo ev)
{
  EBBRC rc;
  lrt_printf("EventTst: triggertest started\n");
  
  rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, ev, MyEventLoc());
}

static EBBRC 
EventTst_start(AppRef _self)
{
  EventTstRef self = (EventTstRef)_self;
  lrt_printf("EventTst, core %d number of cores %d\n", MyEventLoc(), NumEventLoc());

  test_allocate();
  EventNo ev = test_bind(self);
  test_trigger(ev);

  return EBBRC_OK;
}

static EBBRC
EventTst_inEvent(EventTstRef _self)
{
  lrt_printf("EventTst: triggertest succeeded\n");
  return EBBRC_OK;
}

CObjInterface(EventTst) EventTst_ftable = {
  .App_if = {
    .start = EventTst_start
  },
  .inEvent = EventTst_inEvent
};

APP(EventTst, APP_START_ONE);
