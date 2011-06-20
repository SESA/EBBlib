#include "../types.h"
#include "obj.h"
#include "EBBConst.h"
#include "sys/trans.h"
#include "EBBTypes.h"
#include "EBBAssert.h"
#include "EBBMgr.H"

/***** Primative EBBMgr -- EBB Infrastructure and its Management ******/
/* JA: A First hack at a simple version so that we can get going      */
/*     inspired by experience from K42 CO system                      */
/* We really need to work on Events and how they relate to EBB system */
/**********************************************************************/

// JA: KLUDGE  

// static space for pinned translations both global and local
typedef enum 
{ NumPrimitiveEBBS = EBB_NUM_PRIMITIVE_EBBS, NumELS=EBB_NUM_ELS } 
EBBMgrConst; 

EBBTrans PrimitiveGTrnsTbl[NumPrimitiveEBBS];
EBBTrans PrimitiveLTrnsTbls[NumPrimitiveEBBS * NumELS];

EBBMgrPrimitiveId theEBBMgrPrimitiveId;

// usually one does not need to declare the root here
// but the EBBMgrRoot is special because ot the interdependencies
// of initialization

ObjVTable(EBBMgrPrimitiveRoot) {
#include "EBBRoot.iface"
} theRootVTable;

ObjData(EBBMgrPrimitiveRoot) {
  EBBTrans    *rootTable;
  EBBTrans    *repTables;
  EBBTransSys  transSys;
  // JA KLUDGE: array of static reps
  struct EBBMgrPrimitiveRepData {
    struct EBBMgrPrimitiveRootData *myRoot;
    EBBTrans *myPrimitiveRootTable;
    EBBTrans *myPrimitiveRepTable;
  } repData[NumELS];
  EBBMgrPrimitive reps[NumELS];
} theRootData = { PrimitiveGTrnsTbl, PrimitiveLTrnsTbls, 
		  { PrimitiveGTrnsTbl, PrimitiveLTrnsTbls }};
typedef uval EBBIndex;

static inline Obj * TransToObj(EBBTrans *t) 
{
  return (Obj *)t;
}

static inline EBBTrans * ObjToTrans(Obj *o) 
{
  return (EBBTrans *)o;
}

static inline EBBIndex IdToIdx(EBBId id) 
{
  return 0;
}

static inline EBBId IdxToId(EBBIndex idx)
{
  return NULL;
}

static inline EBBTrans * IdToRootTrans(EBBId id)
{
  return theRootData.rootTable + IdToIdx(id);
}


static inline EBBRootRef IdToRootRef(EBBId id)
{
  return TransToObj(IdToRootTrans(id));
}

static inline installObj(EBBTrans *t, Obj *o) 
{
  Obj *eo = TransToObj(t);
  *eo = *o;
}

static inline bindRoot(EBBId id, EBBRootRef r) 
{
  EBBTrans *t = IdToRootTrans(id);
  installObj(t, r);
}

static inline bindRep(EBBId id, EBBRepRef r) 
{
  EBBTrans *t = EBBIdToRepTrans(t);
  installObj(t, r);
}

static inline EBBRC bind(EBBId id, EBBInst inst) 
{ 
  EBBRootRef root = inst;
  
  bindRoot(id, root);

  #ifdef NO_TRANS_FAULTS
  {
    EL el;
    EBBRep rep;
    for (el=0; el<NumELS; el++) {
      OBJ_INVK(root, getRep, EL, &rep);
      bindRep(EL, theEBBMgrPrimitiveId, rep)
    } 
  }
  #endif
}

static inline EBBRC allocId(EBBId *id) {
  *id = &(theRootData.repTables[0]);
  return EBBRC_OK;
}

static EBBRC 
EBBMgrCreateInst(EBBInst *inst)
{
  static Obj theRoot = { &theRootVTable, 
			    &theRootData };
  *inst = &theRoot;
  return EBBRC_OK;
}
 
extern EBBRC 
EBBMgrInit(void) 
{
  EBBRC rc;
  EBBInst ebbmgr;

  // assert basic runtime facts
  // ensure crucial relationship between and object and
  EBBAssert(sizeof(Obj) == sizeof(EBBTrans));

  // create instance
  rc = EBBMgrCreateInst(&ebbmgr);
  EBBAssert(EBBRC_SUCCESS(rc));
  
  // allocate an id for the EBBMgr
  rc = allocId((EBBId *)&theEBBMgrPrimitiveId);
  EBBAssert(EBBRC_SUCCESS(rc));

  rc = bind(theEBBMgrPrimitiveId, ebbmgr);
  EBBAssert(EBBRC_SUCCESS(rc));

  // from now on all calls to the EBBMgr can go through is
  // external interface
  rc = INVK(TheEBBMgrID(), init);

  return rc;
}

