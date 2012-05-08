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
#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <lrt/io.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <lrt/assert.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/MsgMgr.h>
#include <l1/MsgMgrPrim.h>

// globally known id of the message mgr
MsgMgrId theMsgMgrId = 0;


/* -- start routines & types to be implemented, put somewhere global*/
typedef long LockType;
 
static void
spinLock(LockType *lk)
{
  uintptr_t rc = 0;
  while (!rc) {
    rc = __sync_bool_compare_and_swap(lk, 0, 1);
  }
}

static void
spinUnlock(LockType *lk)
{
  __sync_bool_compare_and_swap(lk, 1, 0);
}

enum{MAXARGS = 3};
/* -- end routines to be implemented */

typedef struct MsgStore_struc {
  struct MsgStore_struc *next;
  EvntLoc home;			/* node this was allocated on */
  MsgHandlerId id;
  uintptr_t numargs;
  uintptr_t args[MAXARGS];
} MsgStore;


CObject(EvHdlr) {
  COBJ_EBBFuncTbl(EventHandler);
}; 

CObject(MsgMgrPrim) {
  COBJ_EBBFuncTbl(MsgMgr);

  CObjectDefine(EvHdlr) evHdlr;

  EvntLoc eventLoc;
  LockType msgqueuelock;
  MsgStore *msgqueue; 
  LockType freelistlock;
  MsgStore *freelist; 
  // FIXME: abstract at event mgr
  MsgMgrPrimRef reps[LRT_PIC_MAX_PICS];
  // reference to the single root
  CObjEBBRootMultiRef theRootMM;
};

static EBBRC
MsgMgrPrim_enqueueMsg(MsgMgrPrimRef target, MsgStore *msg)
{
  uintptr_t queueempty = 1;
  spinLock(&target->msgqueuelock);
  if (target->msgqueue != NULL) {
    queueempty = 0;
  }
  msg->next = target->msgqueue;
  target->msgqueue = msg;
  spinUnlock(&target->msgqueuelock);
  if (queueempty) {
    COBJ_EBBCALL(theEventMgrPrimId, dispatchIPI, target->eventLoc);
  }
  return EBBRC_OK;
}

static MsgStore *
MsgMgrPrim_dequeueMsgHead(MsgMgrPrimRef target)
{
  MsgStore *msg;
  spinLock(&target->msgqueuelock);
  msg = target->msgqueue;
  if (msg != NULL) {
    target->msgqueue = msg->next;
  }
  spinUnlock(&target->msgqueuelock);
  return msg;
}

static EBBRC
MsgMgrPrim_findTarget(MsgMgrPrimRef self, EvntLoc loc, MsgMgrPrimRef *target)
{
  RepListNode *node;
  EBBRep * rep = NULL;
  rep = (EBBRep *)self->reps[loc];
  if (rep == NULL) {
    for (node = self->theRootMM->ft->nextRep(self->theRootMM, 0, 
					     &rep);
	 node != NULL; 
	 node = self->theRootMM->ft->nextRep(self->theRootMM, node, 
					   &rep)) {
      LRT_Assert(rep != NULL);
      if (((MsgMgrPrimRef)rep)->eventLoc == loc) break;
    }
    self->reps[loc] = (MsgMgrPrimRef)rep;
  }
  // FIXME: handle case that rep doesn't yet exist
  *target = (MsgMgrPrimRef)rep;
  if (rep == NULL) {
    return EBBRC_NOTFOUND;
  }
  return EBBRC_OK;
}

