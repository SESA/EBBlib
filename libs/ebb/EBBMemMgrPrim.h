#ifndef __EBB_MEMMGRPRIM_H__
#define __EBB_MEMMGRPRIM_H__

CObject(EBBMemMgrPrim) {
  CObjInterface(EBBMemMgr) *ft;
  void *mem;
  uval len;
};

extern CObjInterface(EBBMemMgr) EBBMemMgrPrim_ftable;

static inline void
EBBMemMgrPrimSetFT(EBBMemMgrPrimRef o) {o->ft = &EBBCtrPrim_ftable; }

typedef EBBMemMgrPrimRef *EBBMemMgrPrimId;
extern EBBRC EBBMemMgrPrimSharedCreate(EBBMemMgrPrimId *id);

#endif
