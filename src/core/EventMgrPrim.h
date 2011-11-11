#ifndef __EVENTMGR_H__
#define __EVENTMGR_H__

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
  EBBRC (*getHandler) (void *_self, uval eventNo, EventHandlerId *handler); 

  EBBRC (*registerHandler) (void *_self, uval eventNo,
			    EventHandlerId handler, 
			    uval isrc);
  EBBRC (*allocEventNo) (void *_self, uval *eventNoPtr);
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
typedef uval EvntLoc;
extern EvntLoc  EventMgrPrim_GetMyEL();

#endif
