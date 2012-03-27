#include <lrt/bare/io.h>

void *
memset (void *ptr, int value, size_t num)
{
  unsigned char c = (unsigned char) value;
  unsigned char *cptr = (unsigned char *)ptr;
  for (int i = 0; i < num; i++) {
    cptr[i] = c;
  }
  return ptr;
}
