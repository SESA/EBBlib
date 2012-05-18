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

#include <inttypes.h>
#include <stddef.h>

#include <config.h>
#include <lrt/io.h>
#include <lrt/misc.h>
#include <lrt/assert.h>
#include <lrt/string.h>
#include <l0/lrt/event_loc.h>
#include <l0/lrt/trans.h>
#include <sync/barrier.h>

// Policy for managing global translation memory is software's responsibility
// We are choosing to use a simple partitioning scheme in which each event location
// manages its own local portion.  Local size and thus position are determined by
// simply dividing the total global size by the maximum allowable event locations

int sysTransValidate()
{
  int cores = lrt_num_event_loc();

  // there should be at least one page of translations per el
  if ((LRT_TRANS_TBLSIZE / cores) < LRT_TRANS_PAGESIZE) return 0;

  // we initialized trans memory to 0 so allocated must be a non-zero value
  if (LRT_TRANS_ID_FREE != 0) return 0;

  return 1;
}


// Size of a el's portion of the gtable
static inline size_t
lrt_trans_my_gmem_size(void)
{
  // round down so aligned on sizeof(EBBGTrans)
  return ((LRT_TRANS_TBLSIZE / lrt_num_event_loc())/
          sizeof(lrt_trans_gtrans))*sizeof(lrt_trans_gtrans);
}

//get number of GTrans in the table
static ptrdiff_t
lrt_trans_my_num_gtrans(void) {
  return lrt_trans_my_gmem_size() / sizeof(lrt_trans_gtrans);
}

//get number of LTrans in the table
static ptrdiff_t
lrt_trans_my_num_ltrans(void) {
  return LRT_TRANS_TBLSIZE / sizeof(lrt_trans_ltrans);
}

// This el's portion of the gtable
static inline lrt_trans_gtrans *
lrt_trans_my_gmem(void)
{
  lrt_trans_gtrans *gmem = lrt_trans_gmem();
  return gmem + lrt_my_event_loc() * lrt_trans_my_num_gtrans();
}

static void
trans_mark_core_used(lrt_trans_gtrans *gt, lrt_event_loc core)
{
  uint64_t mask = (uint64_t)1 << core;
  gt->corebv |= mask;
}

static int
trans_test_core_used(lrt_trans_gtrans *gt, int core)
{
  uint64_t mask = (uint64_t)1 << core;
  if (gt->corebv | mask) return 1;
  return 0;
}

void
lrt_trans_cache_obj(lrt_trans_ltrans *lt, lrt_trans_rep_ref ref) {
  lt->ref = ref;
  lrt_trans_gtrans *gt = lrt_trans_lt2gt(lt);
  trans_mark_core_used(gt, lrt_my_event_loc());
}

static void
lrt_trans_set_gtrans(lrt_trans_gtrans *gt, lrt_trans_miss_func mf,
                     lrt_trans_miss_arg arg) {
  gt->mf = mf;
  gt->arg = arg;
}

static void
lrt_trans_init_ltrans(lrt_trans_ltrans *lt) {
  lt->ref = &lt->rep;
  lt->rep = lrt_trans_def_rep;
}

static void
lrt_trans_init_ltable() {
  lrt_trans_ltrans *lt = lrt_trans_lmem();

  int len = lrt_trans_my_num_ltrans();

  for (int i = 0 ; i < len; i++) {
    lrt_trans_init_ltrans(lt + i);
  }
}

lrt_trans_id
lrt_trans_id_alloc()
{
  uintptr_t i, len;

  len = lrt_trans_my_num_gtrans();

  lrt_trans_gtrans *gt = lrt_trans_my_gmem(); //Get my piece of the global table
  for (i = 0; i < len; i++) {
    if (gt[i].alloc_status == LRT_TRANS_ID_FREE) {
      gt[i].alloc_status = LRT_TRANS_ID_ALLOCATED;
      return lrt_trans_gt2id(&gt[i]);
    }
  }
  return NULL;
}

void
lrt_trans_id_free(lrt_trans_id id)
{
  lrt_trans_gtrans *gt = lrt_trans_id2gt(id);
  gt->alloc_status = LRT_TRANS_ID_FREE;
}

static void
lrt_trans_invalidate_caches(lrt_trans_id id)
{
  lrt_event_loc el = lrt_my_event_loc();
  lrt_trans_gtrans *gt = lrt_trans_id2gt(id);

  do{
    if (trans_test_core_used(gt, el)) {
      lrt_trans_ltrans *lt = lrt_trans_id2rlt(el, id);
      LRT_Assert(lt != NULL);

      lrt_trans_init_ltrans(lt);
    }
    el = lrt_next_event_loc(el);
  } while (el != lrt_my_event_loc());
}

void
lrt_trans_id_bind(lrt_trans_id id, lrt_trans_miss_func mf,
                  lrt_trans_miss_arg arg)
{
  lrt_trans_gtrans *gt = lrt_trans_id2gt(id);
  lrt_trans_set_gtrans(gt, mf, arg);

  // invalidate all the local translation caches
  lrt_trans_invalidate_caches(id);
}


void
lrt_trans_init(void)
{
  LRT_Assert(sysTransValidate());
  // maximum bits in corebv in EBBTransStruct
  LRT_Assert(lrt_num_event_loc() <= 64);

  bzero(lrt_trans_my_gmem(), lrt_trans_my_gmem_size());
  lrt_trans_init_ltable();
}
