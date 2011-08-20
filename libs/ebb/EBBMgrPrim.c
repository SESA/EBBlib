#include "../base/include.h"
#include "../base/lrtio.h"
#include "../cobj/cobj.h"
#include "sys/trans.h" //FIXME: move EBBTransLSys out of this header
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "CObjEBBRoot.h"
#include "MsgMgr.h"
#include "EBBMgrPrim.h"
#include "CObjEBBUtils.h"
#include "EBBAssert.h"
#include "EBB9PClient.h"
#include "EBB9PClientPrim.h"
#include "EBBFile.h"
#include "EBB9PFilePrim.h"
#include "BootInfo.h"

#include __LRTINC(misc.h)

#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

// JA KLUDGE
#define MAXNODES 1024

#define STRLEN 160

#define panic() (*(uval *)0)


typedef struct EBBTransGSysStruct {
  EBBGTrans *gTable;
  uval pages;
} EBBTransGSys;

extern uval EBBNodeId;

static uval MsgAvailable;

struct MessageMgr {
  EBB9PClientId fe;
  char nodespath[STRLEN];
  uval nodespathlen;
  EBBFileId p9addr;
  EBBFileId node[MAXNODES];
};

CObjInterface(EBBMgrPrimRoot) 
{
  CObjImplements(CObjEBBRoot);
  void (*init)(void *_self);
  void (*initMsgMgr)(void *_self, EBBId id, char *nodespath, 
		     uval nodespathlen);
  EBBRC (*MsgNode) (void *_self, uval nodeid, MsgHandler h, 
		    uval a0, uval a1, uval a2, uval a3, 
		    uval a4, uval a5, uval a6, uval a7, uval *rcode); 
};

CObject(EBBMgrPrimRoot) 
{
  CObjInterface(EBBMgrPrimRoot) *ft;
  EBBMgrPrim reps[EBB_TRANS_MAX_ELS];
  EBBTransLSys EBBMgrPrimLTrans[EBB_TRANS_MAX_ELS];
  EBBTransGSys gsys;  
  struct BootInfo binfo;
  struct MessageMgr mmgr;
};

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

static  EBBRC
EBBMgrPrim_InitMessageMgr(void *_self, EBBId id, char *nodespath, uval pathlen) 
{
  EBBMgrPrimRef self = _self;
  EBBMgrPrimRootRef root = self->myRoot;

  root->ft->initMsgMgr(root, id, nodespath, pathlen);
  return EBBRC_OK;
}

static EBBRC
EBBMgrPrim_MessageNode(void *_self, uval nodeid, MsgHandler h, uval a0, uval a1, uval a2, 
		       uval a3, uval a4, uval a5, uval a6, uval a7, uval *rc)
{
  EBBMgrPrimRef self = _self;
  EBBMgrPrimRootRef root = self->myRoot;

  return root->ft->MsgNode(root, nodeid, h, a0, a1, a2, a3, a4, a5, a6, a7, rc);
}

static EBBRC
EBBMgrPrim_HandleMsg(void *_self, void *data, uval len,
		     struct MsgMgrReply *reply)
{
  struct MsgMgrMsg *msg = data;
  
  EBBAssert(len == sizeof(struct MsgMgrMsg));
  
  return msg->h(msg->a0, msg->a1, msg->a2, msg->a3, 
		msg->a4, msg->a5, msg->a6, msg->a7, 
		&(reply->rc));
}

static CObjInterface(EBBMgrPrim) EBBMgrPrim_ftable = {
  .AllocLocalId = AllocLocalId, 
  .AllocGlobalId = AllocGlobalId,
  .FreeId = FreeId, 
  .BindId = BindId, 
  .BindGlobalId = BindGlobalId,
  .UnBindId = UnBindId, 
  .MessageNode = EBBMgrPrim_MessageNode,
  .InitMessageMgr = EBBMgrPrim_InitMessageMgr,
  .HandleMsg = EBBMgrPrim_HandleMsg
};

static EBBRC EBBMgrPrimERRMF (void *_self, EBBLTrans *lt,
			      FuncNum fnum, EBBMissArg arg) {
  if (isLocalEBB(EBBLTransToGTrans(lt))) {
    EBB_LRT_printf("ERROR: gtable miss on a local-only EBB\n");
  } else if (!MsgAvailable) {
    EBB_LRT_printf("ERROR: gtable miss on a global EBB but not connected!\n");
  } else if (getLTransNodeId(lt) == EBBNodeId) {
    EBB_LRT_printf("ERROR: gtable miss on a global EBB but we are the home node!\n");
  } else {
    EBB_LRT_printf("gtable miss on a global EBB: unimplemented\n");
    //call the home node, which should return an EBBMissFunc to handle this call
  }

  return EBBRC_GENERIC_FAILURE;
}

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
  rep->lsys->localGTable = &(self->gsys.gTable[numGTransPerEL * EBBMyEL()]);
  rep->lsys->localLTable = EBBGTransToLTrans(rep->lsys->localGTable);
  rep->lsys->localFree = NULL;
  rep->lsys->localNumAllocated = 0;
  rep->lsys->localSize = numGTransPerEL;
  rep->lsys->globalGTable = NULL;
  rep->lsys->globalLTable = NULL;
  rep->lsys->globalFree = NULL;
  rep->lsys->globalNumAllocated = 0;
  rep->lsys->globalSize = 0;
  rep->myRoot = self;
  rep->ft = &EBBMgrPrim_ftable;
  
  *(void **)obj = rep;
  return EBBRC_OK;
}



