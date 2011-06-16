#ifndef __THINWIREIP_H_
#define __THINWIREIP_H_

/*****************************************************************************
 * Module Description: Does passthrough over thinwire to host system
 * for IP and FS access. Note, eventually should create new objects to
 * represent open files and created sockets, for now, just return
 * numbers to get things going.
 * **************************************************************************/
  
typedef struct ThinWireIP_struct ThinWireIP; 
typedef struct ThinWireIPData_struct ThinWireIPData;

enum {MAX_SOCKETS=256};

// error specific to class
enum {
  WOULD_BLOCK=56
};

typedef struct ThinWireIPInterface_struct {
  SysStatus (*socket) (ThinWireIPData *self, s32 *sockPtr, uval type);
  SysStatus (*close) (ThinWireIPData *self, s32 sock);
  SysStatus (*bind) (ThinWireIPData *self, s32 sock, void* addr, uval addrLen);
  SysStatus (*listen) (ThinWireIPData *self, s32 sock, s32 backlog);
  SysStatus (*accept) (ThinWireIPData *self, s32 sock, s32 *clientSockPtr, uval *availablePtr);
  SysStatusUval (*read) (ThinWireIPData *self, s32 sock, char *buf, u32 nbytes, uval *availablePtr);
  SysStatusUval (*write) (ThinWireIPData *self, s32 sock, const char *buf, u32 nbytes);
  SysStatusUval (*sendto) (ThinWireIPData *self, s32 sock, const char *buf, u32 nbytes, void* addr, uval addrLen);
  SysStatusUval (*recvfrom) (ThinWireIPData *self, s32 sock, char *buf, u32 nbytes, uval *availablePtr, void* addr, uval addrLen);
  SysStatusUval (*connect) (ThinWireIPData *self, s32 sock, void* addr, uval addrLen);
  SysStatus (*getThinEnvVar) (ThinWireIPData *self, const char *envVarName, char *envVarValue);
} ThinWireIPInterface;

struct ThinWireIP_struct {
  ThinWireIPInterface *itf;
  ThinWireIPData *data;
};

/* 
 * This is a weird object, since there is only one of these, and it
 * will have a well known ID, for now, put in a reference to the
 * global thin wireIP, later use well defined slot in EBB table.
 */
extern void theThinWireIPInit(void);
extern ThinWireIP *theThinWireIP;

#endif
