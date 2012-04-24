#ifndef L0_LRT_BARE_ARCH_AMD64_TRANS_H
#define L0_LRT_BARE_ARCH_AMD64_TRANS_H

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
#include <l0/lrt/bare/arch/amd64/pic.h>

#define LRT_TRANS_PAGESIZE (PAGE_SIZE) //4 K
#define LRT_TRANS_PAGES (512)
#define LRT_TRANS_TBLSIZE (LRT_TRANS_PAGESIZE * LRT_TRANS_PAGES) 

//These should be virtual addresses
#define GMem (0xFFFFFFFF00000000) //upper 4GB of memory
#define LMem (0xFFFFFFFE00000000) //next 4GB of memory

extern void lrt_trans_init(void);

static inline void *
lrt_trans_gmem(void)
{
  return (void *)GMem;
}

static inline void *
lrt_trans_lmem(void)
{
  return (void *)LMem;
}

static inline uintptr_t
lrt_trans_offset(uintptr_t base, uintptr_t t) 
{
  return (uintptr_t)(t - base);
}

static inline uintptr_t
lrt_trans_idbase(void)
{
  return (uintptr_t)LMem;
}

static inline struct lrt_trans *
lrt_trans_id2lt(uintptr_t i)
{
  return (struct lrt_trans *)i;
}

static inline uintptr_t
lrt_trans_lt2id(struct lrt_trans *t)
{
  return (uintptr_t)(t);
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
  return lrt_trans_id2lt(lrt_trans_gt2id(gt));
}

static inline struct lrt_trans *
lrt_trans_lt2gt(struct lrt_trans *lt)
{
  return lrt_trans_id2gt(lrt_trans_lt2id(lt));
}

#endif
