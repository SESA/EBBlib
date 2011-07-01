#ifndef __EBB_CTR_H__
#define __EBB_CTR_H__

CObjInterface(EBBCtr) {
  EBBRC (*init) (void *_self);
  EBBRC (*inc)  (void *_self);
  EBBRC (*dec)  (void *_self);
  EBBRC (*val)  (void *_self, uval *v);
};

#endif
