#ifndef __LRT_TRANS_H__
#error "should include l0/lrt/trans.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Copyright (C) 2011 by Project SESA, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
  lrt_trans_gtrans *gmem;
  lrt_trans_ltrans *lmem;
};

extern struct lrt_trans_mem_desc lrt_trans_mem;

static inline lrt_trans_gtrans *
lrt_trans_gmem(void)
{
  return lrt_trans_mem.gmem;
}

static inline lrt_trans_ltrans *
lrt_trans_lmem(void)
{
  uintptr_t picbase = lrt_my_event_loc() * LRT_TRANS_TBLSIZE /
    sizeof(lrt_trans_ltrans);
  return lrt_trans_mem.lmem + picbase;
}

static inline lrt_trans_id
lrt_trans_idbase(void)
{
  return &(lrt_trans_mem.lmem[0]);
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

extern void lrt_trans_init_specific(void);
extern void lrt_trans_preinit(int cores);
#ifdef __cplusplus
}
#endif

