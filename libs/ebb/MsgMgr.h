#ifndef __MSG_MGR_H__
#define __MSG_MGR_H__

typedef EBBRC (*MsgHandler)(uval a0, uval a1, uval a2, uval a3, 
			    uval a4, uval a5, uval a6, uval a7,
			    uval *rcode); 
struct MsgMgrMsg {
  MsgHandler h;
  uval a0, a1, a2, a3, a4, a5, a6, a7;
} __attribute__((__packed__));

struct MsgMgrReply {
  uval rc;
} __attribute__((__packed__));



#endif
