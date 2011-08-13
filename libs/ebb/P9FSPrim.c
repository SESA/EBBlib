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


#define IXP_NO_P9_
#define IXP_P9_STRUCTS
#include <ixp.h>

#include "P9FS.h"
#include "CmdMenu.h"
#include "P9FSPrim.h"
#include "EBBAssert.h"

#include __LRTINC(misc.h)

#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

static void p9fs_ebb_attach(Ixp9Req *r);
static void p9fs_ebb_walk(Ixp9Req *r);
static void p9fs_ebb_open(Ixp9Req *r);
static void p9fs_ebb_clunk(Ixp9Req *r);
static void p9fs_ebb_stat(Ixp9Req *r);
static void p9fs_ebb_read(Ixp9Req *r);
static void p9fs_ebb_write(Ixp9Req *r);
static void p9fs_ebb_wstat(Ixp9Req *r);

typedef enum {QNONE=-1, QROOT=0, QIDENT, QMSG, QCMD, QMAX} qpath;

typedef struct {
  char *name;
  qpath parent;
  int type;
  int mode;
  unsigned int size;
} P9FSPrim_finfo;

P9FSPrim_finfo Files[QMAX] = {
	{"", QNONE, P9_QTDIR, 0500|P9_DMDIR, 0},
	{"id", QROOT, P9_QTFILE, 0400, 0},
	{"ebbs", QROOT, P9_QTFILE, 0600, 0},
	{"cmd", QROOT, P9_QTFILE, 0600, 0}
};

typedef struct {
    int id;
    int size;
    char *data;
} P9FSPrim_msg;


CObject (P9FSPrim) {
  CObjInterface(P9FS) *ft;
  char cmdReplyStr[80];

  P9FSPrim_finfo *files;

  P9FSPrim_msg msg;

  Ixp9Srv p9srv;
  char *message;
  uval id;
  CmdMenuId cmd;
  sval cmdRC;  
};

static P9FSPrim_msg *
newmsg(P9FSPrimRef self)
{
  int len;
  P9FSPrim_msg *msg;
  
  len = strlen(self->message);
  if(!(msg = malloc(sizeof(P9FSPrim_msg))) || !(msg->data = malloc(len)))
    return NULL;
  memcpy(msg->data, self->message, len);
  msg->size = len;
  msg->id = self->id++;
  
  return msg;
}

static EBBRC 
P9FSPrim_attach(void *_self, Ixp9Req *r)
{
  P9FSPrimRef self  = _self;
  P9FSPrim_msg *msg = NULL;
  
  r->fid->qid.type = self->files[QROOT].type;
  r->fid->qid.path = QROOT;
  r->ofcall.rattach.qid = r->fid->qid;
  if(!(msg = newmsg(self))) {
    respond(r, "out of memory");
    return EBBRC_OK;
  }
  r->fid->aux = msg;
  respond(r, NULL);

  return EBBRC_OK;
}

static EBBRC 
P9FSPrim_walk(void *_self, Ixp9Req *r)
{
  P9FSPrimRef self  = _self;
  char buf[512];
  qpath cwd;
  int i, j;
  
  cwd = r->fid->qid.path;
  r->ofcall.rwalk.nwqid = 0;
  for(i = 0; i < r->ifcall.twalk.nwname; ++i){
    for(j = 0; j < QMAX; ++j){
      if(self->files[j].parent == cwd && 
	 strcmp(self->files[j].name, r->ifcall.twalk.wname[i]) == 0)
	break;
    }
    if(j >= QMAX){
      snprintf(buf, sizeof(buf), "%s: no such file or directory", 
	       r->ifcall.twalk.wname[i]);
      respond(r, buf);
      return EBBRC_OK;
    }
    r->ofcall.rwalk.wqid[r->ofcall.rwalk.nwqid].type = self->files[j].type;
    r->ofcall.rwalk.wqid[r->ofcall.rwalk.nwqid].path = j;
    r->ofcall.rwalk.wqid[r->ofcall.rwalk.nwqid].version = 0;
    ++r->ofcall.rwalk.nwqid;
  }
  r->newfid->aux = r->fid->aux;
  respond(r, NULL);
  
  return EBBRC_OK;
}

static EBBRC 
P9FSPrim_open(void *_self, Ixp9Req *r)
{
  respond(r, NULL);
  return EBBRC_OK;
}

static EBBRC 
P9FSPrim_clunk(void *_self, Ixp9Req *r)
{
  respond(r, NULL);
 return EBBRC_OK;
}

static void
dostat(P9FSPrimRef self, IxpStat *st, int path)
{
  st->type = self->files[path].type;
  st->qid.type = self->files[path].type;
  st->qid.path = path;
  st->mode = self->files[path].mode;
  st->name = self->files[path].name;
  st->length = 0;
  st->uid = st->gid = st->muid = "";
}

