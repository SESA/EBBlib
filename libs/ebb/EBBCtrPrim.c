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
#include "CObjEBBRootShared.h"
#include "EBBCtr.h"
#include "EBBCtrPrim.h"
#include "EBBAssert.h"

#include "stdio.h"

#include "ebbtest.h"

#ifdef EBB_TEST
#define PRIVATE
#else
#define PRIVATE static
#endif

// Representative Code
PRIVATE EBBRC
init(void *_self) 
{
  EBBCtrPrimRef self = _self;
  self->v = 0;
  return EBBRC_OK;
}

PRIVATE EBBRC 
inc(void *_self) 
{
  EBBCtrPrimRef self = _self;
  //gcc built-in atomics
  __sync_fetch_and_add(&self->v,1);
  return EBBRC_OK;
}

PRIVATE EBBRC 
dec(void *_self) 
{
  EBBCtrPrimRef self = _self;
  //gcc builtin atomics
  __sync_fetch_and_sub(&self->v,1);
  return EBBRC_OK;
}

PRIVATE EBBRC
val(void *_self, uval *v)
{
  EBBCtrPrimRef self = _self;
  *v = self->v;
  return EBBRC_OK;
}

CObjInterface(EBBCtr) EBBCtrPrim_ftable = {
  init, inc, dec, val
};

static EBBRC 
setup(EBBCtrPrimRef repRef, CObjEBBRootSharedRef rootRef, EBBCtrPrimId *id)
{
  EBBRC rc;
  // setup function tables
  CObjEBBRootSharedSetFT(rootRef);
  EBBCtrPrimSetFT(repRef);

  // setup my representative and root
  repRef->ft->init(repRef);
  // shared root knows about only one rep so we 
  // pass it along for it's init
  rootRef->ft->init(rootRef, repRef);

  rc = EBBAllocPrimId(id);
  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  EBBRCAssert(rc);

  return rc;
}

// Statically declared root and rep... this does
// not therefore account for memory locality 
EBBRC
EBBCtrPrimStaticSharedCreate(EBBCtrPrimId *id)
{
  static EBBCtrPrim theRep;
  static CObjEBBRootShared theRoot;

  // use the statically declared root and rep instances
  EBBCtrPrimRef repRef = &theRep;
  CObjEBBRootSharedRef rootRef = &theRoot;

  return setup(repRef, rootRef, id);
}


EBBRC
EBBCtrPrimSharedCreate(EBBCtrPrimId *id)
{
  EBBCtrPrimRef repRef;
  CObjEBBRootSharedRef rootRef;

  //Allocate a root and rep via Primitive Allocator
  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);

  return setup(repRef, rootRef, id);
}
