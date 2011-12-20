#include <stdint.h>
#include "premalloc.h"

extern uintptr_t kend; /* in link.ld. located just after the kernel image. */
uintptr_t *nextptr = &kend;

void *premalloc(uintptr_t size, uintptr_t align) {
  uintptr_t skew; /* how far past the last aligned address we are */
  void *ret;
  skew = (uintptr_t)nextptr % align;
  if(skew)
    nextptr = (uintptr_t*)((align - skew) + (uintptr_t)nextptr);
  ret = nextptr;
  nextptr = (uintptr_t*)((uintptr_t)nextptr + size);
  return ret;
}
