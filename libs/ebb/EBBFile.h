#ifndef __EBB_FILE_H__
#define __EBB_FILE_H__

enum { EBBFILE_OREAD = 1, EBBFILE_OWRITE=2, EBBFILE_ORDWR=4, EBBFILE_OCREATE=8 } EBBFileMode;

CObjInterface(EBBFile) {
  EBBRC (*open)  (void *_self, char *path, uval mode, uval perm);
  EBBRC (*close) (void *_self, sval *rc);
  EBBRC (*read)  (void *_self, void *buf, sval cnt, sval *n);
  EBBRC (*write) (void *_self, const void *buf, sval cnt, sval *n);
  EBBRC (*seek)  (void *_self, sval offset, uval whence, sval *rc);
};

CObject(EBBFile) {
  CObjInterface(EBBFile) *ft;
};

typedef EBBFileRef *EBBFileId;

#endif
