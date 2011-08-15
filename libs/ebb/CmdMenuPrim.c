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
#include "EBB9PFilePrim.h"
#include "CmdMenu.h"
#include "CmdMenuPrim.h"

#include __LRTINC(misc.h)

#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

char prompt[] = "9PConsole <9PAddress>\n";

CObject(CmdMenuPrim) {
  CObjInterface(CmdMenu) *ft;
  char feaddr[80];
  char feprefix[80];
  char nodeid[80];
  uval feaddrlen;
  uval feprefixlen;
  uval nodeidlen;
  EBBFileId stdin;
  EBBFileId stdout;
  EBBFileId stderr;
};

static uval
bufEq(char *b1, char *b2, uval len)
{
  uval i=0;  

  while (i<len) { 
    if (b1[i] != b2[i]) break;  
    i++;
  }

  return (i==len);
}

static uval
bufParse(char *src, uval sl, char *dest, uval dl, char sep)
{
  uval i=0;

  while (i<sl && i<dl) {
    dest[i] = src[i];
    if (dest[i]==sep) {
      dest[i] = 0;
      i++;
      break;
    } 
    i++;
  }
  return i;
}

static sval
CmdMenuPrim_doConnect(CmdMenuPrimRef self, char *buf, uval len)
{
  EBB9PClientId p;
  EBBRC rc;
  char tmp[80];
  sval n;

  self->feaddr[0] = self->feprefix[0] = self->nodeid[0] = 0;

  n = bufParse(buf, len, self->feaddr, 80, ' ');
  len -= n; buf += n;
  self->feaddrlen = n;
  if (len==0) return -1;
  
  n = bufParse(buf, len, self->feprefix, 80, ' ');
  len -= n; buf += n;
  self->feprefixlen = n;
  if (len==0) return -1;

  n = bufParse(buf, len, self->nodeid, 80, ' ');
  len -= n; buf += n;
  self->nodeidlen = n;

  EBB_LRT_printf("%s: connect %s %s %s\n", __func__,
		 self->feaddr, self->feprefix, self->nodeid);

  EBB9PClientPrimCreate(&p);

  rc = EBBCALL(p, mount, self->feaddr);
  EBBRCAssert(rc);

  EBB9PFilePrimCreate(p, &self->stdin);
  EBB9PFilePrimCreate(p, &self->stdout);
  EBB9PFilePrimCreate(p, &self->stderr);

  bufParse(self->feprefix, self->feprefixlen, tmp, 80, 0);
  bufParse("/stdout", 7, &tmp[self->feprefixlen], 80 - self->feprefixlen, 0);

  EBB_LRT_printf("%s: opening %s\n", __func__, tmp);
  rc = EBBCALL(self->stdout, open, tmp, EBBFILE_OWRITE | EBBFILE_OCREATE, 0777);
  EBBRCAssert(rc);
  rc = EBBCALL(self->stdout, write, self->nodeid, self->nodeidlen, &n);
  EBBRCAssert(rc);

#if 0
  rc = EBBCALL(f2, open, "/tmp/stderr", EBBFILE_OWRITE | EBBFILE_OCREATE, 0777);
  EBBRCAssert(rc);
  rc = EBBCALL(f2, write, "stderr", 6, &n);
  EBBRCAssert(rc);

  rc = EBBCALL(f3, open, "/tmp/stdin", EBBFILE_OREAD | EBBFILE_OCREATE, 0777);
  EBBRCAssert(rc);
  rc = EBBCALL(f3, read, buf, 80, &n);
  EBBRCAssert(rc);

  EBB_LRT_printf("read: rc=%ld, n=%ld buf=:\n", rc, n);
  if (n) write(1, buf, n);
#endif

  return 1;
}


static sval
CmdMenuPrim_doRun(CmdMenuPrimRef self, char *buf, uval len)
{
  return -1;
}

static 
EBBRC CmdMenuPrim_doCmd(void *_self, char *cmdbuf, uval n, sval *rc)
{
  CmdMenuPrimRef self = _self;
 
  EBB_LRT_printf("%s: _self=%p, cmdbuf=%p, n=%ld, rc=%p:\n", __func__, 
		 _self, cmdbuf, n, rc);

  if ((n > 2 && bufEq(cmdbuf, "c ", 2)))            
    *rc = CmdMenuPrim_doConnect(self, &cmdbuf[2], n);
  else if ((n > 8 && bufEq(cmdbuf, "connect ", 8))) 
    *rc = CmdMenuPrim_doConnect(self, &cmdbuf[8], n);
  else if ((n > 2 && bufEq(cmdbuf, "r ", 2)))       
    *rc = CmdMenuPrim_doRun(self, &cmdbuf[2], n);
  else if ((n > 4 && bufEq(cmdbuf, "run ", 4)))     
    *rc = CmdMenuPrim_doRun(self, &cmdbuf[4], n);
       
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
