#include "../base/include.h"
#include "../base/types.h"
#include "../cobj/cobj.h"
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "EBBMgrPrim.h"
#include "CObjEBBUtils.h"
#include "CObjEBBRoot.h"
#include "CObjEBBRootShared.h"
#include "EBBMemMgr.h"
#include "EBBMemMgrPrim.h"

#define MEMSIZE (1 << 20)

static uval8 theMemory[MEMSIZE];

static EBBRC
init(void *_self)
{
  EBBMemMgrPrimRef self = _self;
  self->mem = theMemory;
  self->len = MEMSIZE;
  return EBBRC_OK;
}

//just grab from the beginning of the memory and move
//the pointer forward until we run out
static EBBRC
alloc(void *_self, uval size, void *mem, EBB_MEM_POOL pool)
{
  EBBMemMgrPrimRef self = _self;
  if (size > self->len) {
    *((void **)mem) = NULL; //Do I return some error code here??
  } else {
    *((void **)mem) = self->mem;
    self->mem += size;
    self->len -= size;
  }
  return EBBRC_OK;
}

//freeing is a nop in this implementation
static EBBRC
free(void *_self, void *mem) {
  return EBBRC_OK;
}

CObjInterface(EBBMemMgr) EBBMemMgrPrim_ftable = {
  init, alloc, free
};

EBBMemMgrPrimRef *theEBBMemMgrPrimId;

EBBRC
EBBMemMgrPrimInit()
{
  EBBRC rc;
  static EBBMemMgrPrim theRep;
  static CObjEBBRootShared theRoot;
  EBBMemMgrPrimRef repRef = &theRep;
  CObjEBBRootSharedRef rootRef = &theRoot;

  // setup function tables
  CObjEBBRootSharedSetFT(rootRef);
  EBBMemMgrPrimSetFT(repRef);

  // setup my representative and root
  repRef->ft->init(repRef);
  // shared root knows about only one rep so we 
  // pass it along for it's init
  rootRef->ft->init(rootRef, &theRep);

  rc = EBBAllocLocalPrimId(&theEBBMemMgrPrimId);
  //  EBBRCAssert(rc);

  rc = CObjEBBBind(theEBBMemMgrPrimId, rootRef); 
  //  EBBRCAssert(rc);

  return EBBRC_OK;
}
