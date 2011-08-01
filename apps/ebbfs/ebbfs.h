#ifndef __EBBFS_H__
#define __EBBFS_H__

typedef enum {QNONE=-1, QROOT=0, QIDENT, QMSG, QMAX} qpath;
typedef struct Fileinfo_ Fileinfo;
typedef struct Msg_ Msg;

struct Fileinfo_
{
	char *name;
	qpath parent;
	int type;
	int mode;
	unsigned int size;
};

struct Msg_
{
	int id;
	int size;
	char *data;
};

extern Fileinfo files[];
extern char *message;
extern char hellomsg[12];

#endif
