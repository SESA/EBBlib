#include "../base/include.h"
#include "../base/types.h"
#include "../base/lrtio.h"
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


#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

CObject(EBBCtrPrim);
typedef EBBCtrPrimRef *EBBCtrPrimId;

CObjectDefine(EBBCtrPrim) {
  CObjInterface(EBBCtr) *ft;
  uval v;
  EBBCtrPrimId id;
};

extern CObjInterface(EBBCtr) EBBCtrPrim_ftable;

static inline void 
EBBCtrPrimSetFT(EBBCtrPrimRef o) { o->ft = &EBBCtrPrim_ftable; }

#define PRIVATE static

// Representative Code
PRIVATE EBBRC
EBBCtrPrim_init(void *_self) 
{
  EBBCtrPrimRef self = _self;
  self->v = 0;
  return EBBRC_OK;
}

PRIVATE EBBRC 
EBBCtrPrim_inc(EBBCtrRef _self) 
{
  EBBCtrPrimRef self = (EBBCtrPrimRef)_self;
  //gcc built-in atomics
  __sync_fetch_and_add(&self->v,1);
  return EBBRC_OK;
}

PRIVATE EBBRC 
EBBCtrPrim_dec(EBBCtrRef _self) 
{
  EBBCtrPrimRef self = (EBBCtrPrimRef)_self;
  //gcc builtin atomics
  __sync_fetch_and_sub(&self->v,1);
  return EBBRC_OK;
}

PRIVATE EBBRC
EBBCtrPrim_val(EBBCtrRef _self, uval *v)
{
  EBBCtrPrimRef self = (EBBCtrPrimRef)_self;
  *v = self->v;
  return EBBRC_OK;
}

CObjInterface(EBBCtr) EBBCtrPrim_ftable = {
  .inc = EBBCtrPrim_inc, 
  .dec = EBBCtrPrim_dec, 
  .val = EBBCtrPrim_val
};

static EBBRC 
setup(EBBCtrPrimRef repRef, CObjEBBRootSharedRef rootRef, EBBCtrId *id)
{
  EBBRC rc;
  // setup function tables
  CObjEBBRootSharedSetFT(rootRef);
  EBBCtrPrimSetFT(repRef);

  // setup my representative and root
  EBBCtrPrim_init(repRef);
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
EBBCtrPrimStaticSharedCreate(EBBCtrId *id)
{
  static EBBCtrPrim theRep;
  static CObjEBBRootShared theRoot;

  // use the statically declared root and rep instances
  EBBCtrPrimRef repRef = &theRep;
  CObjEBBRootSharedRef rootRef = &theRoot;

  return setup(repRef, rootRef, id);
}


EBBRC
EBBCtrPrimSharedCreate(EBBCtrId *id)
{
  EBBCtrPrimRef repRef;
  CObjEBBRootSharedRef rootRef;

  //Allocate a root and rep via Primitive Allocator
  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);

  return setup(repRef, rootRef, id);
}

