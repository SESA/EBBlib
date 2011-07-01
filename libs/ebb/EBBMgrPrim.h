#ifndef __EBBMGRPRIM_H__
#define __EBBMGRPRIM_H__

CObjInterface(EBBMgrPrim) {
  #include "EBBMgrPrim.iface"
};

CObject(EBBMgrPrim) {
  CObjInterface(EBBMgrPrim) *ft;
  EBBTransLSys lsys;
};

extern EBBMgrPrimRef *theEBBMgrPrimId;
extern void EBBMgrPrimInit(void);

//FIXME: JA fix typing here on id
static inline EBBRC
EBBAllocPrimId(void *id)
{
  return EBBId_DREF(theEBBMgrPrimId)->ft
    ->AllocId(EBBId_DREF(theEBBMgrPrimId), &id);
}

static inline EBBRC
EBBBindPrimId(void *id, EBBMissFunc mf, EBBMissArg arg)
{  
  return EBBId_DREF(theEBBMgrPrimId)->ft
    ->BindId(EBBId_DREF(theEBBMgrPrimId), (EBBId)id, mf, arg);
}



#endif
