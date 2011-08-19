#ifndef __EBBMGRPRIM_H__
#define __EBBMGRPRIM_H__

CObjInterface(EBBMgrPrim) {
  #include "EBBMgrPrim.iface"
};

CObject(EBBMgrPrim) {
  CObjInterface(EBBMgrPrim) *ft;
  EBBTransLSys *lsys;
};

extern EBBMgrPrimRef *theEBBMgrPrimId;
extern void EBBMgrPrimInit(void);

//FIXME: JA fix typing here on id
static inline EBBRC
EBBAllocLocalPrimId(void *id)
{
  return EC(theEBBMgrPrimId)->AllocLocalId(EB(theEBBMgrPrimId), 
				      id);
}

static inline EBBRC
EBBAllocGlobalPrimId(void *id)
{
  return EC(theEBBMgrPrimId)->AllocGlobalId(EB(theEBBMgrPrimId), 
				      id);
}

static inline EBBRC
EBBBindPrimId(void *id, EBBMissFunc mf, EBBMissArg arg)
{  
  return EC(theEBBMgrPrimId)->BindId(EB(theEBBMgrPrimId), 
				     (EBBId)id, mf, arg);
}

static inline EBBRC
EBBBindGlobalPrimId(void *id, EBBMissFunc mf,
		    EBBMissArg arg, EBBMissFunc globalMF)
{  
  return EC(theEBBMgrPrimId)->BindGlobalId(EB(theEBBMgrPrimId), 
				     (EBBId)id, mf, arg, globalMF);
}



#endif
