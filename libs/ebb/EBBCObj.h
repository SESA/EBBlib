#ifndef __EBB_COBJ_H__
#define __EBB_COBJ_H__

extern EBBMissFunc EBBCObjMissFunc;

static inline
EBBRC
EBBCObjBind(void *id, void *root)
{
  return EBBBindPrimId(id, EBBCObjMissFunc, (EBBMissArg) root);
}

#define EB(ID) EBBId_DREF(ID)
#define EC(ID) EB(ID)->ft

#endif
