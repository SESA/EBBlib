#include "../base/include.h"
#include "../base/types.h"
#include "../cobj/cobj.h"
#include "sys/trans.h" //FIXME: move EBBTransLSys out of this header
#include "CObjEBB.h"
#include "EBBTypes.h"
#include "MsgMgr.h"
#include "EBBMgrPrim.h"
#include "EBBMemMgr.h"
#include "EBBMemMgrPrim.h"
#include "CObjEBBUtils.h"
#include "CObjEBBRoot.h"
#include "CObjEBBRootMulti.h"
#include "EBBCtr.h"
#include "EBBCtrPrimDistributed.h"

#include "stdio.h"

#include "ebbtest.h"

static EBBRC
init(void *_self)
{
  EBBCtrPrimDistributedRef self = _self;
  self->localValue = 0;
  return EBBRC_OK;
}

static EBBRC 
inc(void *_self) 
{
  EBBCtrPrimDistributedRef self = _self;
  //gcc built-in atomics
  __sync_fetch_and_add(&self->localValue,1);
  return EBBRC_OK;
}

static EBBRC 
dec(void *_self) 
{
  EBBCtrPrimDistributedRef self = _self;
  //gcc builtin atomics
  __sync_fetch_and_sub(&self->localValue,1);
  return EBBRC_OK;
}

static EBBRC
val(void *_self, uval *v)
{
  EBBCtrPrimDistributedRef self = _self;
  uval val = 0;
  RepListNode *node;
  EBBCtrPrimDistributedRef rep = NULL;
  for (node = self->theRoot->ft->nextRep(self->theRoot, 0, &rep);
       node; node = self->theRoot->ft->nextRep(self->theRoot, node, &rep)) {
    val += rep->localValue;
  }
  *v = val;
  return EBBRC_OK;
}

static void *createRep(CObjEBBRootMultiRef rootRef) {
  EBBCtrPrimDistributedRef repRef;
  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  EBBCtrPrimDistributedSetFT(repRef);
  repRef->theRoot = rootRef;
  repRef->ft->init(repRef);
  return repRef;
}

CObjInterface(EBBCtr) EBBCtrPrimDistributed_ftable = {
  init, inc, dec, val
};

EBBRC
EBBCtrPrimDistributedCreate(EBBCtrPrimDistributedId *id)
{
  EBBRC rc;
  CObjEBBRootMultiRef rootRef;
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  CObjEBBRootMultiSetFT(rootRef);
  rootRef->ft->init(rootRef, createRep);

  rc = EBBAllocLocalPrimId(id);
  //  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef);
  //  EBBRCAssert(rc);

  return EBBRC_OK;
}
