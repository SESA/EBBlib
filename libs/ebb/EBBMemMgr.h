#ifndef __EBBMEMMGR_H__
#define __EBBMEMMGR_H__

typedef enum {EBB_MEM_LOCAL, EBB_MEM_LOCAL_PADDED,
	      EBB_MEM_GLOBAL, EBB_MEM_GLOBAL_PADDED,
	      EBB_MEM_DEFAULT = EBB_MEM_GLOBAL} EBB_MEM_POOL;

CObjInterface(EBBMemMgr) {
  EBBRC (*init) (void *_self);
  EBBRC (*alloc) (void *_self, uval size, void *mem, EBB_MEM_POOL pool);
  EBBRC (*free) (void * _self, void *mem);
};

#endif
