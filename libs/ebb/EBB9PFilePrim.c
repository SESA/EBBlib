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
#include "EBBFile.h"
#include "EBB9PFilePrim.h"
#include "EBBAssert.h"

#define VERBOSE_PR(...) ( EBB_LRT_printf(__VA_ARGS__) )

#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

CObject(EBB9PFilePrim) {
  CObjInterface(EBBFile) *ft;
  EBB9PClientId cid;
  IxpCFid   *fd;
};

static uchar EBB9PFile_convertMode(uval ebbfilemode) {
  uchar rc = 0;
  
  if (ebbfilemode & EBBFILE_OREAD) rc |= P9_OREAD;
  if (ebbfilemode & EBBFILE_OWRITE) rc |= P9_OWRITE;
  if (ebbfilemode & EBBFILE_ORDWR) rc |= P9_ORDWR;

  return rc;
}
static EBBRC
EBB9PFilePrim_open(void *_self, char *path, uval mode, uval perm) 
{ 
  EBB9PFilePrim *self = _self;
  EBBRC rc;
  uval P9mode;

  VERBOSE_PR("%s: open\n", __func__);
  
  P9mode = EBB9PFile_convertMode(mode);

  if (mode & EBBFILE_OCREATE ) {
    rc = EBBCALL(self->cid, create, path, perm, P9mode, &(self->fd));
  } else {
    rc = EBBCALL(self->cid, open, path, P9mode, &(self->fd));
  }
  EBBRCAssert(rc);

  return EBBRC_OK;
}

static EBBRC 
EBB9PFilePrim_close(void *_self, uval *rc) 
{ 
  EBB9PFilePrim *self = _self;
  EBBRC ebbrc;

  VERBOSE_PR("%s: close\n", __func__);

  ebbrc = EBBCALL(self->cid, close, self->fd, rc);
  EBBRCAssert(ebbrc);

  return EBBRC_OK; 
}

static EBBRC 
EBB9PFilePrim_read(void *_self, void *buf, sval cnt, sval *n)
{ 
  EBB9PFilePrim *self = _self;
  EBBRC rc;

  VERBOSE_PR("%s: read\n", __func__);

  rc = EBBCALL(self->cid, read, self->fd, buf, cnt, n);
  EBBRCAssert(rc);

  return EBBRC_OK; 
}

static EBBRC
EBB9PFilePrim_write(void *_self, const void *buf, sval cnt, sval *n)
{ 
  EBB9PFilePrim *self = _self;
  EBBRC rc;

  VERBOSE_PR("%s: write\n", __func__);

  rc = EBBCALL(self->cid, write, self->fd, buf, cnt, n);
  EBBRCAssert(rc);

  return EBBRC_OK; 
}


static EBBRC
EBB9PFilePrim_seek(void *_self, sval offset, uval whence, sval *rc)
{ 
  EBB9PFilePrim *self = _self;
  
  VERBOSE_PR("%s: seek\n", __func__);
  *rc = ixp_seek(self->fd, (vlong)offset, (long)whence);
  return EBBRC_OK; 
}

CObjInterface(EBBFile) EBB9PFilePrim_ftable = {
  .open  = EBB9PFilePrim_open, 
  .close = EBB9PFilePrim_close, 
  .read  = EBB9PFilePrim_read, 
  .write = EBB9PFilePrim_write,
  .seek  = EBB9PFilePrim_seek
};

static inline void 
EBB9PFilePrimSetFT(EBB9PFilePrimRef o) 
{ 
  o->ft = &EBB9PFilePrim_ftable; 
}

EBBRC
EBB9PFilePrimCreate(EBB9PClientId cid, EBBFileId *id) 
{
  EBBRC rc;

  EBB9PFilePrimRef repRef;
  CObjEBBRootSharedRef rootRef;

  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  
  CObjEBBRootSharedSetFT(rootRef);
  EBB9PFilePrimSetFT(repRef);
  
  repRef->cid = cid;
  repRef->fd = NULL;

  rootRef->ft->init(rootRef, repRef);
  
  rc = EBBAllocLocalPrimId(id);
  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  EBBRCAssert(rc);

  return EBBRC_OK;
}
