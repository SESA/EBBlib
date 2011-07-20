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
EBBMalloc(uval size, void **mem) {
  return EC(theEBBMemMgrPrimId)->alloc(EB(theEBBMgrPrimId),
				       size, mem);
}

static inline EBBRC
EBBFree(void *mem) {
  return EC(theEBBMemMgrPrimId)->free(EB(theEBBMgrPrimId),
				      mem);
}

#endif
