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

#include <stdint.h>

#include <arch/amd64/paging.h>

#define LRT_TRANS_PAGESIZE (PAGE_SIZE) //4 K
#define LRT_TRANS_PAGES (512)
#define LRT_TRANS_TBLSIZE (LRT_TRANS_PAGESIZE * LRT_TRANS_PAGES)

//These should be virtual addresses
#define LRT_TRANS_GMEM (0xFFFFFFFF00000000) //upper 4GB of memory
#define LRT_TRANS_LMEM (0xFFFFFFFE00000000) //next 4GB of memory

extern void lrt_trans_init(void);

static inline lrt_trans_gtrans *
lrt_trans_gmem(void)
{
  return (lrt_trans_gtrans *)LRT_TRANS_GMEM;
}

static inline lrt_trans_ltrans *
lrt_trans_lmem(void)
{
  return (lrt_trans_ltrans *)LRT_TRANS_LMEM;
}

static inline lrt_trans_id
lrt_trans_idbase(void)
{
  return lrt_trans_lmem();
}

static inline lrt_trans_ltrans *
lrt_trans_id2lt(lrt_trans_id id)
{
  return id;
}

static inline lrt_trans_id
lrt_trans_lt2id(lrt_trans_ltrans *lt)
{
  return lt;
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
  return lrt_trans_gmem() + index;
}
