#ifndef __LRT_ULNX_TRANS_H_
#define __LRT_ULNX_TRANS_H__

#include <stdint.h>
#include <l0/lrt/event_loc.h>

#define LRT_TRANS_LG2_PAGESIZE (12)
#define LRT_TRANS_LG2_NUMPAGES (8)
#define LRT_TRANS_PAGESIZE (1 << LRT_TRANS_LG2_PAGESIZE) // 4096
#define LRT_TRANS_NUMPAGES (1 << LRT_TRANS_LG2_NUMPAGES) // 256
#define LRT_TRANS_LG2_TBLSIZE \
  (LRT_TRANS_LG2_PAGESIZE + LRT_TRANS_LG2_NUMPAGES)
#define LRT_TRANS_TBLSIZE (1 << LRT_TRANS_LG2_TBLSIZE)

struct TransMemDesc {
  uint8_t *GMem; 
  uint8_t *LMem; 
};

extern struct TransMemDesc TransMem;

static inline void * 
lrt_trans_gmem(void)
{
  return TransMem.GMem;
}

static inline void *
lrt_trans_lmem(void)
{
  uintptr_t picbase = lrt_my_event_loc() * LRT_TRANS_TBLSIZE;
  return (&(TransMem.LMem[picbase]));
}

static inline uintptr_t
lrt_trans_offset(uintptr_t base, uintptr_t t) 
{
  return (uintptr_t)(t - base);
}

static inline uintptr_t
lrt_trans_idbase(void)
{
  return (uintptr_t)&(TransMem.LMem[0]);
}

static inline struct lrt_trans *
lrt_trans_id2lt(uintptr_t i)
{
  return (struct lrt_trans *)(((uintptr_t)lrt_trans_lmem()) + 
			      lrt_trans_offset(lrt_trans_idbase(), i));
}
    
static inline uintptr_t 
lrt_trans_lt2id(struct lrt_trans *t)
{
  return (uintptr_t)(lrt_trans_idbase() + 
		     lrt_trans_offset((uintptr_t)lrt_trans_lmem(), (uintptr_t)t));
}

static inline struct lrt_trans *
lrt_trans_id2gt(uintptr_t i)
{
  return (struct lrt_trans *)(((uintptr_t)lrt_trans_gmem()) + 
			      lrt_trans_offset(lrt_trans_idbase(), i));
}

static inline uintptr_t
lrt_trans_gt2id(struct lrt_trans *t)
{
  return (uintptr_t)(lrt_trans_idbase() + 
		     lrt_trans_offset((uintptr_t)lrt_trans_gmem(),
				      (uintptr_t)t));
}

static inline struct lrt_trans *
lrt_trans_gt2lt(struct lrt_trans *gt)
{
  return (struct lrt_trans *)(((uintptr_t)lrt_trans_lmem()) + 
			      lrt_trans_offset((uintptr_t)lrt_trans_gmem(), 
					       (uintptr_t)gt)); 
}

static inline struct lrt_trans *
lrt_trans_lt2gt(struct lrt_trans *lt)
{
  return (struct lrt_trans *)(((uintptr_t)lrt_trans_gmem()) + 
			      lrt_trans_offset((uintptr_t)lrt_trans_lmem(), 
					       (uintptr_t)lt));
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
