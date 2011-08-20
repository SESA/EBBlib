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
#include "EBBAssert.h"

#include "EBBCtr.h"
#include "EBBCtrPrim.h"
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
      // replace the separator with a null
      dest[i] = 0;
      i++;
      break;
    } 
    i++;
  }
  // regardless null terminate
  // we truncate to destination to null terminate
  if (i>=dl) dest[dl-1]=0;
  else dest[i]=0;

  return i;
}

EBB9PClientId the9PClient = NULL;
uval EBBNodeId = 0;

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

  EBBCALL(theEBBMgrPrimId, InitMessageMgr, (EBBId) p, self->nodespath, self->nodespathlen);

  EBBNodeId = atol(self->nodeid);

  return 1;
}

EBBRC
testMsgHandler(uval arg0, uval arg1, uval arg2, uval arg3,
	      uval arg4, uval arg5, uval arg6, uval arg7)
{
  EBB_LRT_printf("%s: called with %ld %ld %ld %ld %ld %ld %ld %ld\n",
		 __func__, 
		 arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
  return EBBRC_OK;
}


static sval
CmdMenuPrim_doRun(CmdMenuPrimRef self, char *buf, uval len)
{
  //Feel free to comment this out, just using it to test the global EBB stuff
  EBBCtrPrimId ctr;
  uval v;
  EBBRC rc;
  
  rc = EBBMessageNode(2, testMsgHandler);
  EBB_LRT_printf("%s, MsgNode: rc=%ld\n", __func__, rc);

  rc = EBBMessageNode1(2, testMsgHandler, 1);
  EBB_LRT_printf("%s, MsgNode 1: rc=%ld\n", __func__, rc);

  rc = EBBMessageNode2(2, testMsgHandler, 1, 2);
  EBB_LRT_printf("%s, MsgNode 2: rc=%ld\n", __func__, rc);

  rc = EBBMessageNode3(2, testMsgHandler, 1, 2, 3);
  EBB_LRT_printf("%s, MsgNode 3: rc=%ld\n", __func__, rc);

  rc = EBBMessageNode4(2, testMsgHandler, 1, 2, 3, 4);
  EBB_LRT_printf("%s, MsgNode 4: rc=%ld\n", __func__, rc);

  rc = EBBMessageNode5(2, testMsgHandler, 1, 2, 3, 4, 5);
  EBB_LRT_printf("%s, MsgNode 5: rc=%ld\n", __func__, rc);

  rc = EBBMessageNode6(2, testMsgHandler, 1, 2, 3, 4, 5, 6);
  EBB_LRT_printf("%s, MsgNode 6: rc=%ld\n", __func__, rc);

  rc = EBBMessageNode7(2, testMsgHandler, 1, 2, 3, 4, 5, 6, 7);
  EBB_LRT_printf("%s, MsgNode 7: rc=%ld\n", __func__, rc);

  rc = EBBMessageNode8(2, testMsgHandler, 1, 2, 3, 4, 5, 6, 7, 8);
  EBB_LRT_printf("%s, MsgNode 8: rc=%ld\n", __func__, rc);

#if 0
  EBBCtrPrimGlobalSharedCreate(&ctr);
  EBBCALL(ctr,val,&v);
  EBB_LRT_printf("global counter val = %ld\n",v);
  EBBCALL(ctr,inc);
  EBBCALL(ctr,val,&v);
  EBB_LRT_printf("global counter val = %ld\n",v);
#endif
  return EBBRC_OK;
}

static sval
CmdMenuPrim_do9pr(CmdMenuPrimRef self, char *buf, uval len)
{
  EBB9PClientId p;
  EBBRC rc;
  IxpCFid  *fd;
  char rbuf[STRLEN];
  char addr[STRLEN];
  char path[STRLEN];
  uval addrlen;
  uval pathlen;
  sval n;

  n = bufParse(buf, len, addr, STRLEN, ' ');
  len -= n; buf += n;
  addrlen = n-1;
  if (len==0) return -1;

  n = bufParse(buf, len, path, STRLEN, '\n');
  len -= n; buf += n;
  pathlen = n-1;


  EBB_LRT_printf("%s: BEGIN: address=%s path=%s|\n", __func__, addr, path);

  EBB9PClientPrimCreate(&p);

  rc = EBBCALL(p, mount, addr);
  EBBRCAssert(rc);

  rc = EBBCALL(p, open, path, P9_OREAD, &fd);
  EBBRCAssert(rc);

  rc = EBBCALL(p, read, fd, rbuf, STRLEN, &n); 
  EBBRCAssert(rc);  
  rbuf[n] = 0;
  EBB_LRT_printf("%s\n", rbuf);
  
  EBB_LRT_printf("%s: END\n", __func__);
}

static 
EBBRC CmdMenuPrim_doCmd(void *_self, char *cmdbuf, uval n, sval *rc)
{
  CmdMenuPrimRef self = _self;
 
  EBB_LRT_printf("%s: _self=%p, cmdbuf=%p, n=%ld, rc=%p:\n", __func__, 
		 _self, cmdbuf, n, rc);

  //I think here you need to decrement n when you send it to the next function
  if ((n > 2 && bufEq(cmdbuf, "c ", 2)))            
    *rc = CmdMenuPrim_doConnect(self, &cmdbuf[2], n-2);
  else if ((n > 8 && bufEq(cmdbuf, "connect ", 8))) 
    *rc = CmdMenuPrim_doConnect(self, &cmdbuf[8], n-8);
  else if ((n > 2 && bufEq(cmdbuf, "r ", 2)))       
    *rc = CmdMenuPrim_doRun(self, &cmdbuf[2], n-2);
  else if ((n > 4 && bufEq(cmdbuf, "run ", 4)))     
    *rc = CmdMenuPrim_doRun(self, &cmdbuf[4], n-4);
  else if ((n > 4 && bufEq(cmdbuf, "9pr ", 4))) 
    *rc = CmdMenuPrim_do9pr(self, &cmdbuf[4], n-4);
  else  {
    cmdbuf[n]=0;
    EBB_LRT_printf("%s: unknown command %s\n", __func__, cmdbuf);
  }
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
  
  rc = EBBAllocLocalPrimId(id);
  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  EBBRCAssert(rc);

  return EBBRC_OK;
}
