#ifndef __LRT_ULNX_TRANS_H_
#define __LRT_ULNX_TRANS_H__

typedef void * lrt_transid;

struct lrt_trans {
  uintptr_t vals[4];
};

enum { LRT_TRANS_LG2_PAGESIZE=12, LRT_TRANS_LG2_NUMPAGES=4 };
enum { LRT_TRANS_PAGESIZE=(1<<LRT_TRANS_LG2_PAGESIZE),   // 4096
       LRT_TRANS_NUMPAGES=(1<<LRT_TRANS_LG2_NUMPAGES) }; // 16
enum { LRT_TRANS_NUMIDS_PERPAGE=LRT_TRANS_PAGESIZE/sizeof(struct lrt_trans) };
enum { LRT_TRANS_LG2_TBLSIZE=(LRT_TRANS_LG2_PAGESIZE + LRT_TRANS_LG2_NUMPAGES) };
enum { LRT_TRANS_TBLSIZE=(1<<LRT_TRANS_LG2_TBLSIZE) };

struct TransMemDesc {
  uint8_t GMem [LRT_TRANS_TBLSIZE];
  uint8_t LMem [LRT_TRANS_TBLSIZE * LRT_PIC_MAX_PICS];
};

extern struct TransMemDesc TransMem;

static inline int lrt_trans_valid(void)
{
  if ((sizeof(struct lrt_trans) * LRT_TRANS_NUMIDS_PERPAGE) == 
      LRT_TRANS_PAGESIZE) return 1;
  return 0;
}

static inline struct lrt_trans *
lrt_trans_id2lt(lrt_transid i)
{
  uintptr_t offset = (uintptr_t)i & ((1 << (unsigned)LRT_TRANS_LG2_TBLSIZE)-1);
  uintptr_t picbase = lrt_pic_myid * LRT_TRANS_TBLSIZE;
  return (struct lrt_trans *)(&(TransMem.LMem[picbase]) + offset);
}

static inline lrt_transid 
lrt_trans_lt2id(struct lrt_trans *t)
{
  uintptr_t offset = (uintptr_t)t & ((1 << (unsigned)LRT_TRANS_LG2_TBLSIZE)-1);
  return (lrt_transid)(&(TransMem.LMem[0]) + offset);
}

static inline struct lrt_trans *
lrt_trans_id2gt(lrt_transid i)
{
  uintptr_t offset = (uintptr_t)i & ((1 << (unsigned)LRT_TRANS_LG2_TBLSIZE)-1);
  return (struct lrt_trans *)(TransMem.GMem + offset);
}

static inline lrt_transid
lrt_trans_gt2id(struct lrt_trans *t)
{
  uintptr_t offset = (uintptr_t)t & ((1 << (unsigned)LRT_TRANS_LG2_TBLSIZE)-1);
  return (lrt_transid)(TransMem.GMem + offset);
}

void lrt_trans_init(void);

#endif
