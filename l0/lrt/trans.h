#ifndef __LRT_TRANS_H__
#define __LRT_TRANS_H__
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

#include <stdint.h>

#include <l0/lrt/event_loc.h>
#include <lrt/assert.h>

typedef intptr_t lrt_trans_rc;
#define LRT_TRANS_RC_SUCCESS(rc) (rc >= 0)
struct lrt_trans_rep_s;
typedef struct lrt_trans_rep_s *lrt_trans_rep_ref;
typedef lrt_trans_rc (*lrt_trans_func) (lrt_trans_rep_ref);
typedef struct lrt_trans_rep_s {
  lrt_trans_func *ft;
} lrt_trans_rep;
typedef uint8_t lrt_trans_func_num;
typedef uintptr_t lrt_trans_miss_arg;

struct lrt_trans_s;
typedef struct lrt_trans_s lrt_trans_ltrans;
typedef struct lrt_trans_s lrt_trans_gtrans;
typedef lrt_trans_ltrans *lrt_trans_id;

//first argument is the address of the rep that will be executed
//second argument is the local translation entry so a rep can be
//installed
typedef lrt_trans_rc (*lrt_trans_miss_func) (lrt_trans_rep_ref *,
                                             lrt_trans_ltrans *,
                                             lrt_trans_func_num,
                                             lrt_trans_miss_arg);

enum lrt_trans_id_alloc_status {
  LRT_TRANS_ID_FREE = 0,
  LRT_TRANS_ID_ALLOCATED = 1
};

struct lrt_trans_s {
  union {
    uintptr_t v1;
    lrt_trans_rep_ref ref; //as a local entry
    lrt_trans_miss_func mf; //as a global entry
  };
  union {
    uintptr_t v2;
    lrt_trans_rep rep; //as a local entry (by default)
    lrt_trans_miss_arg arg; //as a global entry
  };
  union {
    uintptr_t v3;
    enum lrt_trans_id_alloc_status alloc_status;
  };
  union {
    uintptr_t v4;
    uint64_t corebv; //bit vector of cores where object is cached in
                     //translation table, note, object may have been
                     //accessed in larger set of nodes and translated
                     //id may be on stack in other nodes
  };
};

// prototypes for the common set of functions all platforms must provide
static inline lrt_trans_ltrans *lrt_trans_id2lt(lrt_trans_id);
static inline lrt_trans_id lrt_trans_lt2id(lrt_trans_ltrans *);
static inline lrt_trans_gtrans *lrt_trans_id2gt(lrt_trans_id);
static inline lrt_trans_id lrt_trans_gt2id(lrt_trans_gtrans *);
static inline lrt_trans_ltrans *lrt_trans_gt2lt(lrt_trans_gtrans *);
static inline lrt_trans_gtrans *lrt_trans_lt2gt(lrt_trans_ltrans *);
extern lrt_trans_ltrans *lrt_trans_id2rlt(lrt_event_loc el,
                                          lrt_trans_id objid);
extern void lrt_trans_init(void);
extern void lrt_trans_cache_obj(lrt_trans_ltrans *, lrt_trans_rep_ref);
extern lrt_trans_id lrt_trans_id_alloc(void);
extern void lrt_trans_id_free(lrt_trans_id id);
extern void lrt_trans_id_bind(lrt_trans_id id, lrt_trans_miss_func mf,
			      lrt_trans_miss_arg arg); 
static inline lrt_trans_rep_ref
lrt_trans_id_dref(lrt_trans_id id) {
  lrt_trans_ltrans *lt;
  lt = lrt_trans_id2lt(id);
  return lt->ref;
}

extern lrt_trans_rep lrt_trans_def_rep;

#ifdef LRT_ULNX
#include <l0/lrt/ulnx/trans.h>
#elif LRT_BARE
#include <l0/lrt/bare/trans.h>
#endif

#define LRT_TRANS_NUMIDS_PERPAGE \
  (LRT_TRANS_PAGESIZE / sizeof(lrt_trans_gtrans))

STATIC_ASSERT(sizeof(lrt_trans_gtrans) * LRT_TRANS_NUMIDS_PERPAGE ==
               LRT_TRANS_PAGESIZE, "translation table size mismatch!");

#ifdef __cplusplus
}
#endif
#endif
