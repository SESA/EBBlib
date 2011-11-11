#ifndef __MSG_MGR_H__
#define __MSG_MGR_H__

CObject(MsgHandler);

/* 
 * All Message handling ebbs must conform to these types
 * Any existing ebbs that want to handle messages must be
 * frontended by an message handler EBB
 */
CObjInterface(MsgHandler) {
  /* routines to handle messages sent to me */
  EBBRC (*msg0) (MsgHandlerRef _self);
  EBBRC (*msg1) (MsgHandlerRef _self, uval a1);
  EBBRC (*msg2) (MsgHandlerRef _self, uval a1, uval a2);
  EBBRC (*msg3) (MsgHandlerRef _self, uval a1, uval a2, uval a3);
};

CObjectDefine(MsgHandler) {
  CObjInterface(MsgHandler) *ft;
};

typedef MsgHandlerRef *MsgHandlerId;

CObject(MsgMgr);

/*
 * To send a message to another event location (e.g., core), specify 
 * location, and the ID of the handler object at the target side. This 
 * will normally be the mechanism to start computation on another node 
 * and create local reps as the computation faults on them.
 */
CObjInterface(MsgMgr) {
  /* send message to id at event location */
  EBBRC (*msg0) (MsgMgrRef _self, EvntLoc loc, MsgHandlerId id);

  EBBRC (*msg1) (MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, 
		 uval a1);

  EBBRC (*msg2) (MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, 
		 uval a1, uval a2);

  EBBRC (*msg3) (MsgMgrRef _self, EvntLoc loc, MsgHandlerId id, 
		 uval a1, uval a2, uval a3);
};

CObjectDefine(MsgMgr) {
  CObjInterface(MsgMgr) *ft;
};

// the ID of the one and only Primative MsgHandler
typedef MsgMgrRef *MsgMgrId;

#endif
