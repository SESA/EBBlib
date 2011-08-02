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

#include "EBB9PClient.h"
#include "EBB9PClientPrim.h"
#include "EBBAssert.h"


CObject(EBB9PClientPrim) {
  CObjInterface(EBB9PClient) *ft;
  IxpClient *client;  
};

static EBBRC 
init(void *_self) 
{ 
  EBB9PClientPrim *self = _self;

  self->client = NULL;

  return EBBRC_OK; 
}

static EBBRC 
ismounted(void *_self) 
{
  EBB9PClientPrim *self = _self;
  return (self->client) ? EBBRC_OK : EBBRC_GENERIC_FAILURE;
}

static EBBRC 
mount(void *_self, char *address) 
{ 
  EBB9PClientPrim *self = _self;

  self->client = ixp_mount(address);

  return ismounted(self);
}

static EBBRC 
unmount(void *_self) 
{ 
  return EBBRC_OK; 
}

static EBBRC
open(void *_self, char *path, uval8 mode, IxpCFid **fd) 
{ 
  EBB9PClientPrim *self = _self;

  if (!EBBRC_SUCCESS(ismounted(self))) return EBBRC_GENERIC_FAILURE;
  
  *fd = ixp_open(self->client, path, mode);

  if (*fd == NULL)
    EBB_LRT_printf("Can't open file '%s': %s\n", path, ixp_errbuf());
  
  return (*fd != NULL) ? EBBRC_OK : EBBRC_GENERIC_FAILURE;
}

static EBBRC 
close(void *_self, IxpCFid *fd, int *rc) 
{ 
  return EBBRC_OK; 
}

static EBBRC 
read(void *_self, IxpCFid *fd, void *buf, sval cnt, sval *n)
{ 
  EBB9PClientPrim *self = _self;


  if (!EBBRC_SUCCESS(ismounted(self)) || fd == NULL || cnt < 0)  {
    *n = 0;
    return EBBRC_GENERIC_FAILURE;
  }
  
  *n = ixp_read(fd, buf, cnt);

  if (*n==-1)
    EBB_LRT_printf("cannot read file/directory '%p': %s\n", fd, ixp_errbuf());
  return (*n != -1) ? EBBRC_OK : EBBRC_GENERIC_FAILURE;
}

static EBBRC
write(void *_self, IxpCFid *fd, const void *buf, sval cnt,sval *n)
{ 
  return EBBRC_OK; 
}

CObjInterface(EBB9PClient) EBB9PClientPrim_ftable = {
  init, mount, unmount, ismounted, open, close, read, write
};

static inline void 
EBB9PClientPrimSetFT(EBB9PClientPrimRef o) 
{ 
  o->ft = &EBB9PClientPrim_ftable; 
}

EBBRC
EBB9PClientPrimCreate(EBB9PClientId *id) 
{
  EBBRC rc;

  EBB9PClientPrimRef repRef;
  CObjEBBRootSharedRef rootRef;

  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  
  CObjEBBRootSharedSetFT(rootRef);
  EBB9PClientPrimSetFT(repRef);
  
  repRef->ft->init(repRef);
  rootRef->ft->init(rootRef, repRef);
  
  rc = EBBAllocPrimId(id);
  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  EBBRCAssert(rc);

  return EBBRC_OK;
}
