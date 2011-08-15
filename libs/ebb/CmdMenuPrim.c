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
CmdMenuPrim_doConnect(CmdMenuPrimRef *self, char *buf, uval len)
{
  char addr[80], prefix[80], nodeid[80];
  uval n;

  addr[0] = prefix[0] = nodeid[0] = 0;

  n = bufParse(buf, len, addr, 80, ' ');
  len -= n; buf += n;
  if (len==0) return -1;
  
  n = bufParse(buf, len, prefix, 80, ' ');
  len -= n; buf += n;
  if (len==0) return -1;

  n = bufParse(buf, len, nodeid, 80, ' ');
  len -= n; buf += n;

  EBB_LRT_printf("%s, connect %s %s %s\n", __func__,
		 addr, prefix, nodeid);

  return 1;
}

#if 0 
  char *token, *saveptr;

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

static sval
CmdMenuPrim_doRun(CmdMenuPrimRef *self, char *buf, uval len)
{
  return -1;
}

static 
EBBRC CmdMenuPrim_doCmd(void *_self, char *cmdbuf, uval n, sval *rc)
{
  CmdMenuPrimRef *self = _self;
 
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
