#include <config.h>
#include <types.h>
#include <core/cobj/cobj.h>
#include <core/types.h>
#include <core/cobj/CObjEBB.h>
#include <core/EBBMgrPrim.h>
#include <core/cobj/CObjEBBUtils.h>
#include <core/cobj/CObjEBBRoot.h>
#include <core/cobj/CObjEBBRootShared.h>
#include <core/MemMgr.h>
#include <core/MemMgrPrim.h>

#define MEMSIZE (1 << 20)

static uval8 theMemory[MEMSIZE];

static EBBRC
EBBMemMgrPrim_init(void *_self)
{
  EBBMemMgrPrimRef self = _self;
  self->mem = theMemory;
  self->len = MEMSIZE;
  return EBBRC_OK;
}

//just grab from the beginning of the memory and move
//the pointer forward until we run out
static EBBRC
EBBMemMgrPrim_alloc(void *_self, uval size, void *mem, EBB_MEM_POOL pool)
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
EBBMemMgrPrim_free(void *_self, void *mem) {
  return EBBRC_OK;
}

CObjInterface(EBBMemMgr) EBBMemMgrPrim_ftable = {
  .init = EBBMemMgrPrim_init, 
  .alloc = EBBMemMgrPrim_alloc, 
  .free = EBBMemMgrPrim_free
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

  rc = EBBAllocPrimId(&theEBBMemMgrPrimId);
  //  EBBRCAssert(rc);

  rc = CObjEBBBind(theEBBMemMgrPrimId, rootRef); 
  //  EBBRCAssert(rc);

  return EBBRC_OK;
}