static EBBRC 
P9FSPrim_stat(void *_self, Ixp9Req *r)
{
  P9FSPrimRef self  = _self;
  IxpStat st = {0};
  IxpMsg m;
  char buf[512];
  P9FSPrim_msg *msg;
  
  dostat(self, &st, r->fid->qid.path);
  msg = r->fid->aux;
  st.length = msg->size;
  m = ixp_message(buf, sizeof(buf), MsgPack);
  ixp_pstat(&m, &st);
  r->ofcall.rstat.nstat = ixp_sizeof_stat(&st);
  if(!(r->ofcall.rstat.stat = malloc(r->ofcall.rstat.nstat))) {
    r->ofcall.rstat.nstat = 0;
    respond(r, "out of memory");
    return EBBRC_OK;
  }
  memcpy(r->ofcall.rstat.stat, m.data, r->ofcall.rstat.nstat);
  respond(r, NULL);
  
  return EBBRC_OK;
}

static EBBRC 
P9FSPrim_read(void *_self, Ixp9Req *r)
{
  P9FSPrimRef self  = _self;
  char buf[512];
  P9FSPrim_msg *msg;
  int n;
  
  if(self->files[r->fid->qid.path].type & P9_QTDIR){
    IxpStat st = {0};
    IxpMsg m;
    int i;
    
    m = ixp_message(buf, sizeof(buf), MsgPack);
    
    r->ofcall.rread.count = 0;
    if(r->ifcall.tread.offset > 0) {
      /* hack! assuming the whole directory fits in a single Rread */
      respond(r, NULL);
      return EBBRC_OK;
    }
    for(i = 0; i < QMAX; ++i){
      if(self->files[i].parent == r->fid->qid.path){
	dostat(self, &st, i);
	ixp_pstat(&m, &st);
	r->ofcall.rread.count += ixp_sizeof_stat(&st);
      }
    }
    if(!(r->ofcall.rread.data = malloc(r->ofcall.rread.count))) {
      r->ofcall.rread.count = 0;
      respond(r, "out of memory");
      return EBBRC_OK;
    }
    memcpy(r->ofcall.rread.data, m.data, r->ofcall.rread.count);
    respond(r, NULL);
    return EBBRC_OK;
  }
  
  msg = r->fid->aux;
  
  switch(r->fid->qid.path) {
  case QIDENT: {
    sprintf(buf, "%11d ", msg->id);
    if(r->ifcall.tread.offset <  11) {
      n = strlen(buf);
      if(r->ifcall.tread.offset + r->ifcall.tread.count >  11) {
	r->ofcall.rread.count =  11 - r->ifcall.tread.offset;
      } else {
	r->ofcall.rread.count = r->ifcall.tread.count;
      }
      if(!(r->ofcall.rread.data = malloc(r->ofcall.rread.count))) {
	r->ofcall.rread.count = 0;
	respond(r, "out of memory");
	return EBBRC_OK;
      }
      memcpy(r->ofcall.rread.data, buf, r->ofcall.rread.count);
    }
    break;
  }
  case QMSG: {
    if(r->ifcall.tread.offset < msg->size) {
      if(r->ifcall.tread.offset + r->ifcall.tread.count > msg->size) {
	r->ofcall.rread.count = msg->size - r->ifcall.tread.offset;
      } else {
	r->ofcall.rread.count = r->ifcall.tread.count;
      }
      if(!(r->ofcall.rread.data = malloc(r->ofcall.rread.count))) {
	r->ofcall.rread.count = 0;
	respond(r, "out of memory");
	return EBBRC_OK;
      }
      memcpy(r->ofcall.rread.data, msg->data+r->ifcall.tread.offset, 
	     r->ofcall.rread.count);
    }
    break;
  }
  case QCMD: {
    uval i=0;
    bzero(self->cmdReplyStr, 80);
    sprintf(self->cmdReplyStr, "%ld", self->cmdRC);
    while (self->cmdReplyStr[i]) i++;
    if(r->ifcall.tread.offset < i) {
      if(r->ifcall.tread.offset + r->ifcall.tread.count > i) {
	r->ofcall.rread.count = i - r->ifcall.tread.offset;
      } else {
	r->ofcall.rread.count = r->ifcall.tread.count;
      }
      if(!(r->ofcall.rread.data = malloc(r->ofcall.rread.count))) {
	r->ofcall.rread.count = 0;
	respond(r, "out of memory");
	return EBBRC_OK;
      }
      memcpy(r->ofcall.rread.data, self->cmdReplyStr+r->ifcall.tread.offset, 
	     r->ofcall.rread.count);
    }
    break;
  } 
  }
  
  respond(r, NULL);
  
  return EBBRC_OK;
}

static 
EBBRC P9FSPrim_write(void *_self, Ixp9Req *r)
{
  P9FSPrimRef self  = _self;
  P9FSPrim_msg *msg;
  EBBRC rc;


  msg = r->fid->aux;
  
  switch(r->fid->qid.path){
  case QMSG: {
    r->ofcall.rwrite.count = r->ifcall.twrite.count;
    if(!(msg->data = realloc(msg->data, r->ifcall.twrite.count))) {
      r->ofcall.rwrite.count = 0;
      respond(r, "out of memory");
      return EBBRC_OK;
    }
    memcpy(msg->data, r->ifcall.twrite.data, r->ofcall.rwrite.count);
    msg->size = r->ofcall.rwrite.count;
    break;
  }
  case QCMD: {
    if(!r->ifcall.twrite.data || r->ifcall.twrite.data[0] == 0) break;
    r->ofcall.rwrite.count = r->ifcall.twrite.count;
    rc = EBBCALL(self->cmd, doCmd, r->ifcall.twrite.data, 
		 r->ifcall.twrite.count, &(self->cmdRC));
    EBBRCAssert(rc);
    break;
  }
  }
  respond(r, NULL);
  
  return EBBRC_OK;
}

