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
 * All event handling ebbs must conform to these types
 * Any existing ebbs that want to handle events must be
 * frontended by an event handler EBB
 */
CObjInterface(EventHandler) {
  EBBRC (*handleEvent) (void *_self);
  EBBRC (*init) (void *_self);
};

CObject(EventHandler) {
  CObjInterface(EventHandler) *ft;
};

typedef EventHandlerRef *EventHandlerId;

CObjInterface(EventMgrPrim) {
  // get handler, possibly on this core, needed to dispatch interupt
  EBBRC (*getHandler) (void *_self, uintptr_t eventNo, EventHandlerId *handler); 

  EBBRC (*registerHandler) (void *_self, uintptr_t eventNo,
			    EventHandlerId handler, 
			    uintptr_t isrc);
  EBBRC (*allocEventNo) (void *_self, uintptr_t *eventNoPtr);
};

CObject(EventMgrPrim) {
  CObjInterface(EventMgrPrim) *ft;
};

typedef EventMgrPrimRef *EventMgrPrimId;
// the ID of the one and only event manager
extern EventMgrPrimId theEventMgrPrimId;

/*
 * You need to be able to get the event location of the 
 * node you are running on.  I am not making this a 
 * function on EventMgr, but rather a global function
 * so its clear that its the EL of the current core and not
 * the EL of the rep of the EventMgr
 */
typedef uintptr_t EvntLoc;
extern EvntLoc  EventMgrPrim_GetMyEL();

#endif
