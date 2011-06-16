#include <ebb.h>
#include <types.h>
#include <SysStatus.h>
#include "ThinWireChan.h"
#include "ThinWireIP.h"
#include "simip.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct ThinWireIPData_struct {
  /* put here the information needed to get to ethernet */
};

static sval
locked_blockedRead(void *p, int length)
{
  return INVK(theThinWireChan, read, IPCUT_CHANNEL, p, length);
}


static sval
locked_blockedWrite(const void *p, int length)
{
  return INVK(theThinWireChan, write, IPCUT_CHANNEL, p, length);
}

static SysStatus
thip_close(ThinWireIPData *self, s32 sock)
{
  struct simipCloseRequest in;
  struct simipCloseResponse out;
  // AutoLock<LockType> al(&lock); // locks now, unlocks on return
  
  char c = SIMIP_CLOSE;
  locked_blockedWrite(&c, 1);
  
  in.sock = sock;
  
  locked_blockedWrite(&in, sizeof(in));
  locked_blockedRead(&out, sizeof(out));
  if (out.rc == 0) {
    return 0;
  }
  
  return _SERROR(1263, 0, out.errnum);
}

static SysStatus
thip_socket(ThinWireIPData *self, s32 *sockPtr, uval type)
{
  struct simipSocketRequest out;
  struct simipSocketResponse in;
  SysStatus rc;
  // AutoLock<LockType> al(&lock); // locks now, unlocks on return

  char c = SIMIP_SOCKET;
  locked_blockedWrite(&c, 1);
  
  out.type = type;
  locked_blockedWrite(&out, sizeof(out));
  
  rc = locked_blockedRead(&in, sizeof(in));
  if (_FAILURE(rc)) {
    return rc;
  }
  *sockPtr = in.sock;
  tassertMsg((in.sock<MAX_SOCKETS), "woops\n");
  return 0;
}

static SysStatus
thip_bind(ThinWireIPData *self, s32 sock, void* addr, uval addrLen)
{
  struct sockaddr_in *sin = (struct sockaddr_in*)addr;
  struct simipBindRequest in;
  struct simipBindResponse out;
  // AutoLock<LockType> al(&lock); // locks now, unlocks on return
  
  char c = SIMIP_BIND;
  locked_blockedWrite(&c, 1);
  
  in.sock = sock;
  in.port = sin->sin_port;
  in.addr = sin->sin_addr.s_addr;
  locked_blockedWrite(&in, sizeof(in));
  locked_blockedRead(&out, sizeof(out));
  if (out.rc < 0) {
    return _SERROR(1259, 0, out.errnum);
  }
  
  return 0;
}

static SysStatus
thip_listen(ThinWireIPData *self, s32 sock, s32 backlog)
{
  struct simipListenRequest in;
  struct simipListenResponse out;
  // AutoLock<LockType> al(&lock); // locks now, unlocks on return
  
  char c = SIMIP_LISTEN;
  locked_blockedWrite(&c, 1);
  
  in.sock = sock;
  in.backlog = backlog;
  locked_blockedWrite(&in, sizeof(in));
  
  locked_blockedRead(&out, sizeof(out));
  if (out.rc<0)
    return _SERROR(1260, 0, out.errnum);
  
  return 0;
}

static SysStatus
thip_accept(ThinWireIPData *self, s32 sock, s32 *clientSockPtr, uval *availablePtr)
{
  struct simipAcceptRequest in;
  struct simipAcceptResponse out;
  // AutoLock<LockType> al(&lock); // locks now, unlocks on return
  
  char c = SIMIP_ACCEPT;
  locked_blockedWrite(&c, 1);
  
  in.sock = sock;
  locked_blockedWrite(&in, sizeof(in));
  locked_blockedRead(&out, sizeof(out));
  
  if (out.rc<0) {
    if (out.block) {
      return _SERROR(1261, WOULD_BLOCK, out.errnum);
    } else {
      return _SERROR(1264, 0, out.errnum);
    }
  }
  
  tassertMsg((out.rc<MAX_SOCKETS), "woops\n");
  *clientSockPtr = out.rc;
  *availablePtr = out.available;
  
  
  return 0;
}

static SysStatusUval
thip_read(ThinWireIPData *self, s32 sock, char *buf, u32 nbytes, uval *availablePtr)
{
  struct simipReadRequest in;
  struct simipReadResponse out;
  // AutoLock<LockType> al(&lock); // locks now, unlocks on return
  
  char c = SIMIP_READ;
  locked_blockedWrite(&c, 1);
  
  in.sock   = sock;
  in.nbytes = nbytes;
  locked_blockedWrite(&in, sizeof(in));
  
  locked_blockedRead(&out, sizeof(out));
  if (out.nbytes < 0) {
    SysStatus rc = _SERROR(1536, 0, out.errnum);
    return rc;
  }
  *availablePtr = out.available;
  
  locked_blockedRead(buf, out.nbytes);
  return out.nbytes;
}

