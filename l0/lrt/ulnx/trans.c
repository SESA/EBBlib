#include <stdint.h>
#include <l0/lrt/ulnx/pic.h>
#include <l0/lrt/ulnx/trans.h>

#include <assert.h>

struct TransMemDesc TransMem;

// get the base address of a remote local memory translation table
static void *
lrt_trans_lmemr(lrt_pic_id picid)
{
  uintptr_t picbase = picid * LRT_TRANS_TBLSIZE;
  return (&(TransMem.LMem[picbase]));
}

// returns the pointer to a remote local translation entry for a object id
struct lrt_trans *lrt_trans_id2rlt(lrt_pic_id picid, uintptr_t oid)
{
  return (struct lrt_trans *)(((uintptr_t)lrt_trans_lmemr(picid)) + 
			      lrt_trans_offset(lrt_trans_idbase(), oid));
}

void
lrt_trans_init(void)
{
}
