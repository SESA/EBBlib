#ifndef __EBBMGRPRIM_H__
#define __EBBMGRPRIM_H__

CObjInterface(EBBMgrPrim) {
  #include "EBBMgrPrim.iface"
};

CObject(EBBMgrPrim) {
  CObjInterface(EBBMgrPrim) *ft;
  //JA Hack
  void *myRoot;
  EBBTransLSys *lsys;
};

extern EBBMgrPrimRef *theEBBMgrPrimId;
extern void EBBMgrPrimInit(void);

//FIXME: JA fix typing here on id
static inline EBBRC
EBBAllocPrimId(void *id)
{
  return EC(theEBBMgrPrimId)->AllocId(EB(theEBBMgrPrimId), 
				      id);
}

static inline EBBRC
EBBBindPrimId(void *id, EBBMissFunc mf, EBBMissArg arg)
{  
  return EC(theEBBMgrPrimId)->BindId(EB(theEBBMgrPrimId), 
				     (EBBId)id, mf, arg);
}



#endif
