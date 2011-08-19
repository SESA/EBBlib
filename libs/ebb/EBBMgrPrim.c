#include "../base/include.h"
#include "../cobj/cobj.h"
#include "sys/trans.h" //FIXME: move EBBTransLSys out of this header
#include "../base/lrtio.h"
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "EBBMgrPrim.h"
#include "EBBAssert.h"
#include "EBB9PClient.h"

#define panic() (*(uval *)0)

extern uval EBBNodeId;

static EBBRC
AllocLocalId (void *_self, void **id) {
  EBBMgrPrimRef self = (EBBMgrPrimRef)_self;
  *id = (void *)EBBIdAllocLocal(self->lsys);
  return EBBRC_OK;
}

static EBBRC
AllocGlobalId (void *_self, void **id) {
  EBBMgrPrimRef self = (EBBMgrPrimRef)_self;
  if (!isGlobalSetup(self->lsys)) {
    if (EBBNodeId != 0) {
      SetupGlobal(self->lsys, EBBNodeId);
    } else {
      *id = NULL;
      return EBBRC_GENERIC_FAILURE;
    }
  }

  *id = (void *)EBBIdAllocGlobal(self->lsys);
  return EBBRC_OK;
}

static EBBRC
FreeId (void *_self, EBBId id) {
  EBBMgrPrimRef self = (EBBMgrPrimRef)_self;
  EBBIdFree(self->lsys, id);
  return EBBRC_OK;
}

static EBBRC
BindId (void *_self, EBBId id, EBBMissFunc mf, EBBMissArg arg) {
  //  EBBMgrPrimRef self = (EBBMgrPrimRef)_self;
  EBBIdBind(id, mf, arg);
  return EBBRC_OK;
}

static EBBRC
BindGlobalId (void *_self, EBBId id, EBBMissFunc mf, EBBMissArg arg,
	      EBBMissFunc globalMF) {
  EBBIdBindGlobal(id, mf, arg, globalMF);
  return EBBRC_OK;
}

static EBBRC
UnBindId (void *_self, EBBId id, EBBMissFunc *mf, EBBMissArg *arg) {
  //  EBBMgrPrimRef self = (EBBMgrPrimRef)_self;
  EBBIdUnBind(id, mf, arg);
  return EBBRC_OK;
}

static CObjInterface(EBBMgrPrim) EBBMgrPrim_ftable = {
  AllocLocalId, AllocGlobalId, FreeId, BindId, BindGlobalId, UnBindId
};

//FIXME: have to statically allocate these because there is
//       no memory manager
EBBMgrPrim EBBMgrPrimLObjs[EBB_TRANS_MAX_ELS];
EBBTransLSys EBBMgrPrimLTrans[EBB_TRANS_MAX_ELS];

typedef struct EBBTransGSysStruct {
  EBBGTrans *gTable;
  uval pages;
} EBBTransGSys;


static void EBBMgrPrimRepInit (EBBLTrans *lt, EBBMgrPrimRef ref,
			EBBTransGSys gsys) {
  int numGTransPerEL;
  numGTransPerEL = gsys.pages * EBB_TRANS_PAGE_SIZE / 
    sizeof(EBBGTrans) / EBB_TRANS_MAX_ELS / EBB_TRANS_MAX_NODES;
  ref->lsys = &EBBMgrPrimLTrans[EBBMyEL()];
  ref->lsys->localGTable = &gsys.gTable[numGTransPerEL * EBBMyEL()];
  ref->lsys->localLTable = EBBGTransToLTrans(ref->lsys->localGTable);
  ref->lsys->localFree = NULL;
  ref->lsys->localNumAllocated = 0;
  ref->lsys->localSize = numGTransPerEL;
  ref->lsys->globalGTable = NULL;
  ref->lsys->globalLTable = NULL;
  ref->lsys->globalFree = NULL;
  ref->lsys->globalNumAllocated = 0;
  ref->lsys->globalSize = 0;
  ref->ft = &EBBMgrPrim_ftable;
}

static EBBRC EBBMgrPrimMF (void *_self, EBBLTrans *lt,
		    FuncNum fnum, EBBMissArg arg) {
  EBBTransGSys gsys = *(EBBTransGSys *)arg;
  EBBMgrPrimRef ref = &EBBMgrPrimLObjs[EBBMyEL()];
  EBBMgrPrimRepInit(lt, ref, gsys);
  EBBCacheObj(lt, ref);
  *(EBBMgrPrimRef *)_self = ref;
  return EBBRC_OK;
}

/* static EBBRC EBBMgrPrimERRMF (void *_self, EBBLTrans *lt, */
/* 			      FuncNum fnum, EBBMissArg arg) { */
/*   EBB_LRT_printf("ERROR: unbound object invoked with self: %p, lt: %p," */
/* 	 "fnum: %ld, arg: %ld\n", _self, lt, fnum, arg); */
/*   return EBBRC_GENERIC_FAILURE; */
/* } */
extern EBB9PClientId the9PClient;

static EBBRC EBBMgrPrimERRMF (void *_self, EBBLTrans *lt,
			      FuncNum fnum, EBBMissArg arg) {
  if (isLocalEBB(EBBLTransToGTrans(lt))) {
    EBB_LRT_printf("ERROR: gtable miss on a local-only EBB\n");
  } else if (!the9PClient) {
    EBB_LRT_printf("ERROR: gtable miss on a global EBB but not connected!\n");
  } else if (getLTransNodeId(lt) == EBBNodeId) {
    EBB_LRT_printf("ERROR: gtable miss on a global EBB but we are the home node!\n");
  } else {
    EBB_LRT_printf("gtable miss on a global EBB: unimplemented\n");
    //call the home node, which should return an EBBMissFunc to handle this call
  }

  return EBBRC_GENERIC_FAILURE;
}


static EBBTransGSys gsys;

EBBMissFunc theERRMF;
struct EBB_Trans_Mem EBB_Trans_Mem;

EBBMgrPrimRef *theEBBMgrPrimId;

void EBBMgrPrimInit () {
  EBBRC rc;
  EBBId id;

  EBB_Trans_Mem_Init();
  gsys.pages = EBB_TRANS_NUM_PAGES;
  EBB_Trans_Mem_Alloc_Pages(gsys.pages, (uval8 **)&gsys.gTable);
  theERRMF = EBBMgrPrimERRMF;
  initGTable(gsys.gTable, gsys.pages);
  initAllLTables(EBBGTransToId(gsys.gTable), gsys.pages);

  // manually binding the EBBMgrPrim in
  theEBBMgrPrimId = (EBBMgrPrimRef *)EBBGTransToId(gsys.gTable);
  EBBIdBind((EBBId)theEBBMgrPrimId, EBBMgrPrimMF, (EBBMissArg)&gsys);
  
  // do an alloc to account for manual binding 
  rc = EBBAllocLocalPrimId(&id);

  EBBRCAssert(rc);
  EBBAssert(id == (EBBId)theEBBMgrPrimId);

}
