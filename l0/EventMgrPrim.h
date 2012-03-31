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

/* 
 * All event handling ebbs must conform to these types
 * Any existing ebbs that want to handle events must be
 * frontended by an event handler EBB
 */
COBJ_EBBType(EventHandler) {
  EBBRC (*handleEvent) (EventHandlerRef _self);
  EBBRC (*init) (EventHandlerRef _self, uintptr_t extra);
};

/*
 * You need to be able to get the event location of the node you are
 * running on.  I am not making this a function on EventMgr, but
 * rather a global function so its clear that its the EL of the
 * current core and not the EL of the rep of the EventMgr
 * 
 * For now, one-to-one mapping event location and pic location. Since
 * this may change, we include the corresponding header file here.
 */
#include <l0/lrt/pic.h>
typedef uintptr_t EvntLoc;
inline static EvntLoc MyEL() { return lrt_pic_myid; }

typedef EBBRC (*EventFunc)(void *);

/* 
 * key local to eventmgr for allocating specific reserved events/interrupt
 * sources
 */
COBJ_EBBType(EventMgrPrim) {
  EBBRC (*registerHandler) (EventMgrPrimRef _self, uintptr_t eventNo,
			    EventHandlerId handler, FuncNum fn, 
			    lrt_pic_src *isrc); 

  EBBRC (*eventEnable) (EventMgrPrimRef _self, uintptr_t eventNo);

  EBBRC (*eventDisable) (EventMgrPrimRef _self, uintptr_t eventNo);
			     
  EBBRC (*registerIPIHandler) (EventMgrPrimRef _self, EventHandlerId handler);

  EBBRC (*allocEventNo) (EventMgrPrimRef _self, uintptr_t *eventNoPtr);

  EBBRC (*dispatchIPI) (EventMgrPrimRef _self, EvntLoc el);

  // called by eary implementation of the vector function
  EBBRC (*dispatchEventLocal) (EventMgrPrimRef _self, uintptr_t eventNo);
};

// the ID of the one and only event manager
extern EventMgrPrimId theEventMgrPrimId;


#endif