static EBBRC 
P9FSPrim_wstat(void *_self, Ixp9Req *r)
{
  respond(r, NULL); /* pretend it worked */
  return EBBRC_OK;
}

static EBBRC 
P9FSPrim_serverloop(void *_self, char *address)
{
  static char *msg = "Hello World!\n";
  P9FSPrimRef self  = _self;
  IxpServer srv = {0};
  uval fd;

  fd = ixp_announce(address);
  if (fd<0) return EBBRC_GENERIC_FAILURE;
  ixp_listen(&srv, fd, &(self->p9srv), serve_9pcon, NULL);

  self->message = msg;
  
  (void)ixp_serverloop(&srv);
  
  return EBBRC_OK;
}

CObjInterface(P9FS) P9FSPrim_ftable = {
  .attach     = P9FSPrim_attach,
  .open       = P9FSPrim_open,
  .walk       = P9FSPrim_walk,
  .clunk      = P9FSPrim_clunk,
  .stat       = P9FSPrim_stat,
  .read       = P9FSPrim_read,
  .write      = P9FSPrim_write,
  .wstat      = P9FSPrim_wstat,
  .serverloop = P9FSPrim_serverloop
};

static inline void
P9FSPrimSetFT(P9FSPrimRef o) 
{
  o->ft = &P9FSPrim_ftable;
}


static void
P9FSPrim_init(P9FSPrimRef rep, CmdMenuId cmd)
{
  rep->files    = Files;
  rep->id       = 0;
  rep->message  = NULL;
  rep->cmd      = cmd;
  rep->cmdRC    = 0;
  
  rep->p9srv.open   = p9fs_ebb_open;
  rep->p9srv.clunk  = p9fs_ebb_clunk;
  rep->p9srv.walk   = p9fs_ebb_walk;
  rep->p9srv.read   = p9fs_ebb_read;
  rep->p9srv.stat   = p9fs_ebb_stat;
  rep->p9srv.write  = p9fs_ebb_write;
  rep->p9srv.wstat  = p9fs_ebb_wstat;
  rep->p9srv.attach = p9fs_ebb_attach;
}


EBBRC
P9FSPrimCreate(P9FSid *id, CmdMenuId cmd)
{
  EBBRC rc;  
  P9FSPrimRef repRef;
  CObjEBBRootSharedRef rootRef;

  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  EBBPrimMalloc(sizeof(*rootRef), &rootRef, EBB_MEM_DEFAULT);
  
  CObjEBBRootSharedSetFT(rootRef);
  P9FSPrimSetFT(repRef);
  
  P9FSPrim_init(repRef, cmd);
  rootRef->ft->init(rootRef, repRef);
  
  rc = EBBAllocPrimId(id);
  EBBRCAssert(rc);

  rc = CObjEBBBind(*id, rootRef); 
  EBBRCAssert(rc);
  
  repRef->p9srv.aux = *id;

  return rc;
}

// EVENTUALLY THESE SHOULD GO AWAY WHEN 
// WE HAVE A REAL EBB PORT OF IXPLIB

static void
p9fs_ebb_attach(Ixp9Req *r)
{
  P9FSid id = (P9FSid) r->srv->aux;
  EBBCALL(id, attach, r);
}

static void
p9fs_ebb_open(Ixp9Req *r)
{
  P9FSid id = (P9FSid) r->srv->aux;
  EBBCALL(id, open, r);
}

static void
p9fs_ebb_walk(Ixp9Req *r)
{
  P9FSid id = (P9FSid) r->srv->aux;
  EBBCALL(id, walk, r);
}

static void
p9fs_ebb_clunk(Ixp9Req *r)
{
  P9FSid id;
  if (r->srv && r->srv->aux) {
    id = (P9FSid) r->srv->aux;
    EBBCALL(id, clunk, r);
  }
}

static void
p9fs_ebb_stat(Ixp9Req *r)
{
  P9FSid id = (P9FSid) r->srv->aux;
  EBBCALL(id, stat, r);
}
  

static void
p9fs_ebb_read(Ixp9Req *r)
{
  P9FSid id = (P9FSid) r->srv->aux;
  EBBCALL(id, read, r);
}

static void 
p9fs_ebb_write(Ixp9Req *r)
{
  P9FSid id = (P9FSid) r->srv->aux;
  EBBCALL(id, write, r);
}

static void
p9fs_ebb_wstat(Ixp9Req *r)
{
  P9FSid id = (P9FSid) r->srv->aux;
  EBBCALL(id, wstat, r);
}
