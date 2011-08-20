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

static inline EBBRC
EBBMessageNode(uval nodeid, MsgHandler h)
{
  return EC(theEBBMgrPrimId)->MessageNode(EB(theEBBMgrPrimId), 
					  nodeid, h, 
					  0, 0, 0, 0,
					  0, 0, 0, 0);
}

static inline EBBRC
EBBMessageNode1(uval nodeid, MsgHandler h, uval a0)
{
  return EC(theEBBMgrPrimId)->MessageNode(EB(theEBBMgrPrimId), 
					  nodeid, h, 
					  a0, 0, 0, 0,
					  0, 0, 0, 0);
    
}

static inline EBBRC
EBBMessageNode2(uval nodeid, MsgHandler h, uval a0, uval a1)
{
  return EC(theEBBMgrPrimId)->MessageNode(EB(theEBBMgrPrimId), 
					  nodeid, h, 
					  a0, a1, 0 ,0,
					  0, 0, 0, 0);

    
}

static inline EBBRC
EBBMessageNode3(uval nodeid, MsgHandler h, uval a0, uval a1, uval a2) 
{
  return EC(theEBBMgrPrimId)->MessageNode(EB(theEBBMgrPrimId), 
					  nodeid, h, 
					  a0, a1, a2, 0,
					  0, 0, 0, 0);
    
}

static inline EBBRC
EBBMessageNode4(uval nodeid, MsgHandler h, uval a0, uval a1, uval a2, 
		uval a3)
{
  return EC(theEBBMgrPrimId)->MessageNode(EB(theEBBMgrPrimId), 
					  nodeid, h, 
					  a0, a1, a2, a3,
					  0, 0, 0, 0);
    
}

static inline EBBRC
EBBMessageNode5(uval nodeid, MsgHandler h, uval a0, uval a1, uval a2, 
		uval a3, uval a4)
{
  return EC(theEBBMgrPrimId)->MessageNode(EB(theEBBMgrPrimId), 
					  nodeid, h, 
					  a0, a1, a2, a3,
					  a4, 0, 0, 0);
    
}

static inline EBBRC
EBBMessageNode6(uval nodeid, MsgHandler h, uval a0, uval a1, uval a2, 
		uval a3, uval a4, uval a5)
{
  return EC(theEBBMgrPrimId)->MessageNode(EB(theEBBMgrPrimId), 
					  nodeid, h, 
					  a0, a1, a2, a3,
					  a4, a5, 0, 0);
    
}

static inline EBBRC
EBBMessageNode7(uval nodeid, MsgHandler h, uval a0, uval a1, uval a2, 
		uval a3, uval a4, uval a5, uval a6)
{
  return EC(theEBBMgrPrimId)->MessageNode(EB(theEBBMgrPrimId), 
					  nodeid, h, 
					  a0, a1, a2, a3,
					  a4, a5, a6, 0);
    
}

static inline EBBRC
EBBMessageNode8(uval nodeid, MsgHandler h, uval a0, uval a1, uval a2, 
		uval a3, uval a4, uval a5, uval a6, uval a7)
{
  return EC(theEBBMgrPrimId)->MessageNode(EB(theEBBMgrPrimId), 
					  nodeid, h, 
					  a0, a1, a2, a3,
					  a4, a5, a6, a7);
    
}

#endif
