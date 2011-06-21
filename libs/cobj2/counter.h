#ifndef __COUNTER_H__
#define __COUNTER_H__

#include "cobj.h"

CObjInterface(Counter) {
  #include "counter.iface"
};

CObject(Counter) {
  CObjInterface(Counter) *ftable;
};

#endif
