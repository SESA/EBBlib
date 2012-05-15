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
#include <l0/lrt/trans.h>
#include <l0/lrt/trans-def.h>
#include <sync/barrier.h>

EBBGTrans * const ALLOCATED = (EBBGTrans *)-1;

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
  if (ALLOCATED == 0) return 0;

  // ensure sizes of trans structs all match
  if (sizeof(EBBGTrans) != sizeof(struct lrt_trans)) return 0;
  if (sizeof(EBBLTrans) != sizeof(struct lrt_trans)) return 0;

  return 1;
}


// Size of a el's portion of the gtable
static inline uintptr_t
mygmem_size(void)
{
  // round up so aligned on sizeof(EBBGTrans)
  return ((LRT_TRANS_TBLSIZE / lrt_num_event_loc())/
          sizeof(EBBGTrans))*sizeof(EBBGTrans);
}


// This el's portion of the gtable
static inline uintptr_t
mygmem(void)
{
  uintptr_t ret = (uintptr_t)lrt_trans_gmem();
  return ret + (lrt_my_event_loc() * mygmem_size());
}

static inline
EBBId
gt2id(EBBGTrans *gt) {
  return (EBBId)lrt_trans_gt2id((struct lrt_trans *)gt);
}

static inline
EBBGTrans *
id2gt(EBBId id) {
  return (EBBGTrans *)lrt_trans_id2gt((uintptr_t)id);
}


void
trans_mark_core_used(EBBGTrans *gt, lrt_event_loc core)
{
  uint64_t mask = (uint64_t)1 << core;
  gt->corebv |= mask;
}

int
trans_test_core_used(EBBGTrans *gt, int core)
{
  uint64_t mask = (uint64_t)1 << core;
  if (gt->corebv | mask) return 1;
  return 0;
}

void
EBBCacheObj(EBBLTrans *lt, EBBRep *obj) {
  EBBGTrans *gt = (EBBGTrans *)lrt_trans_lt2gt((struct lrt_trans *)lt);
  lt->obj = obj;
  trans_mark_core_used(gt, lrt_my_event_loc());
}

//get number of GTrans in the table
uintptr_t
myNumGTrans() {
  return mygmem_size() / sizeof(EBBGTrans);
}

//get my portion of the gtable
EBBGTrans *
myGTable() {
  return (EBBGTrans *)mygmem();
}

//get number of LTrans in the table
uintptr_t
myNumLTrans() {
  return LRT_TRANS_TBLSIZE / sizeof(EBBLTrans);
}

void
EBBInitGTrans(EBBGTrans *gt, EBBMissFunc mf, EBBMissArg arg) {
  gt->mf = mf;
  gt->arg = arg;
}

void
EBBInitLTrans(EBBLTrans *lt) {
  lt->obj = &lt->ftable;
  lt->ftable = EBBDefFT;
}

void
initGTable(EBBMissFunc mf, EBBMissArg arg) {
  EBBGTrans *gt = myGTable();
  uintptr_t i, len;

  len = myNumGTrans();

  // We expect that all the global trans memory had been zero'd
  // and that any early allocations have set the free field
  // to ALLOCATED (a non-zero value)
  for (i = 0; i < len; i++) {
    if (gt[i].free != ALLOCATED) EBBInitGTrans(&gt[i], mf, arg);
  }
}

void
initLTable() {
  EBBLTrans *lt = (EBBLTrans *)lrt_trans_lmem();
  uintptr_t i, len;

  len = myNumLTrans();

  for (i = 0 ; i < len; i++) {
    EBBInitLTrans(&lt[i]);
  }
}

EBBId
TransEBBIdAlloc() {
  EBBGTrans *gt;
  uintptr_t i, len;

  len = myNumGTrans();

  gt = myGTable(); //Get my piece of the global table
  for (i = 0; i < len; i++) {
    if (gt[i].free != ALLOCATED) {
      gt[i].free = ALLOCATED;
      return gt2id(&gt[i]);
    }
  }
  return NULL;
}

void
TransEBBIdFree(EBBId id) {
  EBBGTrans *gt = id2gt(id);
  gt->free = ALLOCATED;
}

static void
TransEBBIdInvalidateCaches(EBBId id)
{
  lrt_event_loc el = lrt_my_event_loc();
  EBBGTrans *gt = id2gt(id);

  do{
    if (trans_test_core_used(gt, el)) {
      EBBLTrans *lt = (EBBLTrans *)lrt_trans_id2rlt(el, (uintptr_t)id);
      LRT_Assert(lt != NULL);

      EBBInitLTrans(lt);
    }
    el = lrt_next_event_loc(el);
  } while (el != lrt_my_event_loc());
}

void
TransEBBIdBind(EBBId id, EBBMissFunc mf, EBBMissArg arg) {
  EBBGTrans *gt = id2gt(id);
  gt->mf = mf;
  gt->arg = arg;

  // invalidate all the local translation caches
  TransEBBIdInvalidateCaches(id);
}


void
lrt_trans_init(void)
{
  LRT_Assert(sysTransValidate());
  // maximum bits in corebv in EBBTransStruct
  LRT_Assert(lrt_num_event_loc() <= 64);

  bzero((void *)mygmem(), mygmem_size());
  initLTable();

}
