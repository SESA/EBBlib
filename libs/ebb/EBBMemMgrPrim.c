#define MEMSIZE (1 << 20)

static u8 theMemory[MEMSIZE]

static EBBRC
init(void *_self)
{
  EBBMemMgrPrimRef self = _self;
  self->mem = theMemory;
  len = MEMSIZE;
  return EBBRC_OK;
}

//just grab from the beginning of the memory and move
//the pointer forward until we run out
static EBBRC
alloc(void *_self, uval size, void **mem)
{
  EBBMemMgrPrimRef self = _self;
  if (size > self->len) {
    *mem = NULL; //Do I return some error code here??
  } else {
    *mem = self->mem;
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

EBBRC
EBBMemMgrPrimShared(EBBMemMgrPrimId *id)
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

  rc = EBBAllocPrimId(id);
  //  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  //  EBBRCAssert(rc);

  return EBBRC_OK;
}
