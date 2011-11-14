#include <config.h>
#include <types.h>
#include <l0/cobj/cobj.h>
#include <l0/sys/trans.h> //FIXME: move EBBTransLSys out of this header
#include <l0/cobj/CObjEBB.h>
#include <l0/types.h>
#include <l0/EBBMgrPrim.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <misc/Ctr.h>
#include <misc/CtrPrimDistributed.h>

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

  rc = EBBAllocPrimId(id);
  //  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef);
  //  EBBRCAssert(rc);

  return EBBRC_OK;
}
