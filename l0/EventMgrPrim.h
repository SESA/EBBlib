#ifndef __EVENTMGR_H__
#define __EVENTMGR_H__
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

/*
 * The Event Manager (EM) is one of the primordial objects.  The EM
 * will be a fully distributed objects.  Clients can register with the
 * EM objects to handle specific events.  That will cause a customized
 * routine to be generated on each core, that will register itself
 * with the PIC (programable interrupt controller) on that core.
 * There is a one-to-one correspondence between event numbers and
 * interrupts.  This will allow a dispatch from an interrupt to an EBB
 * to be highly efficient, and conversly all interrupts to be handled
 * on EBBs.  The interrupt routine will buy the EM's rep's stack, and
 * then invoke the handler previously registered for that event with
 * interrupts disabled.
 *
 * The current implementation is very simple, a shared table of
 * handlers, no dynamically generated code yet. ... but wanted to
 * document the plan.
 */

#include <l0/cobj/cobj.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/lrt/event_irq.h>
#include <l0/lrt/event_loc.h>
#include <l0/lrt/event_num.h>

/*
 * All event handling ebbs must conform to these types
 * Any existing ebbs that want to handle events must be
 * frontended by an event handler EBB

 * FIXME: get rid of this type, now any function in EBB can
 *  be used to dispatch an event
 */
COBJ_EBBType(EventHandler) {
  EBBRC (*handleEvent) (EventHandlerRef _self);
  EBBRC (*init) (EventHandlerRef _self, uintptr_t extra);
};

#define EVENTFUNC(funcName) EBBRC (* funcName)(void * _self);
typedef EVENTFUNC(GenericEventFunc);

enum EventLocDesc {
  EVENT_LOC_NONE = LRT_EVENT_LOC_NONE, /* disable this IRQ */
  EVENT_LOC_SINGLE = LRT_EVENT_LOC_SINGLE,
  EVENT_LOC_ALL = LRT_EVENT_LOC_ALL
};

typedef lrt_event_loc EventLoc;
inline static EventLoc MyEventLoc() {return lrt_my_event_loc();}
inline static EventLoc NextEventLoc(EventLoc l) {return lrt_next_event_loc(l);}
inline static EventLoc NumEventLoc() {return lrt_num_event_loc();}

typedef lrt_event_num EventNo;

/*
 * key local to eventmgr for allocating specific reserved events/interrupt
 * sources
 */
COBJ_EBBType(EventMgrPrim) {
  EBBRC (*allocEventNo) (EventMgrPrimRef _self, EventNo *eventNoPtr);

  EBBRC (*freeEventNo) (EventMgrPrimRef _self, EventNo eventNo);

  EBBRC (*bindEvent) (EventMgrPrimRef _self, EventNo eventNo,
                      EBBId handler, EBBFuncNum fn);

  EBBRC (*routeIRQ) (EventMgrPrimRef _self, IRQ *isrc, EventNo eventNo,
                     enum EventLocDesc desc, EventLoc el);

  EBBRC (*triggerEvent) (EventMgrPrimRef _self, EventNo eventNo,
                         enum EventLocDesc desc, EventLoc el);
};

// the ID of the one and only event manager
extern EventMgrPrimId theEventMgrPrimId;

#endif