static SysStatusUval
thip_write(ThinWireIPData *self, s32 sock, const char *buf, u32 nbytes)
{
  struct simipWriteRequest in;
  struct simipWriteResponse out;
  // AutoLock<LockType> al(&lock); // locks now, unlocks on return
  
  char c = SIMIP_WRITE;
  locked_blockedWrite(&c, 1);
  
  in.sock   = sock;
  in.nbytes = nbytes;
  locked_blockedWrite(&in, sizeof(in));
  locked_blockedWrite(buf, in.nbytes);
  locked_blockedRead(&out, sizeof(out));
  if (out.nbytes < 0) {
    SysStatus rc = _SERROR(1537, 0, out.errnum);
    return rc;
  }
  nbytes = out.nbytes;
  return nbytes;
}

static SysStatusUval
thip_sendto(ThinWireIPData *self, s32 sock, const char *buf, u32 nbytes, void* addr, uval addrLen)
{
    struct sockaddr_in *sin = (struct sockaddr_in*)addr;
    struct simipSendtoRequest out;
    struct simipSendtoResponse in;
    // AutoLock<LockType> al(&lock); // locks now, unlocks on return

    char c = SIMIP_SENDTO;
    locked_blockedWrite(&c, 1);

    out.sock   = sock;
    out.nbytes = nbytes;
    if (sin) {
	out.port   = sin->sin_port;
	out.addr   = sin->sin_addr.s_addr;
    } else {
	// these values should get ignored since the socket is bound
	out.port   = 0;
	out.addr   = 0;
    }

    locked_blockedWrite(&out, sizeof(out));
    locked_blockedWrite(buf, out.nbytes);
    locked_blockedRead(&in, sizeof(in));
    if (in.nbytes < 0) {
	SysStatus rc = _SERROR(1258, 0, in.errnum);
	return rc;
    }
    nbytes = in.nbytes;
    return nbytes;
}

static SysStatusUval
thip_recvfrom(ThinWireIPData *self, s32 sock, char *buf, u32 nbytes, uval *availablePtr, void* addr, uval addrLen)
{
    struct simipRecvfromRequest in;
    struct simipRecvfromResponse out;
    // AutoLock<LockType> al(&lock); // locks now, unlocks on return

    char c = SIMIP_RECVFROM;
    locked_blockedWrite(&c, 1);

    in.sock   = sock;
    in.nbytes = nbytes;
    locked_blockedWrite(&in, sizeof(in));

    locked_blockedRead(&out, sizeof(out));
    if (out.nbytes < 0) {
	SysStatus rc = _SERROR(1255, 0, out.errnum);
	return rc;
    }

    *availablePtr = out.available;

    struct sockaddr_in *sin = (struct sockaddr_in*)addr;

    if (sin) {
	sin->sin_family = AF_INET;
	sin->sin_port = out.port;
	sin->sin_addr.s_addr  = out.addr;
	addrLen = sizeof(struct sockaddr_in);
    }

    locked_blockedRead(buf, out.nbytes);
    return out.nbytes;
}

static SysStatusUval
thip_connect(ThinWireIPData *self, s32 sock, void* addr, uval addrLen)
{
  struct sockaddr_in *sin = (struct sockaddr_in*)addr;
  
  tassert(sin->sin_family == AF_INET, "unsupported addr family\n");
  
  struct simipConnectRequest in;
  struct simipConnectResponse out;
  // AutoLock<LockType> al(&lock); // locks now, unlocks on return
  
  char c = SIMIP_CONNECT;
  locked_blockedWrite(&c, 1);
  
  in.sock = sock;
  in.port = sin->sin_port;
  in.addr = sin->sin_addr.s_addr;
  locked_blockedWrite(&in, sizeof(in));
  locked_blockedRead(&out, sizeof(out));
  if (out.rc<0) return _SERROR(1538, 0, out.errnum);
  
  return 0;
}

static SysStatus
thip_getThinEnvVar(ThinWireIPData *self, const char *envVarName, char *envVarValue)
{
  SysStatus rc;
  struct simipGetEnvVarRequest out;
  struct simipGetEnvVarResponse in;
  // AutoLock<LockType> al(&lock); // locks now, unlocks on return
  
  char c = SIMIP_GETENVVAR;
  
  if ((rc = locked_blockedWrite(&c, 1)) < 0) {
    return rc;
  }
  
  strcpy(out.envVarName, envVarName);
  locked_blockedWrite(&out, sizeof(out));
  
  locked_blockedRead(&in, sizeof(in));
  strcpy(envVarValue, in.envVarValue);
  
  return 0;
}

void 
theThinWireIPInit()
{
  // initialize the thinwire channel
  // assign reference
  // assign function pointers
  // without late binding, fill all the tables with the reference
}