static MsgStore *
allocMsg(MsgMgrPrimRef self)
{
  EBBRC rc;
  MsgStore *msg;  
  spinLock(&self->freelistlock);
  msg = self->freelist;
  if (msg != NULL) {
    self->freelist = msg->next; 
    spinUnlock(&self->freelistlock);
    msg->home = MyEL();
    // lrt_printf("%s:%s found free message\n", __FILE__, __func__);
    return msg ;
  }
  spinUnlock(&self->freelistlock);

  // lrt_printf("%s:%s freelist empty, allocating new msg\n", __FILE__, __func__);

  // need to allocate another message 
  rc = EBBPrimMalloc(sizeof(*msg), &msg, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  msg->home = MyEL();

  return msg;
}

static void
freeMsg(MsgMgrPrimRef self, MsgStore *msg)
{
  EBBRC rc;
  MsgMgrPrimRef target; 

  rc = MsgMgrPrim_findTarget(self, msg->home, &target);
  LRT_RCAssert(rc);

  spinLock(&target->freelistlock);
  msg->next = target->freelist;
  target->freelist = msg;
  spinUnlock(&target->freelistlock);
}

static EBBRC 
MsgMgrPrim_msg0(MsgMgrRef _self, EvntLoc loc, MsgHandlerId id)
{
  MsgMgrPrimRef self = (MsgMgrPrimRef)_self;
  MsgStore *msg;  
  MsgMgrPrimRef target;
  EBBRC rc;

  rc = MsgMgrPrim_findTarget(self, loc, &target);
  if (rc == EBBRC_NOTFOUND) return rc;

  LRT_RCAssert(rc);

  msg = allocMsg(self);
  msg->id = id;
  msg->numargs = 0;
  
  MsgMgrPrim_enqueueMsg(target, msg);
  return EBBRC_OK;
}

static EBBRC 
MsgMgrPrim_msg1(MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, uintptr_t a1)
{
  MsgMgrPrimRef self = (MsgMgrPrimRef)_self;
  MsgStore *msg;  
  MsgMgrPrimRef target;
  EBBRC rc;

  rc = MsgMgrPrim_findTarget(self, loc, &target);
  if (rc == EBBRC_NOTFOUND) return rc;

  LRT_RCAssert(rc);

  msg = allocMsg(self);
  msg->id = id;
  msg->numargs = 1;
  msg->args[0] = a1;
  // target->enqueueMsg(msg);
  MsgMgrPrim_enqueueMsg(target, msg);
  return EBBRC_OK;
}

static EBBRC 
MsgMgrPrim_msg2(MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, uintptr_t a1, 
		uintptr_t a2)
{
  MsgMgrPrimRef self = (MsgMgrPrimRef)_self;
  MsgStore *msg;  
  MsgMgrPrimRef target;
  EBBRC rc;

  rc = MsgMgrPrim_findTarget(self, loc, &target);
  if (rc == EBBRC_NOTFOUND) return rc;

  LRT_RCAssert(rc);

  msg = allocMsg(self);
  msg->id = id;
  msg->numargs = 2;
  msg->args[0] = a1;
  msg->args[1] = a2;
  MsgMgrPrim_enqueueMsg(target, msg);
  return EBBRC_OK;
}

static EBBRC 
MsgMgrPrim_msg3(MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, 
		uintptr_t a1, uintptr_t a2, uintptr_t a3)
{
  MsgMgrPrimRef self = (MsgMgrPrimRef)_self;
  MsgStore *msg;  
  MsgMgrPrimRef target;
  EBBRC rc;

  rc = MsgMgrPrim_findTarget(self, loc, &target);
  if (rc == EBBRC_NOTFOUND) return rc;

  LRT_RCAssert(rc);

  msg = allocMsg(self);
  msg->id = id;
  msg->numargs = 3;
  msg->args[0] = a1;
  msg->args[1] = a2;
  msg->args[2] = a3;
  MsgMgrPrim_enqueueMsg(target, msg);
  return EBBRC_OK;
};

static EBBRC 
MsgMgrPrim_handleEvent(EventHandlerRef _self)
{
  MsgMgrPrimRef self = (MsgMgrPrimRef)ContainingCOPtr(_self,MsgMgrPrim,evHdlr);
  MsgStore *msg;
  EBBRC rc;
  rc = COBJ_EBBCALL(theEventMgrPrimId, ackIPI);
  LRT_RCAssert(rc);

  msg = MsgMgrPrim_dequeueMsgHead(self);
  while (msg != NULL) {
    switch(msg->numargs) {
    case 0:
      COBJ_EBBCALL(msg->id, msg0);
      break;
    case 1:
      COBJ_EBBCALL(msg->id, msg1, msg->args[0]);
      break;
    case 2:
      COBJ_EBBCALL(msg->id, msg2, msg->args[0], msg->args[1]);
      break;
    case 3:
      COBJ_EBBCALL(msg->id, msg3, msg->args[0], msg->args[1], msg->args[2]);
      break;
    }
    // FIXME: retain in a free list?
    freeMsg(self, msg);
    msg = MsgMgrPrim_dequeueMsgHead(self);
  }
  // we are re-enabling interrupts before returning to event mgr
  // not obvious yet if we should be doing this here, or lower down, 
  // but its at least reasonable that we want to execute an entire message
  // disabled. Note, a whole chain of messages may be invoked here, so, 
  // the implicit assumption is that  you re-disable interrupts if you enable
  // them at the end of a message. 
  rc = COBJ_EBBCALL(theEventMgrPrimId, enableIPI);
  LRT_RCAssert(rc);
  return EBBRC_OK;
}

//MsgMgr part of the interface 
CObjInterface(MsgMgr) MsgMgrPrim_ftable = {
  .msg0 = MsgMgrPrim_msg0,
  .msg1 = MsgMgrPrim_msg1,
  .msg2 = MsgMgrPrim_msg2,
  .msg3 = MsgMgrPrim_msg3,
  {// the implementation of the event handler functions
    .handleEvent = MsgMgrPrim_handleEvent
  }
};

static inline void
MsgMgrPrim_SetFT(MsgMgrPrimRef o)
{
  o->ft = &MsgMgrPrim_ftable; 
  o->evHdlr.ft = &(MsgMgrPrim_ftable.EventHandler_if);
};


static EBBRep *
MsgMgrPrim_createRepAssert(CObjEBBRootMultiRef root)
{
  LRT_Assert(0);
  return NULL;
}

static EBBRC
MsgMgrPrim_createRep(CObjEBBRootMultiImpRef rootRefMM, 
		     CObjEBBRootMultiImpRef rootRefEH, 
		     EventHandlerId ehid)
{
  MsgMgrPrimRef repRef;
  LRT_Assert(ehid != NULL);
  

  EBBPrimMalloc(sizeof(MsgMgrPrim), &repRef, EBB_MEM_DEFAULT);
  MsgMgrPrim_SetFT(repRef);
  int i;

  repRef->eventLoc = MyEL();
  repRef->msgqueuelock = 0;
  repRef->msgqueue = 0;
  repRef->freelistlock = 0;
  repRef->freelist = 0;
  for (i=0; i<LRT_PIC_MAX_PICS ; i++ ) {
    repRef->reps[i] = NULL;
  }
  repRef->theRootMM = (CObjEBBRootMultiRef)rootRefMM;

  // register this rep to take over the IPI on this core, note, this
  // needs to be before we tell the root about ourselves, since otherwise 
  // have a race condition where the rep can say there is a message handler on
  // this core, but we are not yet up. 
  COBJ_EBBCALL(theEventMgrPrimId, registerIPIHandler, ehid, NOFUNCNUM);

  // now tell the root for event handler its pseudo representative on this core
  // note, EH must by set up before MM
  rootRefMM->ft->addRepOn((CObjEBBRootMultiRef)rootRefEH, MyEL(),
			  (EBBRep *)(void *)&(repRef->evHdlr));

  // tell the root for MsgMgr its rep on this core
  rootRefMM->ft->addRepOn((CObjEBBRootMultiRef)rootRefMM, MyEL(),
			  (EBBRep *)repRef);

  return EBBRC_OK;
};

static CObjEBBRootMultiImpRef rootRefMM = 0, rootRefEH = 0;

EBBRC
MsgMgrPrim_Init(void)
{
  static EventHandlerId theMsgMgrEHId = 0;

  if (__sync_bool_compare_and_swap(&theMsgMgrEHId, (MsgMgrId)0,
				   (MsgMgrId)-1)) {
    EBBRC rc;
    EBBId id;

    // create root for MsgMgr
    rc = CObjEBBRootMultiImpCreate(&rootRefMM, MsgMgrPrim_createRepAssert);
    LRT_RCAssert(rc);
    rc = EBBAllocPrimId(&id);
    LRT_RCAssert(rc); 
    rc = EBBBindPrimId(id, CObjEBBMissFunc, (EBBMissArg)rootRefMM);
    LRT_RCAssert(rc); 
    theMsgMgrId = (MsgMgrId)id;

    // create root for EventHandler part of MsgMgr
    rc = CObjEBBRootMultiImpCreate(&rootRefEH, MsgMgrPrim_createRepAssert);
    LRT_RCAssert(rc);
    rc = EBBAllocPrimId(&id);
    LRT_RCAssert(rc); 
    rc = EBBBindPrimId(id, CObjEBBMissFunc, (EBBMissArg)rootRefEH);
    LRT_RCAssert(rc); 
    theMsgMgrEHId = (EventHandlerId)id;
    LRT_Assert(id != NULL);
  } else {
    while (((volatile uintptr_t)theMsgMgrEHId)==-1);
  }

  // initialize the msgmgr rep on this core, since we need to take
  // over the event locally for IPI even before anyone sends a message from
  // this event location
  LRT_Assert(theMsgMgrEHId != NULL);
  MsgMgrPrim_createRep(rootRefMM, rootRefEH, theMsgMgrEHId);
  return EBBRC_OK;
}

