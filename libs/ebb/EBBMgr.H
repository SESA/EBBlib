#ifndef __EBB_EBBMGR_H__
#define __EBB_EBBMGR_H__

EBBCreateTypes(EBBMgrPrimitive);

ObjVTable(EBBMgrPrimitive) {
  #include "EBBMgr.iface"
};

extern EBBMgrPrimitiveId theEBBMgrPrimitiveId;

static inline EBBMgrPrimitiveId TheEBBMgrPrimitiveId(void) 
{ 
  return theEBBMgrPrimitiveId; 
}

extern EBBRC EBBMgrInit(void);
#endif
