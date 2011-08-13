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

#include "EBB9PClient.h"
#include "EBB9PClientPrim.h"
#include "EBBFile.h"

#include "CmdMenu.h"
#include "CmdMenuPrim.h"

#include __LRTINC(misc.h)

#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

char prompt[] = "9PConsole <9PAddress>\n";

CObject(CmdMenuPrim) {
  CObjInterface(CmdMenu) *ft;
  EBBFileId stdin;
  EBBFileId stdout;
  EBBFileId stderr;
};

static uval
bufEq(char *b1, char *b2, uval len)
{
  uval i=0;  
  while (i<len && b1[i] == b2[i]) i++;
  return (i==len);
}

static void 
CmdMenuPrim_doConnect(char *buf, uval len)
{
  char *token, *saveptr;

  token = strtok_r(buf, ":", &saveptr);
  if(strcmp(token, "connect") == 0) {
#if 0 
    EBB9PClientId p;
    IxpCFid *fd;
    sval n;
    EBBRC rc;

    // replace this with the new File code
    // updating member fields appropriately.
    token = strtok_r(NULL, ":", &saveptr);
    EBB9PClientPrimCreate(&p);
    
    rc = EBBCALL(p, mount, token);
    EBBRCAssert(rc);
    
    rc = EBBCALL(p, open, "/tmp/ebbtest/stdout", 
		 P9_OWRITE | P9_OAPPEND, &fd);
    EBBRCAssert(rc);
    
    rc = EBBCALL(p, write, fd, "Hello World!\n", 13, &n);
    EBBRCAssert(rc);
#endif
  }
  
}


static 
EBBRC CmdMenuPrim_doCmd(void *_self, char *cmdbuf, uval n, sval *rc)
{
  EBB_LRT_printf("%s: _self=%p, cmdbuf=%p, n=%ld, rc=%p:\n", __func__, 
		 _self, cmdbuf, n, rc);
  EBB_LRT_write(1, cmdbuf, n);


  if (n > 8 && bufEq(cmdbuf, "connect:", 8)) CmdMenuPrim_doConnect(cmdbuf, n);

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
