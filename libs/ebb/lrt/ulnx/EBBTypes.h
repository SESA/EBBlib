#ifndef __ULNX_EBB_TYPES_H__
#define __ULNX_EBB_TYPES_H__

#include "pthread.h"

extern pthread_key_t ELKey;

static inline uval LRTEBBMyEL(void) {
/*   return 0; */
  return (uval)pthread_getspecific(ELKey);
}

#endif
