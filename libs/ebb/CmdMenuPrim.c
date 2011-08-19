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

#define STRLEN 160

CObject(CmdMenuPrim) {
  CObjInterface(CmdMenu) *ft;
  char feaddr[STRLEN];
  uval feaddrlen;
  char feprefix[STRLEN];
  uval feprefixlen;
  char appid[STRLEN];
  uval appidlen;
  char nodespath[STRLEN];
  uval nodespathlen;
  char nodeid[STRLEN];
  uval nodeidlen;
  char mynodepath[STRLEN];
  uval mynodepathlen;

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
  // FIXME: null terminate length boundaries reached
  //        cases
  return i;
}

static sval
CmdMenuPrim_doConnect(CmdMenuPrimRef self, char *buf, uval len)
{
  EBB9PClientId p;
  EBBRC rc;
  char tmp[STRLEN];
  sval n;

  self->feaddr[0] = self->feprefix[0] = self->appid[0] = 0;
  self->nodespath[0] = self->nodeid[0] = self->mynodepath[0] = 0;

  n = bufParse(buf, len, self->feaddr, STRLEN, ' ');
  len -= n; buf += n;
  self->feaddrlen = n-1;
  if (len==0) return -1;
  
  n = bufParse(buf, len, self->feprefix, STRLEN, ' ');
  len -= n; buf += n;
  self->feprefixlen = n-1;
  if (len==0) return -1;

  n = bufParse(buf, len, self->appid, STRLEN, ' ');
  len -= n; buf += n;
  self->appidlen = n-1;

  n = bufParse(buf, len, self->nodespath, STRLEN, ' ');
  len -= n; buf += n;
  self->nodespathlen = n-1;

  n = bufParse(buf, len, self->mynodepath, STRLEN, ' ');
  len -= n; buf += n;
  self->mynodepathlen = n-1;

  n = bufParse(buf, len, self->nodeid, STRLEN, '\n');
  len -= n; buf += n;
  self->nodeidlen = n-1;


  EBB_LRT_printf("%s: connect %s %s %s %s %s %s\n", __func__,
		 self->feaddr, self->feprefix, 
		 self->appid, self->nodespath, 
		 self->mynodepath, self->nodeid);

  EBB9PClientPrimCreate(&p);

  rc = EBBCALL(p, mount, self->feaddr);
  EBBRCAssert(rc);

  EBB9PFilePrimCreate(p, &self->stdin);
  EBB9PFilePrimCreate(p, &self->stdout);
  EBB9PFilePrimCreate(p, &self->stderr);

  bufParse(self->mynodepath, self->mynodepathlen, tmp, STRLEN, 0);

  // including terminating 0 in length passed to bufParse
  bufParse("/stdout", 8, &tmp[self->mynodepathlen], 
	   STRLEN - self->mynodepathlen,
	   0);
  rc = EBBCALL(self->stdout, open, tmp, 
	       EBBFILE_OWRITE | EBBFILE_OCREATE, 0777);
  EBBRCAssert(rc);
  rc = EBBCALL(self->stdout, write, self->nodeid, 
	       self->nodeidlen, &n);
  EBBRCAssert(rc);

  bufParse("/stderr", 8, &tmp[self->mynodepathlen], 
	   STRLEN - self->mynodepathlen, 0);
  rc = EBBCALL(self->stderr, open, tmp, 
	       EBBFILE_OWRITE | EBBFILE_OCREATE, 0777);
  EBBRCAssert(rc);
  rc = EBBCALL(self->stderr, write, 
	       self->nodeid, self->nodeidlen, &n);
  EBBRCAssert(rc);

  bufParse("/stdin", 7, &tmp[self->mynodepathlen], 
	   STRLEN - self->mynodepathlen, 0);
  rc = EBBCALL(self->stdin, open, tmp, 
	       EBBFILE_OREAD | EBBFILE_OCREATE, 0777);
  EBBRCAssert(rc);
  rc = EBBCALL(self->stdin, read, buf, STRLEN, &n);
  EBBRCAssert(rc);

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
    *rc = CmdMenuPrim_doConnect(self, &cmdbuf[2], n-2);
  else if ((n > 8 && bufEq(cmdbuf, "connect ", 8))) 
    *rc = CmdMenuPrim_doConnect(self, &cmdbuf[8], n-8);
  else if ((n > 2 && bufEq(cmdbuf, "r ", 2)))       
    *rc = CmdMenuPrim_doRun(self, &cmdbuf[2], n-2);
  else if ((n > 4 && bufEq(cmdbuf, "run ", 4)))     
    *rc = CmdMenuPrim_doRun(self, &cmdbuf[4], n-4);
       
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
