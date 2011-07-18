#include "../base/types.h"
#include "../cobj/cobj.h"
#include "sys/trans.h" //FIXME: move EBBTransLSys out of this header
#include "CObjEBB.h"
#include "EBBTypes.h"
#include "EBBMgrPrim.h"
#include "CObjEBBUtils.h"
#include "CObjEBBRoot.h"
#include "CObjEBBRootShared.h"
#include "EBBCtr.h"
#include "EBBCtrPrim.h"

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
  //  add atomics here
  self->v++;
  return EBBRC_OK;
}

PRIVATE EBBRC 
dec(void *_self) 
{
  EBBCtrPrimRef self = _self;
  //  add atomics here
  self->v--;
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


EBBRC
EBBCtrPrimSharedCreate(EBBCtrPrimId *id)
{
  EBBRC rc;
  static EBBCtrPrim theRep;
  static CObjEBBRootShared theRoot;
  EBBCtrPrimRef repRef = &theRep;
  CObjEBBRootSharedRef rootRef = &theRoot;

  // setup function tables
  CObjEBBRootSharedSetFT(rootRef);
  EBBCtrPrimSetFT(repRef);

  // setup my representative and root
  repRef->ft->init(repRef);
  // shared root knows about only one rep so we 
  // pass it along for it's init
  rootRef->ft->init(rootRef, &theRep);

  rc = EBBAllocPrimId(id);
  //  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  //  EBBRCAssert(rc);

  return EBBRC_OK;
}
