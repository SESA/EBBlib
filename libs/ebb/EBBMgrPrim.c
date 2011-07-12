#include "../cobj/cobj.h"
#include "sys/trans.h" //FIXME: move EBBTransLSys out of this header
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "EBBMgrPrim.h"

#define panic() (*(uval *)0)

static EBBRC
AllocId (void *_self, void **id) {
  EBBMgrPrimRef self = (EBBMgrPrimRef)_self;
  *id = (void *)EBBIdAlloc(self->lsys);
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
  EBBMgrPrimRef self = (EBBMgrPrimRef)_self;
  EBBIdBind(id, mf, arg);
  return EBBRC_OK;
}

static EBBRC
UnBindId (void *_self, EBBId id, EBBMissFunc *mf, EBBMissArg *arg) {
  EBBMgrPrimRef self = (EBBMgrPrimRef)_self;
  EBBIdUnBind(id, mf, arg);
  return EBBRC_OK;
}

static CObjInterface(EBBMgrPrim) EBBMgrPrim_ftable = {
  AllocId, FreeId, BindId, UnBindId
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
    sizeof(EBBGTrans) / EBB_TRANS_MAX_ELS;
  ref->lsys = &EBBMgrPrimLTrans[EBBMyEL()];
  ref->lsys->gTable = &gsys.gTable[numGTransPerEL * EBBMyEL()];
  ref->lsys->lTable = EBBGTransToLTrans(ref->lsys->gTable);
  ref->lsys->free = NULL;
  ref->lsys->numAllocated = 0;
  ref->lsys->size = numGTransPerEL;
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

//FIXME: Kludge, hide behind some interface for other systems
#include <stdio.h>

static EBBRC EBBMgrPrimERRMF (void *_self, EBBLTrans *lt,
			      FuncNum fnum, EBBMissArg arg) {
  printf("ERROR: unbound object invoked with self: %p, lt: %p,"
	 "fnum: %ld, arg: %ld\n", _self, lt, fnum, arg);
  return EBBRC_FAILURE;
}


static EBBTransGSys gsys;

EBBMissFunc theERRMF;
struct EBB_Trans_Mem EBB_Trans_Mem;

EBBMgrPrimRef *theEBBMgrPrimId;

void EBBMgrPrimInit () {
  EBBRC rc;
  EBBId id;

  EBB_Trans_Mem_Init();
  gsys.pages = 1;
  EBB_Trans_Mem_Alloc_Pages(gsys.pages, (u8 **)&gsys.gTable);
  theERRMF = EBBMgrPrimERRMF;
  initGTable(gsys.gTable, gsys.pages);
  initAllLTables(EBBGTransToId(gsys.gTable), gsys.pages);

  // manually binding the EBBMgrPrim in
  theEBBMgrPrimId = (EBBMgrPrimRef *)EBBGTransToId(gsys.gTable);
  EBBIdBind((EBBId)theEBBMgrPrimId, EBBMgrPrimMF, (EBBMissArg)&gsys);
  
  // do an alloc to account for manual binding 
  rc = EBBAllocPrimId(&id);

  if (!EBBRC_SUCCESS(rc)) panic();
  if ( id != (EBBId)theEBBMgrPrimId) panic();

}
