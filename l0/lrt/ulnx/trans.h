#ifndef __LRT_ULNX_TRANS_H_
#define __LRT_ULNX_TRANS_H__

#include <stddef.h>
#include <stdint.h>

#include <l0/lrt/event_loc.h>

#define LRT_TRANS_LG2_PAGESIZE (12)
#define LRT_TRANS_LG2_NUMPAGES (8)
#define LRT_TRANS_PAGESIZE (1 << LRT_TRANS_LG2_PAGESIZE) // 4096
#define LRT_TRANS_NUMPAGES (1 << LRT_TRANS_LG2_NUMPAGES) // 256
#define LRT_TRANS_LG2_TBLSIZE \
  (LRT_TRANS_LG2_PAGESIZE + LRT_TRANS_LG2_NUMPAGES)
#define LRT_TRANS_TBLSIZE (1 << LRT_TRANS_LG2_TBLSIZE)

struct lrt_trans_mem_desc {
  lrt_trans_gtrans *GMem;
  lrt_trans_ltrans *LMem;
};

extern struct lrt_trans_mem_desc lrt_trans_mem;

static inline lrt_trans_gtrans *
lrt_trans_gmem(void)
{
  return lrt_trans_mem.GMem;
}

static inline lrt_trans_ltrans *
lrt_trans_lmem(void)
{
  uintptr_t picbase = lrt_my_event_loc() * LRT_TRANS_TBLSIZE /
    sizeof(lrt_trans_ltrans);
  return lrt_trans_mem.LMem + picbase;
}

static inline lrt_trans_id
lrt_trans_idbase(void)
{
  return &(lrt_trans_mem.LMem[0]);
}

static inline lrt_trans_ltrans *
lrt_trans_id2lt(lrt_trans_id id)
{
  lrt_trans_ltrans *lmem = lrt_trans_lmem();
  ptrdiff_t index = id - lrt_trans_idbase();
  return lmem + index;
}

static inline lrt_trans_id
lrt_trans_lt2id(lrt_trans_ltrans *lt)
{
  lrt_trans_ltrans *lmem = lrt_trans_lmem();
  ptrdiff_t index = lt - lmem;
  return lrt_trans_idbase() + index;
}

static inline lrt_trans_gtrans *
lrt_trans_id2gt(lrt_trans_id id)
{
  lrt_trans_gtrans *gmem = lrt_trans_gmem();
  ptrdiff_t index = id - lrt_trans_idbase();
  return gmem + index;
}

static inline lrt_trans_id
lrt_trans_gt2id(lrt_trans_gtrans *gt)
{
  lrt_trans_gtrans *gmem = lrt_trans_gmem();
  ptrdiff_t index = gt - gmem;
  return lrt_trans_idbase() + index;
}

static inline lrt_trans_ltrans *
lrt_trans_gt2lt(lrt_trans_gtrans *gt)
{
  lrt_trans_gtrans *gmem = lrt_trans_gmem();
  ptrdiff_t index = gt - gmem;
  return lrt_trans_lmem() + index;
}

static inline lrt_trans_gtrans *
lrt_trans_lt2gt(lrt_trans_ltrans *lt)
{
  lrt_trans_ltrans *lmem = lrt_trans_lmem();
  ptrdiff_t index = lt - lmem;
  return lrt_trans_gmem() +  index;
}

#ifdef __cplusplus
extern "C" {
#endif
extern void lrt_trans_init_specific(void);
extern void lrt_trans_preinit(int cores);
#ifdef __cplusplus
}
#endif

#endif
