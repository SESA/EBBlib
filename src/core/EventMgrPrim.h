#ifndef __EVENTMGR_H__
#define __EVENTMGR_H__

/* 
 * All event handling ebbs must conform to these types
 * Any existing ebbs that want to handle events must be
 * frontended by an event handler EBB
 */
CObjInterface(EBBEventHandler) {
  EBBRC (*handleEvent) (void *_self);
  EBBRC (*init) (void *_self);
};

CObject(EBBEventHandler) {
  CObjInterface(EBBEventHandler) *ft;
};

typedef EBBEventHandlerRef *EBBEventHandlerId;

CObjInterface(EBBEventMgrPrim) {
  EBBRC (*registerHandler) (void *_self, uval eventNo,
			    EBBEventHandlerId handler, 
			    uval isrc);
  EBBRC (*allocEventNo) (void *_self, uval *eventNoPtr);
};

CObject(EBBEventMgrPrim) {
  CObjInterface(EBBEventMgrPrim) *ft;
};

typedef EBBEventMgrPrimRef *EBBEventMgrPrimId;
// the ID of the one and only event manager
extern EBBEventMgrPrimId theEBBEventMgrPrimId;

#endif
