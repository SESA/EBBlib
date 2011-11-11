#include <config.h>
#include <types.h>
#include <lrt/io.h>
#include <core/cobj/cobj.h>
#include <core/sys/trans.h> //FIXME: move EBBTransLSys out of this header
#include <core/cobj/CObjEBB.h>
#include <core/types.h>
#include <lrt/assert.h>
#include <core/EBBMgrPrim.h>
#include <core/MemMgr.h>
#include <core/MemMgrPrim.h>
#include <core/EventMgrPrim.h>
#include <core/EventMgrPrimImp.h>
#include <core/cobj/CObjEBBUtils.h>
#include <core/cobj/CObjEBBRoot.h>
#include <core/cobj/CObjEBBRootShared.h>

#include <net/EthTypeMgr.h>
#include <net/EthMgr.h>
#include <net/EthMgrPrim.h>

#include <core/lrt/pic.h>
#include <net/lrt/ethlib.h>
#include <strings.h>

#define VERBOSE_PR(...) ( EBB_LRT_printf(__VA_ARGS__) )

#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

#define NUMETHTYPES (1<<(sizeof(uval16) * 8))

CObject(EvHdlr) {
  CObjInterface(EventHandler) *ft;
}; 

CObject(EthMgrPrim) {
  CObjInterface(EthMgr) *ft;
  EthTypeMgrId typeMgrs[NUMETHTYPES];
  CObjectDefine(EvHdlr) evHdlr;
  EventHandlerId hdlrId;
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
  o->evHdlr.ft = &(EthMgrPrim_ftable.EventHandler_if);
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
  
  rc = EBBAllocPrimId(id);
  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  EBBRCAssert(rc);

  // setup the EthMgr on a second id that services the EventHander Interface
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  CObjEBBRootSharedSetFT(rootRef);
  rootRef->ft->init(rootRef, &(repRef->evHdlr));
  rc = EBBAllocPrimId(&(repRef->hdlrId));
  EBBRCAssert(rc);
  rc = CObjEBBBind(repRef->hdlrId, rootRef);
  EBBRCAssert(rc);
 
  rc = EBBCALL(theEventMgrPrimId, allocEventNo, &(repRef->ev));
  EBBRCAssert(rc);

  rc = ethlib_nic_init("eth1", &nicisrc);
  EBBRCAssert(rc);

  rc = EBBCALL(theEventMgrPrimId, registerHandler, repRef->ev, 
	       repRef->hdlrId, nicisrc);
  EBBRCAssert(rc);
  
  return EBBRC_OK;
}
