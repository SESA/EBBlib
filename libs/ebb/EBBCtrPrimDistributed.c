#include "../base/include.h"
#include "../base/types.h"
#include "../cobj/cobj.h"
#include "sys/trans.h" //FIXME: move EBBTransLSys out of this header
#include "CObjEBB.h"
#include "EBBTypes.h"
#include "EBBMgrPrim.h"
#include "EBBMemMgr.h"
#include "EBBMemMgrPrim.h"
#include "CObjEBBUtils.h"
#include "CObjEBBRoot.h"
#include "CObjEBBRootMulti.h"
#include "EBBCtr.h"
#include "EBBCtrPrimDistributed.h"
#include "CObjEBBRootMulti.h"

CObject(EBBCtrPrimDistributed) {
  CObjInterface(EBBCtr) *ft;
  uval localValue;
  CObjEBBRootMultiRef theRoot;
};

extern CObjInterface(EBBCtr) EBBCtrPrimDistributed_ftable;

static inline void
EBBCtrPrimDistributedSetFT(EBBCtrPrimDistributedRef o)
{
  o->ft = &EBBCtrPrimDistributed_ftable;
}

typedef EBBCtrPrimDistributedRef *EBBCtrPrimDistributedId;

static EBBRC 
EBBCtrPrimDistributed_inc(EBBCtrRef _self) 
{
  EBBCtrPrimDistributedRef self = (EBBCtrPrimDistributedRef)_self;
  //gcc built-in atomics
  __sync_fetch_and_add(&self->localValue,1);
  return EBBRC_OK;
}

static EBBRC 
EBBCtrPrimDistributed_dec(EBBCtrRef _self) 
{
  EBBCtrPrimDistributedRef self = (EBBCtrPrimDistributedRef)_self;
  //gcc builtin atomics
  __sync_fetch_and_sub(&self->localValue,1);
  return EBBRC_OK;
}

static EBBRC
EBBCtrPrimDistributed_val(EBBCtrRef _self, uval *v)
{
  EBBCtrPrimDistributedRef self = (EBBCtrPrimDistributedRef)_self;
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

static void *
EBBCtrPrimDistributed_createRep(CObjEBBRootMultiRef rootRef) {
  EBBCtrPrimDistributedRef repRef;
  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  EBBCtrPrimDistributedSetFT(repRef);
  repRef->theRoot = rootRef;
  repRef->localValue = 0;
  return repRef;
}

CObjInterface(EBBCtr) EBBCtrPrimDistributed_ftable = {
  .inc = EBBCtrPrimDistributed_inc, 
  .dec = EBBCtrPrimDistributed_dec, 
  .val = EBBCtrPrimDistributed_val
};

EBBRC
EBBCtrPrimDistributedCreate(EBBCtrId *id)
{
  EBBRC rc;
  CObjEBBRootMultiRef rootRef;
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  CObjEBBRootMultiSetFT(rootRef);
  rootRef->ft->init(rootRef, EBBCtrPrimDistributed_createRep);

  rc = EBBAllocLocalPrimId(id);
  //  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef);
  //  EBBRCAssert(rc);

  return EBBRC_OK;
}
