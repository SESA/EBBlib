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
#include <l0/lrt/pic.h>
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
#include <l0/lrt/pic.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/MsgMgr.h>
#include <l1/MsgMgrPrim.h>


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

int SendIPIEvent(EvntLoc el){return lrt_pic_ipi(el);};
enum{MAXARGS = 3};
/* -- end routines to be implemented */

typedef struct MsgStore_struc {
  struct MsgStore_struc *next;
  uintptr_t numargs;
  uintptr_t args[MAXARGS];
} MsgStore;


CObject(MsgMgrPrim) {
  CObjInterface(MsgMgr) *ft;
  uintptr_t eventLoc;
  LockType mml;
  MsgStore *msgqueue; 
  // FIXME: abstract at event mgr
  MsgMgrPrimRef reps[LRT_PIC_MAX_PICS];
  // reference to the single root
  CObjEBBRootMultiRef theRoot;
};

static EBBRC
MsgMgrPrim_enqueueMsg(MsgMgrPrimRef target, MsgStore *msg)
{
  uintptr_t queueempty = 1;
  spinLock(&target->mml);
  if (target->msgqueue != NULL) {
    queueempty = 0;
  }
  msg->next = target->msgqueue;
  target->msgqueue = msg;
  spinUnlock(&target->mml);
  if (queueempty) {
    SendIPIEvent(target->eventLoc);
  }
  return EBBRC_OK;
}

static EBBRC
MsgMgrPrim_findTarget(MsgMgrPrimRef self, EvntLoc loc, MsgMgrPrimRef *target)
{
  RepListNode *node;
  MsgMgrPrimRef rep = NULL;
  rep = self->reps[loc];
  if (rep == NULL) {
    for (node = self->theRoot->ft->nextRep(self->theRoot, 0, (EBBRep **)&rep);
	 node != NULL; 
	 node = self->theRoot->ft->nextRep(self->theRoot, node, 
					   (EBBRep **)&rep)) {
      EBBAssert(rep != NULL);
      if (rep->eventLoc == loc) break;
    }
    EBBAssert(rep != NULL);
    self->reps[loc] = rep;
  }
  // FIXME: handle case that rep doesn't yet exist
  *target = rep;
  return EBBRC_OK;
}

static EBBRC 
MsgMgrPrim_msg0(MsgMgrRef _self, EvntLoc loc, MsgHandlerId id)
{
  MsgMgrPrimRef self = (MsgMgrPrimRef)_self;
  MsgStore *msg;  
  MsgMgrPrimRef target;
  EBBRC rc;

  rc = MsgMgrPrim_findTarget(self, loc, &target);
  EBBRCAssert(rc);

  EBBPrimMalloc(sizeof(*msg), &msg, EBB_MEM_DEFAULT);
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
  EBBRCAssert(rc);

  rc = EBBPrimMalloc(sizeof(*msg), &msg, EBB_MEM_DEFAULT);
  EBBRCAssert(rc);
  msg->numargs = 1;
  msg->args[0] = a1;
  MsgMgrPrim_enqueueMsg(target, msg);
  return EBBRC_OK;
}

static EBBRC 
MsgMgrPrim_msg2(MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, uintptr_t a1, uintptr_t a2)
{
  MsgMgrPrimRef self = (MsgMgrPrimRef)_self;
  MsgStore *msg;  
  MsgMgrPrimRef target;
  EBBRC rc;

  rc = MsgMgrPrim_findTarget(self, loc, &target);
  EBBRCAssert(rc);

  rc = EBBPrimMalloc(sizeof(*msg), &msg, EBB_MEM_DEFAULT);
  EBBRCAssert(rc);
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
  EBBRCAssert(rc);

  rc = EBBPrimMalloc(sizeof(*msg), &msg, EBB_MEM_DEFAULT);
  EBBRCAssert(rc);
  msg->numargs = 3;
  msg->args[0] = a1;
  msg->args[1] = a2;
  msg->args[2] = a3;
  MsgMgrPrim_enqueueMsg(target, msg);
  return EBBRC_OK;
};

CObjInterface(MsgMgr) MsgMgrPrim_ftable = {
  .msg0 = MsgMgrPrim_msg0,
  .msg1 = MsgMgrPrim_msg1,
  .msg2 = MsgMgrPrim_msg2,
  .msg3 = MsgMgrPrim_msg3
};

static inline void
MsgMgrPrim_SetFT(MsgMgrPrimRef o)
{
  o->ft = &MsgMgrPrim_ftable;
}
/*
 * routine called by distributed root on a miss
 * to create/return a representative on a core
 */
static EBBRep *
MsgMgrPrim_createRep(CObjEBBRootMultiRef root)
{
  MsgMgrPrimRef repRef;
  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  MsgMgrPrim_SetFT(repRef);
  int i;

  repRef->eventLoc = MyEL();
  repRef->mml = 0;
  repRef->msgqueue = 0;
  for (i=0; i<LRT_PIC_MAX_PICS ; i++ ) {
    repRef->reps[i] = NULL;
  }
  repRef->theRoot = root;
  return (EBBRep *)repRef;
};


MsgMgrId theMsgMgrId;

EBBRC
MsgMgrPrim_Init(void)
{
  CObjEBBRootMultiImpRef rootRef;
  MsgMgrId id;

  if (__sync_bool_compare_and_swap(&theMsgMgrId, (MsgMgrId)0,
				   (MsgMgrId)-1)) {
    CObjEBBRootMultiImpCreate(&rootRef, MsgMgrPrim_createRep);
    id = (MsgMgrId)EBBIdAlloc();
    EBBAssert(id != NULL);

    EBBIdBind((EBBId)id, CObjEBBMissFunc, (EBBMissArg) rootRef);
    theMsgMgrId = id;
  } else {
    while (((volatile uintptr_t)theMsgMgrId)==-1);
  }
  return EBBRC_OK;
}

