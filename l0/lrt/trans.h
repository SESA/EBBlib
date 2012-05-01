#ifndef __LRT_TRANS_H__
#define __LRT_TRANS_H__

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

#include <l0/lrt/event_loc.h>

//forward declaration for the lrt specific headers
struct lrt_trans;

// prototypes for the common set of functions all platforms must provide
static inline struct lrt_trans *lrt_trans_id2lt(uintptr_t i);
static inline uintptr_t lrt_trans_lt2id(struct lrt_trans *t);
static inline struct lrt_trans *lrt_trans_id2gt(uintptr_t i);
static inline uintptr_t lrt_trans_gt2id(struct lrt_trans *t);
static inline struct lrt_trans *lrt_trans_gt2lt(struct lrt_trans *gt);
static inline struct lrt_trans *lrt_trans_lt2gt(struct lrt_trans *lt);

// returns the pointer to a remote local translation entry for a object id
extern struct lrt_trans *lrt_trans_id2rlt(lrt_event_loc el, uintptr_t objid);

// real implementations come from these files
#ifdef LRT_ULNX
#include <l0/lrt/ulnx/trans.h>
#elif LRT_BARE
#include <l0/lrt/bare/trans.h>
#endif

#include <lrt/assert.h>

// this has been sized for the future use
// doing multi-node translations
// eg.  some bits used as a key map to a node
//      some bits used as a key map to a 
//      lrt_trans pointer
struct lrt_trans {
  uint64_t vals[4];
};

#define LRT_TRANS_NUMIDS_PERPAGE \
  (LRT_TRANS_PAGESIZE / sizeof(struct lrt_trans))

STATIC_ASSERT(sizeof(struct lrt_trans) * LRT_TRANS_NUMIDS_PERPAGE == 
	       LRT_TRANS_PAGESIZE, "translation table size mismatch!");

#endif
