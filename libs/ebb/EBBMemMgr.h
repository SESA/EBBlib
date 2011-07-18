#ifndef __EBBMEMMGR_H__
#define __EBBMEMMGR_H__

CObjInterface(EBBMemMgr) {
  EBBRC (*init) (void *_self);
  EBBRC (*alloc) (void *_self, uval size, void **mem);
  EBBRC (*free) (void * _self, void *mem);
};

#endif
