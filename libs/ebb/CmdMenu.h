#ifndef __EBB_CMDMENU_H__
#define __EBB_CMDMENU_H__

CObjInterface(CmdMenu) {
  EBBRC (*doCmd)  (void *_self, char *cmdbuf, uval n, char *rtndata, uval *rdlen);
};

CObject(CmdMenu) {
  CObjInterface(CmdMenu) *ft;
};

typedef CmdMenuRef *CmdMenuId;

#endif
