#include <stdint.h>
#include <l0/lrt/ulnx/pic.h>
#include <l0/lrt/ulnx/trans.h>

#include <assert.h>

struct TransMemDesc TransMem;

void
lrt_trans_init(void)
{
  assert(lrt_trans_valid());
}
