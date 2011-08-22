#ifndef __EBB_CTR_PRIM_H__
#define __EBB_CTR_PRIM_H__

CObject(EBBCtrPrim);
typedef EBBCtrPrimRef *EBBCtrPrimId;

CObjectDefine(EBBCtrPrim) {
  CObjInterface(EBBCtr) *ft;
  uval v;
  EBBCtrPrimId id;
};

extern CObjInterface(EBBCtr) EBBCtrPrim_ftable;

static inline void 
EBBCtrPrimSetFT(EBBCtrPrimRef o) { o->ft = &EBBCtrPrim_ftable; }

extern EBBRC EBBCtrPrimSharedCreate(EBBCtrPrimId *id);
extern EBBRC EBBCtrPrimGlobalSharedCreate(EBBCtrPrimId *id);


#endif
