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

// the root of both message manager
static CObjEBBRootMultiImpRef rootRefMM = 0;

// the event reserved for the message manager
static EventNo theMsgMgrEvent = 0;


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
  EventLoc home;			/* node this was allocated on */
  MsgHandlerId id;
  uintptr_t numargs;
  uintptr_t args[MAXARGS];
} MsgStore;

CObject(MsgMgrPrim) {
  CObjInterface(MsgMgrPrim) *ft;
  EventLoc eventLoc;
  LockType msgqueuelock;
  MsgStore *msgqueue; 
  LockType freelistlock;
  MsgStore *freelist; 
  // FIXME: abstract at event mgr
  MsgMgrPrimRef reps[LRT_MAX_EL];
  // reference to the single root
  CObjEBBRootMultiRef theRootMM;
};

CObjInterface(MsgMgrPrim) {
  CObjImplements(MsgMgr);
  EBBRC (*handleEvent) (MsgMgrPrimRef _self);
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
    COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, theMsgMgrEvent, target->eventLoc);
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
MsgMgrPrim_findTarget(MsgMgrPrimRef self, EventLoc loc, MsgMgrPrimRef *target)
{
  RepListNode *node;
  EBBRep * rep = NULL;
  int sent_event = 0;

  while (1) {
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
    if (rep != NULL) {
      return EBBRC_OK; 
    }
    if (!sent_event) {
      lrt_printf("MsgMgr: no rep on loc %d\n, kicking the bugger\n", loc);
      COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, theMsgMgrEvent, loc);
      sent_event=1; 
    } else {
      lrt_printf("x");      
    }
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
    msg->home = MyEventLoc();
    // lrt_printf("%s:%s found free message\n", __FILE__, __func__);
    return msg ;
  }
  spinUnlock(&self->freelistlock);

  // lrt_printf("%s:%s freelist empty, allocating new msg\n", __FILE__, __func__);

  // need to allocate another message 
  rc = EBBPrimMalloc(sizeof(*msg), &msg, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  msg->home = MyEventLoc();

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
MsgMgrPrim_msg0(MsgMgrRef _self, EventLoc loc, MsgHandlerId id)
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
MsgMgrPrim_msg1(MsgMgrRef _self, EventLoc loc, MsgHandlerId id, uintptr_t a1)
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
MsgMgrPrim_msg2(MsgMgrRef _self, EventLoc loc, MsgHandlerId id, uintptr_t a1, 
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
MsgMgrPrim_msg3(MsgMgrRef _self, EventLoc loc, MsgHandlerId id, 
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
MsgMgrPrim_handleEvent(MsgMgrPrimRef self)
{
  MsgStore *msg;

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
  return EBBRC_OK;
}

//MsgMgr part of the interface 
CObjInterface(MsgMgrPrim) MsgMgrPrim_ftable = {
  .MsgMgr_if = {
    .msg0 = MsgMgrPrim_msg0,
    .msg1 = MsgMgrPrim_msg1,
    .msg2 = MsgMgrPrim_msg2,
    .msg3 = MsgMgrPrim_msg3
  },
  .handleEvent = MsgMgrPrim_handleEvent
};

static inline void
MsgMgrPrim_SetFT(MsgMgrPrimRef o)
{
  o->ft = &MsgMgrPrim_ftable; 
};


static EBBRep *
MsgMgrPrim_createRep(CObjEBBRootMultiRef rootRefMM)
{
  MsgMgrPrimRef repRef;

  EBBPrimMalloc(sizeof(MsgMgrPrim), &repRef, EBB_MEM_DEFAULT);
  MsgMgrPrim_SetFT(repRef);
  int i;

  repRef->eventLoc = MyEventLoc();
  repRef->msgqueuelock = 0;
  repRef->msgqueue = 0;
  repRef->freelistlock = 0;
  repRef->freelist = 0;
  for (i=0; i<LRT_MAX_EL ; i++ ) {
    repRef->reps[i] = NULL;
  }
  repRef->theRootMM = rootRefMM;

  return (EBBRep *)repRef;
};

EBBRC
MsgMgrPrim_Init(void)
{
  if (__sync_bool_compare_and_swap(&theMsgMgrId, (MsgMgrId)0,
				   (MsgMgrId)-1)) {
    EBBRC rc;
    EBBId id;

    // create root for MsgMgr
    rc = CObjEBBRootMultiImpCreate(&rootRefMM, MsgMgrPrim_createRep);
    LRT_RCAssert(rc);
    rc = EBBAllocPrimId(&id);
    LRT_RCAssert(rc); 
    rc = EBBBindPrimId(id, CObjEBBMissFunc, (EBBMissArg)rootRefMM);
    LRT_RCAssert(rc); 

    // allocate the event and bind it before publishing the msgmgr id, since 
    // publishing the MsgMgrEHId will unblock everyone
    rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &theMsgMgrEvent);
    LRT_RCAssert(rc); 

    LRT_Assert(id != NULL);
    rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, theMsgMgrEvent, id, 
		      __builtin_offsetof(struct MsgMgrPrim_if, handleEvent)/sizeof(COBJFunc));
    LRT_RCAssert(rc); 

    theMsgMgrId = (MsgMgrId)id;
  } else {
    while (((volatile uintptr_t)theMsgMgrId)==-1);
  }
  return EBBRC_OK;
}