static void
EBBMgrPrimRoot_init(void *_self)
{
  EBBMgrPrimRootRef self = _self;

  EBB_Trans_Mem_Init();
  self->gsys.pages = EBB_TRANS_NUM_PAGES;
  EBB_Trans_Mem_Alloc_Pages(self->gsys.pages, (uval8 **)&self->gsys.gTable);
  theERRMF = EBBMgrPrimERRMF;
  initGTable(self->gsys.gTable, self->gsys.pages);
  initAllLTables(EBBGTransToId(self->gsys.gTable), self->gsys.pages);
  bzero(&(self->binfo), sizeof(self->binfo));
  bzero(&(self->mmgr), sizeof(self->mmgr));
  MsgAvailable = 0;
}

static void
EBBMgrPrimRoot_initMsgMgr(void *_self, EBBId id, char *nodespath, uval pathlen) 
{
  EBBMgrPrimRootRef self = _self;
  if (MsgAvailable) return;
  self->mmgr.fe = (EBB9PClientId) id;
  self->mmgr.nodespathlen = (pathlen < STRLEN) ? pathlen : STRLEN;
  // JA KLUDGE --- I am getting lazy
  memcpy(self->mmgr.nodespath, nodespath, self->mmgr.nodespathlen);
  MsgAvailable = 1;
}

static EBBRC 
EBBMgrPrimRoot_MsgNode(void *_self, uval nodeid, MsgHandler h,
		       uval a0, uval a1, uval a2, 
		       uval a3, uval a4, uval a5, uval a6, uval a7, 
		       uval *rcode)
{
  EBBMgrPrimRootRef self = _self;
  EBB9PClientId feId = self->mmgr.fe;
  EBB9PClientId node;
  EBBRC rc;
  char tmp[STRLEN];
  sval n;
  struct MsgMgrMsg msg;
  struct MsgMgrReply reply;

  if (MsgAvailable == 0 ||  feId == 0 || nodeid > MAXNODES) return EBBRC_GENERIC_FAILURE;

  if (self->mmgr.node[nodeid] == 0) {
    // TRANSLATE nodeid into a "physical id" in this case a 9p id
    EBB_LRT_printf("%s: nodespathlen=%ld nodespath=%s\n" , 
		   __func__, self->mmgr.nodespathlen, self->mmgr.nodespath);
    if (self->mmgr.p9addr==0) EBB9PFilePrimCreate(feId, &(self->mmgr.p9addr));
    // JA KLUDGE GETTING REALLY LAZY
    snprintf(tmp, STRLEN, "%s/%ld/p9addr", self->mmgr.nodespath, nodeid);
    EBB_LRT_printf("%s: target node path: %s\n", __func__, tmp);
    rc = EBBCALL(self->mmgr.p9addr, open, tmp, EBBFILE_OREAD, 0);
    if (!EBBRC_SUCCESS(rc)) {
	// JA FIXME: LEAK OBJECTS
	return rc;
    }
    rc = EBBCALL(self->mmgr.p9addr, read, tmp, STRLEN, &n);
    if (!EBBRC_SUCCESS(rc)) {
	// JA FIXME: LEAK OBJECTS
	return rc;
    }
    if (n<=STRLEN) tmp[n]=0;
    EBBCALL(self->mmgr.p9addr, close, &n);
    EBB_LRT_printf("%s: target node %ld 9p address: %s\n", __func__, nodeid, 
		   tmp);
    EBB9PClientPrimCreate(&node);
    rc = EBBCALL(node, mount, tmp);
    if (!EBBRC_SUCCESS(rc)) {
	// JA FIXME: LEAK OBJECTS
      return rc;
    }
    EBB9PFilePrimCreate(node,&(self->mmgr.node[nodeid]));
    rc = EBBCALL(self->mmgr.node[nodeid], open, "/msg", EBBFILE_ORDWR, 0);
    if (!EBBRC_SUCCESS(rc)) {
	// JA FIXME: LEAK OBJECTS
	return rc;
    }
  }

  // marshall
  msg.h = h;
  msg.a0 = a0; msg.a1 = a1; msg.a2 = a2; msg.a3 = a3;  
  msg.a4 = a4; msg.a5 = a5; msg.a6 = a6; msg.a7 = a7;

  rc = EBBCALL(self->mmgr.node[nodeid], write, &msg, sizeof(msg), &n);

  EBBRCAssert(rc);
  EBBAssert(n == sizeof(msg));

  rc = EBBCALL(self->mmgr.node[nodeid], pread, &reply, sizeof(reply), 0, &n);

  EBBRCAssert(rc);
  EBBAssert(n == sizeof(reply));

  *rcode = reply.rc;

  return rc;
}

static CObjInterface(EBBMgrPrimRoot) EBBMgrPrimRoot_ftable = {
  { .handleMiss = EBBMgrPrimRoot_handleMiss },
  .init = EBBMgrPrimRoot_init,
  .initMsgMgr = EBBMgrPrimRoot_initMsgMgr,
  .MsgNode = EBBMgrPrimRoot_MsgNode
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

void EBBMgrPrimInit() {
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
  rc = EBBAllocLocalPrimId(&id);

  EBBRCAssert(rc);
  EBBAssert(id == (EBBId)theEBBMgrPrimId);
}

// FIXME: MISC external declartions
EBBMissFunc theERRMF;
struct EBB_Trans_Mem EBB_Trans_Mem;
