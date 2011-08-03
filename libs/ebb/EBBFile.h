#ifndef __EBB_FILE_H__
#define __EBB_FILE_H__

CObjInterface(EBBFile) {
  EBBRC (*open)  (void *_self, char *path, uval8 mode);
  EBBRC (*close) (void *_self, int *rc);
  EBBRC (*read)  (void *_self, void *buf, sval cnt, sval *n);
  EBBRC (*write) (void *_self, const void *buf, sval cnt, sval *n);
};

CObject(EBBFile) {
  CObjInterface(EBB9PClient) *ft;
};

typedef EBBFileRef *EBBFileId;

#endif
