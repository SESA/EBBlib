#ifndef __EBB_9P_CLIENT_H__
#define __EBB_9P_CLIENT_H__

#define IXP_NO_P9_
#define IXP_P9_STRUCTS
#include <ixp.h>

CObjInterface(EBB9PClient) {
  EBBRC (*init)      (void *_self);
  EBBRC (*mount)     (void *_self, char *address);
  EBBRC (*unmount)   (void *_self);
  EBBRC (*ismounted) (void *_self);
  EBBRC (*open)      (void *_self, char *path, uval8 mode, IxpCFid **fd);
  EBBRC (*create)    (void *_self, char *path, uint perm, uval8 mode, IxpCFid **fd);
  EBBRC (*close)     (void *_self, IxpCFid *fd, int *rc);
  EBBRC (*read)      (void *_self, IxpCFid *fd, void *buf, sval cnt, 
		      sval *n);
  EBBRC (*write)     (void *_self, IxpCFid *fd, const void *buf, sval cnt,
		      sval *n);
};

CObject(EBB9PClient) {
  CObjInterface(EBB9PClient) *ft;
};

typedef EBB9PClientRef *EBB9PClientId;

#endif  // __EBB_9P_CLIENT_H__
