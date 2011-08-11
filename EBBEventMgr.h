#ifndef __EBB_EVENT_MGR_H__
#define __EBB_EVENT_MGR_H__

CObjInterface (EBBEventMgr) {
  EBBRC (*start)(void *_self);
  EBBRC (*end)(void *_self);
};

#endif
