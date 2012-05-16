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
#include <l0/lrt/event_irq_def.h>
#include <lrt/exit.h>

// move exit handler into L1 service
// --------------------------------------------------------------------------
#include <l0/EBBMgrPrim.h>
#include <l0/cobj/CObjEBBRootShared.h>

COBJ_EBBType(ExitHandler)
{
  EBBRC (*triggerExit) (ExitHandlerRef _self, int status);
  EVENTFUNC(handleEvent);      /* actual event that signals an exit */
};

CObject(ExitHandlerImp) {
  CObjInterface(ExitHandler) *ft;
  int exitStatus;
};


// globally known id of the exit handler
ExitHandlerId theExitHandlerId = 0;

// internally used event for signalling exit
static EventNo theExitHandlerEvent = 0;

static EBBRC
ExitHandlerImp_handleEvent(void *_self)
{
  ExitHandlerImpRef self = (ExitHandlerImpRef)_self;
  lrt_exit(self->exitStatus);
  return EBBRC_OK;
}

static EBBRC
ExitHandlerImp_triggerExit(ExitHandlerRef _self, int status)
{
  EBBRC rc;
  ExitHandlerImpRef self = (ExitHandlerImpRef)_self;
  self->exitStatus = status;
  rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, theExitHandlerEvent,
               EVENT_LOC_SINGLE, MyEventLoc());
  LRT_RCAssert(rc);
  return EBBRC_OK;
}

CObjInterface(ExitHandler) ExitHandlerImp_ftable = {
  .handleEvent = ExitHandlerImp_handleEvent,
  .triggerExit = ExitHandlerImp_triggerExit
};


void
ExitHandlerImp_SetFT(ExitHandlerImpRef o)
{
  o->ft = &ExitHandlerImp_ftable;
}

static EBBRep *
ExitHandler_createRep(void)
{
  ExitHandlerImpRef repRef;
  EBBRC rc;

  rc = EBBPrimMalloc(sizeof(ExitHandlerImp), &repRef, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  ExitHandlerImp_SetFT(repRef);
  repRef->exitStatus = 0;

  return (EBBRep *)repRef;
};

EBBRC
ExitHandlerImp_Init(void)
{
  CObjEBBRootSharedRef rootRef;
  // there could be a race, with this called on multiple cores
  // simultaneously
  if (__sync_bool_compare_and_swap(&theExitHandlerId, (ExitHandlerId)0,
                                   (ExitHandlerId)-1)) {
    EBBRC rc;
    EBBId id;

    // create single rep aggressively, since we want to minize
    // functinality on the call path to exit
    EBBRep *rep = ExitHandler_createRep();

    // create root for ExitHandler
    rc = CObjEBBRootSharedCreate(&rootRef, rep);
    LRT_RCAssert(rc);
    rc = EBBAllocPrimId(&id);
    LRT_RCAssert(rc);
    rc = EBBBindPrimId(id, CObjEBBMissFunc, (EBBMissArg)rootRef);
    LRT_RCAssert(rc);

    LRT_Assert(id != NULL);

    // allocate event number and bind to handleEvent function
    rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &theExitHandlerEvent);
    LRT_RCAssert(rc);
    rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, theExitHandlerEvent, id,
                      COBJ_FUNCNUM_FROM_TYPE(CObjInterface(ExitHandler),
                                             handleEvent));
    LRT_RCAssert(rc);


    theExitHandlerId = (ExitHandlerId)id;
  } else {
    while ((*(volatile uintptr_t *)&theExitHandlerId)==-1);
  }
  return EBBRC_OK;
}

// -------------------------------------------------------------------------


CObject(EventTst) {
  CObjInterface(EventTst) *ft;
};

CObjInterface(EventTst) {
  CObjImplements(App);
  EBBRC (*triggerLocalTestEvent) (EventTstRef _self);
  EBBRC (*triggerRemoteTestEvent) (EventTstRef _self);
  EBBRC (*irqLocalTestEvent) (EventTstRef _self);
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


static void
test_bind(EventTstRef self)
{
  EBBRC rc;
  EventNo ev;
  lrt_printf("EventTst: bindtest started\n");
  rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &ev);
  LRT_RCAssert(rc);

  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, ev, (EBBId)theAppId,
                    COBJ_FUNCNUM(self, triggerLocalTestEvent));
  LRT_RCAssert(rc);
  rc = COBJ_EBBCALL(theEventMgrPrimId, freeEventNo, ev);
  LRT_RCAssert(rc);

  lrt_printf("EventTst: bindtest succeeded\n");
}

