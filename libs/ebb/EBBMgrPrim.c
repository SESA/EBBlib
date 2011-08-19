#include "../base/include.h"
#include "../base/lrtio.h"
#include "../cobj/cobj.h"
#include "sys/trans.h" //FIXME: move EBBTransLSys out of this header
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "CObjEBBRoot.h"
#include "EBBMgrPrim.h"
#include "CObjEBBUtils.h"
#include "EBBAssert.h"

// JA KLUDGE
#define MAXNODES 1024

#define panic() (*(uval *)0)

struct BootInfo {
  uval8 raw[4096];
  uval nodeId;
};

struct MessageMgr {
#if 0
  EBBFile node[MAXNODES];
  EBB9PClientId feId;
#endif
};

typedef struct EBBTransGSysStruct {
  EBBGTrans *gTable;
  uval pages;
} EBBTransGSys;

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
  //  EBBMgrPrimRef self = (EBBMgrPrimRef)_self;
  EBBIdBind(id, mf, arg);
  return EBBRC_OK;
}

static EBBRC
UnBindId (void *_self, EBBId id, EBBMissFunc *mf, EBBMissArg *arg) {
  //  EBBMgrPrimRef self = (EBBMgrPrimRef)_self;
  EBBIdUnBind(id, mf, arg);
  return EBBRC_OK;
}

#if 0
static EBBRC
EBBMgrPrim_InitMessageMgr(void *self, char *addr)
{
  struct MessageSys *msys;
  // alloc message struct

  rc = EBBPrimMalloc(sizeof(struct MessageSys), &msys, EBB_MEM_DEFAULT); 
  EBBRCAssert(rc);

  rc = EBB9PClientPrimCreate(&(msys->feId));
  EBBRCAssert(rc);

  rc = EBBCALL(msys->feId, mount, addr);
  EBBRCAssert(rc);

  self->msys = msys;
  return EBBRC_OK;
}
#endif

static EBBRC
EBBMgrPrim_MessageNode(void *self, uval nodeid, char *buf, uval len)
{
#if 0
  EBBMgrPrim *self = _self;
  EBBFile node;
  if (self->msys == NULL) return EBBRC_GENERIC_FAILURE;
  if (self->msys.node[nodeid] == NULLID) {
    // get address of node via fe (open node path)
  }
  node=self->msys.node[nodeid];
  return EBB_CALL(node, write, buf, len, *len);
#endif
  return EBBRC_OK;
}

static CObjInterface(EBBMgrPrim) EBBMgrPrim_ftable = {
  .AllocId = AllocId, 
  .FreeId = FreeId, 
  .BindId = BindId, 
  .UnBindId = UnBindId, 
  .MessageNode = EBBMgrPrim_MessageNode
};

static EBBRC EBBMgrPrimERRMF (void *_self, EBBLTrans *lt,
			      FuncNum fnum, EBBMissArg arg) {
  EBB_LRT_printf("ERROR: unbound object invoked with self: %p, lt: %p,"
		 "fnum: %ld, arg: %ld\n", _self, lt, fnum, arg);
  return EBBRC_GENERIC_FAILURE;
}

CObjInterface(EBBMgrPrimRoot) 
{
  CObjImplements(CObjEBBRoot);
  void (*init)(void *_self);
};

CObject(EBBMgrPrimRoot) 
{
  CObjInterface(EBBMgrPrimRoot) *ft;
  EBBMgrPrim reps[EBB_TRANS_MAX_ELS];
  EBBTransLSys EBBMgrPrimLTrans[EBB_TRANS_MAX_ELS];
  EBBTransGSys gsys;  
};

static uval
EBBMgrPrimRoot_handleMiss(void *_self, void *obj, EBBLTrans *lt,
				 FuncNum fnum)
{
  EBBMgrPrimRootRef self = _self;
  EBBMgrPrimRef rep;
  int numGTransPerEL;
 
  numGTransPerEL = self->gsys.pages * EBB_TRANS_PAGE_SIZE / 
    sizeof(EBBGTrans) / EBB_TRANS_MAX_ELS;

  rep = &(self->reps[EBBMyEL()]);

  rep->lsys = &(self->EBBMgrPrimLTrans[EBBMyEL()]);
  rep->lsys->gTable = &(self->gsys.gTable[numGTransPerEL * EBBMyEL()]);
  rep->lsys->lTable = EBBGTransToLTrans(rep->lsys->gTable);
  rep->lsys->free = NULL;
  rep->lsys->numAllocated = 0;
  rep->lsys->size = numGTransPerEL;
  rep->ft = &EBBMgrPrim_ftable;

  *(void **)obj = rep;
  return EBBRC_OK;
}

static void
EBBMgrPrimRoot_init(void *_self)
{
  EBBMgrPrimRootRef self = _self;

  EBB_Trans_Mem_Init();
  self->gsys.pages = 1;
  EBB_Trans_Mem_Alloc_Pages(self->gsys.pages, (uval8 **)&self->gsys.gTable);
  theERRMF = EBBMgrPrimERRMF;
  initGTable(self->gsys.gTable, self->gsys.pages);
  initAllLTables(EBBGTransToId(self->gsys.gTable), self->gsys.pages);
}


static CObjInterface(EBBMgrPrimRoot) EBBMgrPrimRoot_ftable = {
  { .handleMiss = EBBMgrPrimRoot_handleMiss },
  .init = EBBMgrPrimRoot_init
};
				     
//FIXME: have to statically allocate these because there is
//       no memory manager

#if 0
static EBBRC EBBMgrPrimMF (void *_self, EBBLTrans *lt,
		    FuncNum fnum, EBBMissArg arg) {
  EBBTransGSys gsys = *(EBBTransGSys *)arg;
  EBBMgrPrimRef ref = &EBBMgrPrimLObjs[EBBMyEL()];
  EBBMgrPrimRepInit(lt, ref, gsys);
  EBBCacheObj(lt, ref);
  *(EBBMgrPrimRef *)_self = ref;
  return EBBRC_OK;
}
#endif

// declarations of externals
EBBMgrPrimRef *theEBBMgrPrimId;

void EBBMgrPrimInit () {
  static EBBMgrPrimRoot theEBBMgrPrimRoot = { .ft = &EBBMgrPrimRoot_ftable };
  EBBRC rc;
  EBBId id;


  theEBBMgrPrimRoot.ft->init(&theEBBMgrPrimRoot);

  // manually binding the EBBMgrPrim in
  theEBBMgrPrimId = (EBBMgrPrimRef *)
    EBBGTransToId(theEBBMgrPrimRoot.gsys.gTable);

  EBBIdBind((EBBId) theEBBMgrPrimId, 
	    CObjEBBMissFunc,                
	    (EBBMissArg)&theEBBMgrPrimRoot);

  // do an alloc to account for manual binding 
  rc = EBBAllocPrimId(&id);

  EBBRCAssert(rc);
  EBBAssert(id == (EBBId)theEBBMgrPrimId);
}

// FIXME: MISC external declartions
EBBMissFunc theERRMF;
struct EBB_Trans_Mem EBB_Trans_Mem;
