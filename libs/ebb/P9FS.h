#ifndef __P9_FS_H__
#define __P9_FS_H__

CObjInterface (P9FS) {
  EBBRC (*attach)(void *_self, Ixp9Req *r);
  EBBRC (*walk)(void *_self, Ixp9Req *r);
  EBBRC (*open)(void *_self, Ixp9Req *r);
  EBBRC (*clunk)(void *_self, Ixp9Req *r);
  EBBRC (*stat)(void *_self, Ixp9Req *r);
  EBBRC (*read)(void *_self, Ixp9Req *r);
  EBBRC (*write)(void *_self, Ixp9Req *r);
  EBBRC (*wstat)(void *_self, Ixp9Req *r);
  EBBRC (*serverloop)(void *_self, char *address);
};

CObject(P9FS) {
  CObjInterface(P9FS) *ft;
};

typedef P9FSRef *P9FSid;

#endif