static EventNo localEV;

static void
test_triggerlocal(EventTstRef self)
{
  EBBRC rc;
  lrt_printf("EventTst: triggerlocaltest started\n");
  rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &localEV);
  LRT_RCAssert(rc);
  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, localEV, (EBBId)theAppId,
                    COBJ_FUNCNUM(self, triggerLocalTestEvent));
  LRT_RCAssert(rc);
  rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, localEV,
                    EVENT_LOC_SINGLE, MyEventLoc());
  LRT_RCAssert(rc);
}

static EventNo remoteEV;

static void
test_triggerremote(EventTstRef self)
{
  EBBRC rc;
  lrt_printf("EventTst: triggerremotetest started\n");
  rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &remoteEV);
  LRT_RCAssert(rc);
  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, remoteEV, (EBBId)theAppId,
                    COBJ_FUNCNUM(self, triggerRemoteTestEvent));
  LRT_RCAssert(rc);
  rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, remoteEV,
                    EVENT_LOC_SINGLE, NextEventLoc(MyEventLoc()));
  LRT_RCAssert(rc);
}

//FIXME: make this compliant with other LRTs
static EventNo irqEV;
static int pipes[2];
static struct IRQ_t irq;

static void
test_irqlocal(EventTstRef self)
{
  EBBRC rc;
  lrt_printf("EventTst: irqlocaltest started\n");

  rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &irqEV);
  LRT_RCAssert(rc);
  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, irqEV, (EBBId)theAppId,
                    COBJ_FUNCNUM(self, irqLocalTestEvent));
  LRT_RCAssert(rc);

  int ret = pipe(pipes);
  LRT_Assert(ret == 0);
  //FIXME: error check
  irq.flags = LRT_EVENT_IRQ_READ;
  irq.fd = pipes[0];

  rc = COBJ_EBBCALL(theEventMgrPrimId, routeIRQ, &irq, irqEV,
                    EVENT_LOC_SINGLE, MyEventLoc());
  LRT_RCAssert(rc);

  //Now trigger the event
  char c = '.';
  ssize_t ret2 = write(pipes[1], &c, sizeof(c));
  LRT_Assert(ret2 == sizeof(c));
}

static EBBRC
EventTst_start(AppRef _self)
{
  EventTstRef self = (EventTstRef)_self;
  EBBRC rc;

  lrt_printf("EventTst, core %d number of cores %d\n", MyEventLoc(),
             NumEventLoc());

  test_allocate();
  test_bind(self);
  test_triggerlocal(self);

  ExitHandlerImp_Init();
  rc = COBJ_EBBCALL(theExitHandlerId, triggerExit, 0);
  LRT_RCAssert(rc);

  return EBBRC_OK;
}

static EBBRC
EventTst_triggerLocalTestEvent(EventTstRef _self)
{
  EBBRC rc;
  rc = COBJ_EBBCALL(theEventMgrPrimId, freeEventNo, localEV);
  LRT_RCAssert(rc);
  lrt_printf("EventTst: triggerlocaltest succeeded\n");
  test_triggerremote(_self);
  return EBBRC_OK;
}

static EBBRC
EventTst_triggerRemoteTestEvent(EventTstRef _self)
{
  EBBRC rc;

  rc = COBJ_EBBCALL(theEventMgrPrimId, freeEventNo, remoteEV);
  LRT_RCAssert(rc);
  lrt_printf("EventTst: triggerremotetest succeeded on core %d\n",
             MyEventLoc());
  test_irqlocal(_self);
  return EBBRC_OK;
}

static EBBRC
EventTst_irqLocalTestEvent(EventTstRef _self)
{
  char c;
  ssize_t rc = read(pipes[0], &c, sizeof(c));
  LRT_Assert(rc == sizeof(c));
  lrt_printf("EventTst: irqlocaltest succeeded\n");
  return EBBRC_OK;
}

CObjInterface(EventTst) EventTst_ftable = {
  .App_if = {
    .start = EventTst_start
  },
  .triggerLocalTestEvent = EventTst_triggerLocalTestEvent,
  .triggerRemoteTestEvent = EventTst_triggerRemoteTestEvent,
  .irqLocalTestEvent = EventTst_irqLocalTestEvent
};

APP(EventTst, APP_START_ONE);
