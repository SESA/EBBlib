#include "../base/include.h"
#include "../base/types.h"
#include "../base/lrtio.h"
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
#include "CObjEBBRootShared.h"

#include "EBB9PClient.h"
#include "EBB9PClientPrim.h"
#include "EBBAssert.h"

CObject(EBB9PClientPrim) {
  CObjInterface(EBB9PClient) *ft;
  IxpClient *client;  
};

static EBBRC 
EBB9PClientPrim_init(void *_self) 
{ 
  EBB9PClientPrim *self = _self;

  self->client = NULL;

  return EBBRC_OK; 
}

static EBBRC 
EBB9PClientPrim_ismounted(void *_self) 
{
  EBB9PClientPrim *self = _self;
  return (self->client) ? EBBRC_OK : EBBRC_GENERIC_FAILURE;
}

static EBBRC 
EBB9PClientPrim_mount(void *_self, char *address) 
{ 
  EBB9PClientPrim *self = _self;

  self->client = ixp_mount(address);

  return EBB9PClientPrim_ismounted(self);
}

static EBBRC 
EBB9PClientPrim_unmount(void *_self) 
{ 
  EBB9PClientPrim *self = _self;

  ixp_unmount(self->client);
  return EBBRC_OK; 
}

static EBBRC
EBB9PClientPrim_open(void *_self, char *path, uval8 mode, IxpCFid **fd) 
{ 
  EBB9PClientPrim *self = _self;

  if (!EBBRC_SUCCESS(EBB9PClientPrim_ismounted(self))) 
    return EBBRC_GENERIC_FAILURE;
  
  *fd = ixp_open(self->client, path, mode);

  if (*fd == NULL)
    EBB_LRT_printf("Can't open file '%s': %s\n", path, ixp_errbuf());
  
  return (*fd != NULL) ? EBBRC_OK : EBBRC_GENERIC_FAILURE;
}


static EBBRC
EBB9PClientPrim_create(void *_self, char *path, uint perm, uval8 mode, IxpCFid **fd) 
{ 
  EBB9PClientPrim *self = _self;

  if (!EBBRC_SUCCESS(EBB9PClientPrim_ismounted(self)))
    return EBBRC_GENERIC_FAILURE;
  
  *fd = ixp_create(self->client, path, perm, mode);

  if (*fd == NULL)
    EBB_LRT_printf("Can't create file '%s': %s\n", path, ixp_errbuf());
  
  return (*fd != NULL) ? EBBRC_OK : EBBRC_GENERIC_FAILURE;
}

static EBBRC 
EBB9PClientPrim_close(void *_self, IxpCFid *fd, int *rc) 
{ 
  *rc = ixp_close(fd);
  EBBAssert(*rc == 1);

  return EBBRC_OK; 
}

static EBBRC 
EBB9PClientPrim_read(void *_self, IxpCFid *fd, void *buf, sval cnt, sval *n)
{ 
  EBB9PClientPrim *self = _self;

  if (!EBBRC_SUCCESS(EBB9PClientPrim_ismounted(self)) || 
      fd == NULL || cnt < 0)  {
    *n = 0;
    return EBBRC_GENERIC_FAILURE;
  }
  
  *n = ixp_read(fd, buf, cnt);

  if (*n==-1)
    EBB_LRT_printf("cannot read file/directory '%p': %s\n", fd, ixp_errbuf());
  return (*n != -1) ? EBBRC_OK : EBBRC_GENERIC_FAILURE;
}

static EBBRC
EBB9PClientPrim_write(void *_self, IxpCFid *fd, const void *buf, sval cnt,sval *n)
{ 
  EBB9PClientPrim *self = _self;

  if (!EBBRC_SUCCESS(EBB9PClientPrim_ismounted(self)) || 
      fd == NULL || cnt < 0)  {
    *n = 0;
    return EBBRC_GENERIC_FAILURE;
  }

  *n = ixp_write(fd, buf, cnt);

  if (*n != cnt)
    EBB_LRT_printf("cannot write file/directory '%p': %s\n", fd, ixp_errbuf());
  return (*n == cnt) ? EBBRC_OK : EBBRC_GENERIC_FAILURE;
}

CObjInterface(EBB9PClient) EBB9PClientPrim_ftable = {
  .init      = EBB9PClientPrim_init, 
  .mount     = EBB9PClientPrim_mount, 
  .unmount   = EBB9PClientPrim_unmount, 
  .ismounted = EBB9PClientPrim_ismounted, 
  .open      = EBB9PClientPrim_open, 
  .create    = EBB9PClientPrim_create,
  .close     = EBB9PClientPrim_close, 
  .read      = EBB9PClientPrim_read, 
  .write     = EBB9PClientPrim_write
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
  
  rc = EBBAllocLocalPrimId(id);
  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  EBBRCAssert(rc);

  return EBBRC_OK;
}
