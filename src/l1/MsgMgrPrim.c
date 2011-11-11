#include <config.h>
#include <types.h>
#include <lrt/io.h>
#include <core/lrt/pic.h>
#include <lrt/assert.h>
#include <core/cobj/cobj.h>
#include <core/types.h>
#include <core/cobj/CObjEBB.h>
#include <core/EBBMgrPrim.h>
#include <core/MemMgr.h> 
#include <core/MemMgrPrim.h>
#include <core/EventMgrPrim.h>

#include <l1/MsgMgr.h>
#include <l1/MsgMgrPrim.h>

// FIXME: need this to do IPI, do we really want to do these inline and bypass
//   EventMgr???
#include __LRTINC(pic.h)


/* -- start routines & types to be implemented, put somewhere global*/
typedef long LockType;
 
static void
spinLock(LockType *lk)
{
  uval rc = 0;
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
#define MAXARGS 3
/* -- end routines to be implemented */

typedef struct MsgStore_struc {
  struct MsgStore_struc *next;
  uval numargs;
  uval args[MAXARGS];
} MsgStore;


CObject(MsgMgrPrim) {
  CObjInterface(MsgMgr) *ft;
  uval eventLoc;
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
  uval queueempty = 1;
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
    for (node = self->theRoot->ft->nextRep(self->theRoot, 0, &rep);
	 node != NULL; 
	 node = self->theRoot->ft->nextRep(self->theRoot, node, &rep)) {
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
MsgMgrPrim_msg1(MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, uval a1)
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
MsgMgrPrim_msg2(MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, uval a1, uval a2)
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
		uval a1, uval a2, uval a3)
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
static void *
MsgMgrPrim_createRep(CObjEBBRootMultiRef _theRoot)
{
  MsgMgrPrimRef repRef;
  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  MsgMgrPrim_SetFT(repRef);
  int i;

  repRef->eventLoc = EventMgrPrim_GetMyEL();
  repRef->mml = 0;
  repRef->msgqueue = 0;
  for (i=0; i<LRT_PIC_MAX_PICS ; i++ ) {
    repRef->reps[i] = NULL;
  }
  repRef->theRoot = _theRoot;
  return repRef;
};


EBBRC
MsgMgrPrim_Create(MsgMgrId *id)
{
  EBBRC rc;
  CObjEBBRootMultiRef rootRef;
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  CObjEBBRootMultiSetFT(rootRef);
  rootRef->ft->init(rootRef, MsgMgrPrim_createRep);

  rc = EBBAllocPrimId(id);
  //  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef);
  //  EBBRCAssert(rc);

  return EBBRC_OK;
}

