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
#include "EBBAssert.h"

#include "EBBFile.h"

#include "CmdMenu.h"
#include "CmdMenuPrim.h"

char prompt[] = "9PConsole <9PAddress>\n";

CObject(CmdMenuPrim) {
  CObjInterface(CmdMenu) *ft;
  EBBFileId stdin;
  EBBFileId stdout;
  EBBFileId stderr;
};

static 
EBBRC CmdMenuPrim_doCmd(void *_self, char *cmdbuf, uval n, sval *rc)
{
  EBB_LRT_printf("%s: _self=%p, cmdbuf=%p, n=%ld, rc=%p:\n", __func__, 
		 _self, cmdbuf, n, rc);
  EBB_LRT_write(1, cmdbuf, n);
  *rc = 10;
  return EBBRC_OK;
}

CObjInterface(CmdMenu) CmdMenuPrim_ftable = {
  .doCmd = CmdMenuPrim_doCmd
};

static inline void
CmdMenuPrimSetFT(CmdMenuPrimRef o)
{
  o->ft = &CmdMenuPrim_ftable;
}

extern EBBRC CmdMenuPrimCreate(CmdMenuId *id)
{
  EBBRC rc;

  CmdMenuPrimRef repRef;
  CObjEBBRootSharedRef rootRef;

  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  
  CObjEBBRootSharedSetFT(rootRef);
  CmdMenuPrimSetFT(repRef);
  
  
  rootRef->ft->init(rootRef, repRef);
  
  rc = EBBAllocPrimId(id);
  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  EBBRCAssert(rc);

  return EBBRC_OK;
}
