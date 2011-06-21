#ifndef __MYOBJ_H__
#define __MYOBJ_H__

#include "cobj.h"
#include "counter.h"
#include "file.h"

CObjInterface(MyObj) {
  CObjImplements(Counter);
  CObjImplements(File);
  #include "myobj.iface"
};

CObject(MyObj) {
  CObjInterface(MyObj) *ftable;
  uval val;
  uval fd;
  uval offset;
  uval ocnt, ccnt,rcnt, wcnt;
};

extern CObjInterface(MyObj) MyObj_ftable;

#endif
