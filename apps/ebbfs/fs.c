/* © 2009 JGL (yiyus) <yiyu.jgl@gmail.com>
 * See COPYING for details.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ixp.h>

#include "ebbfs.h"

static int id = 0;

/* 
0400 /id
0600 /msg
*/

/* name, parent, type, mode, size */
Fileinfo files[QMAX] = {
	{"", QNONE, P9_QTDIR, 0500|P9_DMDIR, 0},
	{"id", QROOT, P9_QTFILE, 0400, 0},
	{"ebbs", QROOT, P9_QTFILE, 0600, 0}
};

Msg*
newmsg(void)
{
	int len;
	Msg *msg;

	len = strlen(message);
	if(!(msg = malloc(sizeof(Msg))) || !(msg->data = malloc(len)))
		return NULL;
	memcpy(msg->data, message, len);
	msg->size = len;
	msg->id = id++;

	return msg;
}

void
fs_attach(Ixp9Req *r)
{
	Msg *msg = NULL;

	r->fid->qid.type = files[QROOT].type;
	r->fid->qid.path = QROOT;
	r->ofcall.rattach.qid = r->fid->qid;
	if(!(msg = newmsg())) {
		respond(r, "out of memory");
		return;
	}
	r->fid->aux = msg;
	respond(r, NULL);
}

void
fs_walk(Ixp9Req *r)
{
	char buf[512];
	qpath cwd;
	int i, j;

	cwd = r->fid->qid.path;
	r->ofcall.rwalk.nwqid = 0;
	for(i = 0; i < r->ifcall.twalk.nwname; ++i){
		for(j = 0; j < QMAX; ++j){
			if(files[j].parent == cwd && strcmp(files[j].name, r->ifcall.twalk.wname[i]) == 0)
				break;
		}
		if(j >= QMAX){
			snprintf(buf, sizeof(buf), "%s: no such file or directory", r->ifcall.twalk.wname[i]);
			respond(r, buf);
			return;
		}
		r->ofcall.rwalk.wqid[r->ofcall.rwalk.nwqid].type = files[j].type;
		r->ofcall.rwalk.wqid[r->ofcall.rwalk.nwqid].path = j;
		r->ofcall.rwalk.wqid[r->ofcall.rwalk.nwqid].version = 0;
		++r->ofcall.rwalk.nwqid;
	}
	r->newfid->aux = r->fid->aux;
	respond(r, NULL);
}

void
fs_open(Ixp9Req *r)
{
	respond(r, NULL);
}

void
fs_clunk(Ixp9Req *r)
{
	respond(r, NULL);
}

void
dostat(IxpStat *st, int path)
{
	st->type = files[path].type;
	st->qid.type = files[path].type;
	st->qid.path = path;
	st->mode = files[path].mode;
	st->name = files[path].name;
	st->length = 0;
	st->uid = st->gid = st->muid = "";
}

void
fs_stat(Ixp9Req *r)
{
	IxpStat st = {0};
	IxpMsg m;
	char buf[512];
	Msg *msg;

	dostat(&st, r->fid->qid.path);
	msg = r->fid->aux;
	st.length = msg->size;
	m = ixp_message(buf, sizeof(buf), MsgPack);
	ixp_pstat(&m, &st);
	r->ofcall.rstat.nstat = ixp_sizeof_stat(&st);
	if(!(r->ofcall.rstat.stat = malloc(r->ofcall.rstat.nstat))) {
		r->ofcall.rstat.nstat = 0;
		respond(r, "out of memory");
		return;
	}
	memcpy(r->ofcall.rstat.stat, m.data, r->ofcall.rstat.nstat);
	respond(r, NULL);
}

void
fs_read(Ixp9Req *r)
{
	char buf[512];
	Msg *msg;
	int n;

	if(files[r->fid->qid.path].type & P9_QTDIR){
		IxpStat st = {0};
		IxpMsg m;
		int i;

		m = ixp_message(buf, sizeof(buf), MsgPack);

		r->ofcall.rread.count = 0;
		if(r->ifcall.tread.offset > 0) {
			/* hack! assuming the whole directory fits in a single Rread */
			respond(r, NULL);
			return;
		}
		for(i = 0; i < QMAX; ++i){
			if(files[i].parent == r->fid->qid.path){
				dostat(&st, i);
				ixp_pstat(&m, &st);
				r->ofcall.rread.count += ixp_sizeof_stat(&st);
			}
		}
		if(!(r->ofcall.rread.data = malloc(r->ofcall.rread.count))) {
			r->ofcall.rread.count = 0;
			respond(r, "out of memory");
			return;
		}
		memcpy(r->ofcall.rread.data, m.data, r->ofcall.rread.count);
		respond(r, NULL);
		return;
	}

	msg = r->fid->aux;

	switch(r->fid->qid.path){
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
					return;
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
					return;
				}
				memcpy(r->ofcall.rread.data, msg->data+r->ifcall.tread.offset, r->ofcall.rread.count);
			}
			break;
		}
	}

	respond(r, NULL);
}

void
fs_write(Ixp9Req *r)
{
	Msg *msg;

	msg = r->fid->aux;

	switch(r->fid->qid.path){
		case QMSG: {
			r->ofcall.rwrite.count = r->ifcall.twrite.count;
			if(!(msg->data = realloc(msg->data, r->ifcall.twrite.count))) {
				r->ofcall.rwrite.count = 0;
				respond(r, "out of memory");
				return;
			}
			memcpy(msg->data, r->ifcall.twrite.data, r->ofcall.rwrite.count);
			msg->size = r->ofcall.rwrite.count;
			break;
		}
	}
	respond(r, NULL);
}

void
fs_wstat(Ixp9Req *r)
{
	respond(r, NULL); /* pretend it worked */
}

Ixp9Srv p9srv = {
	.open=fs_open,
	.clunk=fs_clunk,
	.walk=fs_walk,
	.read=fs_read,
	.stat=fs_stat,
	.write=fs_write,
	.wstat=fs_wstat,
	.attach=fs_attach
};
