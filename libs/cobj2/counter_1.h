#ifndef __COUNTER_1_H__
#define __COUNTER_1_H__

#include "cobj.h"
#include "counter.h"

CObject(Counter_1) {
  CObjInterface(Counter) *ftable;
  uval value;
};

extern CObjInterface(Counter) Counter_1_ftable;

#endif
