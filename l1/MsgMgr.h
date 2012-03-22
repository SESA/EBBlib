#ifndef __MSG_MGR_H__
#define __MSG_MGR_H__
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
 * All Message handling ebbs must conform to these types
 * Any existing ebbs that want to handle messages must be
 * frontended by an message handler EBB
 */
COBJ_EBBType(MsgHandler) {
  /* routines to handle messages sent to me */
  EBBRC (*msg0) (MsgHandlerRef _self);
  EBBRC (*msg1) (MsgHandlerRef _self, uintptr_t a1);
  EBBRC (*msg2) (MsgHandlerRef _self, uintptr_t a1, uintptr_t a2);
  EBBRC (*msg3) (MsgHandlerRef _self, uintptr_t a1, uintptr_t a2, uintptr_t a3);
};

/*
 * To send a message to another event location (e.g., core), specify 
 * location, and the ID of the handler object at the target side. This 
 * will normally be the mechanism to start computation on another node 
 * and create local reps as the computation faults on them.
 */
COBJ_EBBType(MsgMgr) {
  /* send message to id at event location */
  EBBRC (*msg0) (MsgMgrRef _self, EvntLoc loc, MsgHandlerId id);

  EBBRC (*msg1) (MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, 
		 uintptr_t a1);

  EBBRC (*msg2) (MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, 
		 uintptr_t a1, uintptr_t a2);

  EBBRC (*msg3) (MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, 
		 uintptr_t a1, uintptr_t a2, uintptr_t a3);

  /* this is only called by corresponding event handler */
  EBBRC (*handleIPI) (MsgMgrRef _self);
};


// the ID of the one and only Primative MsgHandler
extern MsgMgrId theMsgMgrId;

#endif
