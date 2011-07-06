#ifndef __EBB_CTR_PRIM_H__
#define __EBB_CTR_PRIM_H__

CObject(EBBCtrPrim) {
  CObjInterface(EBBCtr) *ft;
  uval v;
};

extern CObjInterface(EBBCtr) EBBCtrPrim_ftable;

static inline void 
EBBCtrPrimSetFT(EBBCtrPrimRef o) { o->ft = &EBBCtrPrim_ftable; }

typedef EBBCtrPrimRef *EBBCtrPrimId;
extern EBBRC EBBCtrPrimSharedCreate(EBBCtrPrimId *id);


#endif
