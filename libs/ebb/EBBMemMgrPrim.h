#ifndef __EBB_MEMMGRPRIM_H__
#define __EBB_MEMMGRPRIM_H__

CObject(EBBMemMgrPrim) {
  CObjInterface(EBBMemMgr) *ft;
  void *mem;
  uval len;
};

extern CObjInterface(EBBMemMgr) EBBMemMgrPrim_ftable;

static inline void
EBBMemMgrPrimSetFT(EBBMemMgrPrimRef o) {o->ft = &EBBMemMgrPrim_ftable; }

typedef EBBMemMgrPrimRef *EBBMemMgrPrimId;
extern EBBRC EBBMemMgrPrimInit(void);
extern EBBMemMgrPrimRef *theEBBMemMgrPrimId;

static inline EBBRC
EBBPrimMalloc(uval size, void *mem, EBB_MEM_POOL pool) {
  return EC(theEBBMemMgrPrimId)->alloc(EB(theEBBMemMgrPrimId),
				       size, mem, pool);
}

static inline EBBRC
EBBPrimFree(void *mem) {
  return EC(theEBBMemMgrPrimId)->free(EB(theEBBMemMgrPrimId),
				      mem);
}

#endif
