#include "../base/include.h"
#include "../base/types.h"
#include "../base/lrtio.h"
#include "../cobj/cobj.h"
#include "sys/trans.h" //FIXME: move EBBTransLSys out of this header
#include "CObjEBB.h"
#include "EBBTypes.h"
#include "EBBAssert.h"
#include "EBBMgrPrim.h"
#include "EBBMemMgr.h"
#include "EBBMemMgrPrim.h"
#include "EBBEventMgrPrim.h"
#include "EBBEventMgrPrimImp.h"
#include "CObjEBBUtils.h"
#include "CObjEBBRoot.h"
#include "CObjEBBRootShared.h"

#include "EthTypeMgr.h"
#include "EthMgr.h"
#include "EthMgrPrim.h"

#include __LRTINC(misc.h)
#include __LRTINC(pic.h)
#include __LRTINC(ethlib.h)

#define VERBOSE_PR(...) ( EBB_LRT_printf(__VA_ARGS__) )

#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

#define NUMETHTYPES (1<<(sizeof(uval16) * 8))

CObject(EvHdlr) {
  CObjInterface(EBBEventHandler) *ft;
}; 

CObject(EthMgrPrim) {
  CObjInterface(EthMgr) *ft;
  EthTypeMgrId typeMgrs[NUMETHTYPES];
  CObjectDefine(EvHdlr) evHdlr;
  EBBEventHandlerId hdlrId;
  uval ev;
  uval rcnt;
};


static EBBRC
EthMgrPrim_init(void *_self)
{
  return EBBRC_GENERIC_FAILURE;
}

static EBBRC
EthMgrPrim_bind(void *_self, uval16 type, EthTypeMgrId id)
{
  return EBBRC_GENERIC_FAILURE;
}

#define ContainingCOPtr(addr, ctype, field) \
  ((EthMgrPrim *)(((uval)addr) - (__builtin_offsetof(ctype, field))))
  
static EBBRC 
EthMgrPrim_handleEvent(void *_self)
{
  EthMgrPrim *self = ContainingCOPtr(_self,EthMgrPrim,evHdlr);
  ethlib_nic_readpkt();
  self->rcnt++;
  return EBBRC_OK;
}
  
CObjInterface(EthMgr) EthMgrPrim_ftable = {
  .init = EthMgrPrim_init,
  .bind = EthMgrPrim_bind,
  {
    .handleEvent = EthMgrPrim_handleEvent
  }
};

static inline void 
EthMgrPrimSetFT(EthMgrPrimRef o) 
{ 
  o->ft = &EthMgrPrim_ftable; 
  o->evHdlr.ft = &(EthMgrPrim_ftable.EBBEventHandler_if);
}


EBBRC
EthMgrPrimCreate(EthMgrId *id) 
{
  EBBRC rc;
  EthMgrPrimRef repRef;
  CObjEBBRootSharedRef rootRef;
  lrt_pic_src nicisrc;

  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  
  CObjEBBRootSharedSetFT(rootRef);
  EthMgrPrimSetFT(repRef);

  bzero(repRef->typeMgrs, sizeof(repRef->typeMgrs));
  repRef->rcnt=0;

  rootRef->ft->init(rootRef, repRef);
  
  rc = EBBAllocLocalPrimId(id);
  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  EBBRCAssert(rc);

  // setup the EthMgr on a second id that services the EventHander Interface
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  CObjEBBRootSharedSetFT(rootRef);
  rootRef->ft->init(rootRef, &(repRef->evHdlr));
  rc = EBBAllocLocalPrimId(&(repRef->hdlrId));
  EBBRCAssert(rc);
  rc = CObjEBBBind(repRef->hdlrId, rootRef);
  EBBRCAssert(rc);
 
  rc = EBBCALL(theEBBEventMgrPrimId, allocEventNo, &(repRef->ev));
  EBBRCAssert(rc);

  rc = ethlib_nic_init("eth1", &nicisrc);
  EBBRCAssert(rc);

  rc = EBBCALL(theEBBEventMgrPrimId, registerHandler, repRef->ev, 
	       repRef->hdlrId, nicisrc);
  EBBRCAssert(rc);
  
  return EBBRC_OK;
}
