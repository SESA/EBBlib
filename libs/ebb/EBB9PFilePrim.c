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
#include "EBBFile.h"
#include "EBB9PFilePrim.h"
#include "EBBAssert.h"


CObject(EBB9PFilePrim) {
  CObjInterface(EBBFile) *ft;
  EBB9PClientId cid;
  IxpCFid   *fd;
};

static EBBRC
EBB9PFilePrim_open(void *_self, char *path, uval8 mode) 
{ 
  EBB_LRT_printf("EBB9PFilePrim: open\n");
  return EBBRC_OK;
}

static EBBRC 
EBB9PFilePrim_close(void *_self, int *rc) 
{ 
  EBB_LRT_printf("EBB9PFilePrim: close\n");
  return EBBRC_OK; 
}

static EBBRC 
EBB9PFilePrim_read(void *_self, void *buf, sval cnt, sval *n)
{ 
  EBB_LRT_printf("EBB9PFilePrim: read\n");
  return EBBRC_OK; 
}

static EBBRC
EBB9PFilePrim_write(void *_self, const void *buf, sval cnt,sval *n)
{ 
  EBB_LRT_printf("EBB9PFilePrim: write\n");
  return EBBRC_OK; 
}

CObjInterface(EBBFile) EBB9PFilePrim_ftable = {
  .open  = EBB9PFilePrim_open, 
  .close = EBB9PFilePrim_close, 
  .read  = EBB9PFilePrim_read, 
  .write = EBB9PFilePrim_write
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
  
  rc = EBBAllocPrimId(id);
  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  EBBRCAssert(rc);

  return EBBRC_OK;
}
