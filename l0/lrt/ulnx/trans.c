#include <stdint.h>
#include <l0/lrt/ulnx/trans.h>

#include <assert.h>

struct TransMemDesc TransMem;

// get the base address of a remote local memory translation table
static void *
lrt_trans_lmemr(lrt_event_loc el)
{
  uintptr_t elbase = el * LRT_TRANS_TBLSIZE;
  return (&(TransMem.LMem[elbase]));
}

// returns the pointer to a remote local translation entry for a object id
struct lrt_trans *lrt_trans_id2rlt(lrt_event_loc el, uintptr_t oid)
{
  return (struct lrt_trans *)(((uintptr_t)lrt_trans_lmemr(el)) + 
			      lrt_trans_offset(lrt_trans_idbase(), oid));
}

void
lrt_trans_init(void)
{
}
