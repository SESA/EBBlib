#ifndef __LRT_ULNX_TRANS_H_
#define __LRT_ULNX_TRANS_H__

// this has been sized for the future use
// doing multi-node translations
// eg.  some bits used as a key map to a node
//      some bits used as a key map to a 
//      lrt_trans pointer
typedef uint64_t lrt_transid;

struct lrt_trans {
  uint64_t vals[4];
};

enum { LRT_TRANS_LG2_PAGESIZE=12, LRT_TRANS_LG2_NUMPAGES=4 };
enum { LRT_TRANS_PAGESIZE=(1<<LRT_TRANS_LG2_PAGESIZE),   // 4096
       LRT_TRANS_NUMPAGES=(1<<LRT_TRANS_LG2_NUMPAGES) }; // 16
enum { LRT_TRANS_NUMIDS_PERPAGE=LRT_TRANS_PAGESIZE/sizeof(struct lrt_trans) };
enum { LRT_TRANS_LG2_TBLSIZE=(LRT_TRANS_LG2_PAGESIZE + 
			      LRT_TRANS_LG2_NUMPAGES) };
enum { LRT_TRANS_TBLSIZE=(1<<LRT_TRANS_LG2_TBLSIZE) };

struct TransMemDesc {
  uint8_t GMem [LRT_TRANS_TBLSIZE];
  uint8_t LMem [LRT_TRANS_TBLSIZE * LRT_PIC_MAX_PICS];
};

extern struct TransMemDesc TransMem;

static inline int 
lrt_trans_valid(void)
{
  if ((sizeof(struct lrt_trans) * LRT_TRANS_NUMIDS_PERPAGE) == 
      LRT_TRANS_PAGESIZE) return 1;
  return 0;
}

static inline uintptr_t 
lrt_trans_gmem(void)
{
  return (uintptr_t)TransMem.GMem;
}

static inline uintptr_t
lrt_trans_lmem(void)
{
  uintptr_t picbase = lrt_pic_myid * LRT_TRANS_TBLSIZE;
  return (uintptr_t)(&(TransMem.LMem[picbase]));
}

static inline uintptr_t
lrt_trans_offset(void *t) 
{
 return (uintptr_t)t & ((1 << (unsigned)LRT_TRANS_LG2_TBLSIZE)-1);    
}

static inline uintptr_t
lrt_trans_idbase(void)
{
  return (uintptr_t)&(TransMem.LMem[0]);
}

static inline struct lrt_trans *
lrt_trans_id2lt(lrt_transid i)
{
  return (struct lrt_trans *)(lrt_trans_lmem() + lrt_trans_offset((void *)i));
}
    
static inline lrt_transid 
lrt_trans_lt2id(struct lrt_trans *t)
{
  return (lrt_transid)(lrt_trans_idbase() + lrt_trans_offset(t));
}

static inline struct lrt_trans *
lrt_trans_id2gt(lrt_transid i)
{
  return (struct lrt_trans *)(lrt_trans_gmem() + lrt_trans_offset((void *)i));
}

static inline lrt_transid
lrt_trans_gt2id(struct lrt_trans *t)
{
  return (lrt_transid)(lrt_trans_idbase() + lrt_trans_offset(t));
}

static inline struct lrt_trans *
lrt_trans_gt2lt(struct lrt_trans *gt)
{
  return (struct lrt_trans *)(lrt_trans_lmem() + lrt_trans_offset(gt)); 
}

static inline struct lrt_trans *
lrt_trans_lt2gt(struct lrt_trans *lt)
{
  return (struct lrt_trans *)(lrt_trans_gmem() + lrt_trans_offset(lt));
}

void lrt_trans_init(void);

#endif
