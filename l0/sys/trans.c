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
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <l0/sys/trans-def.h>

// JA FIXME: Need to decided who really uses lrt_pic_myid versus myEL()

EBBGTrans * const ALLOCATED = (EBBGTrans *)-1;

// Policy for managing global translation memory is software's responsibility
// We are choosing to use a simple partitioning scheme in which each event location
// manages its own local portion.  Local size and thus position are determined by 
// simply dividing the total global size by the maximum allowable event locations

int sysTransValidate()
{
  uintptr_t psize = LRT_TRANS_TBLSIZE / LRT_PIC_MAX_PICS;

  // ensure that tables divide evenly among max pics
  if (psize * LRT_PIC_MAX_PICS != LRT_TRANS_TBLSIZE) return 0;

  // there should be at least one page of translations per pic
  if ((LRT_TRANS_TBLSIZE / LRT_PIC_MAX_PICS) < LRT_TRANS_PAGESIZE) return 0;
  
  // we initialized trans memory to 0 so allocated must be a non-zero value
  if (ALLOCATED == 0) return 0;

  // ensure sizes of trans structs all match
  if (sizeof(EBBGTrans) != sizeof(struct lrt_trans)) return 0;
  if (sizeof(EBBLTrans) != sizeof(struct lrt_trans)) return 0;

  return 1;
}


// Size of a pic's portion of the gtable
static inline uintptr_t
mygmem_size(void)
{
  return LRT_TRANS_TBLSIZE / LRT_PIC_MAX_PICS;
}

// This pic's portion of the gtable
static inline uintptr_t 
mygmem(void)
{
  uintptr_t ret = (uintptr_t)lrt_trans_gmem();
  return ret + (lrt_pic_myid * mygmem_size());
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
EBBCacheObj(EBBLTrans *lt, EBBRep *obj) {
  lt->obj = obj;
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

void
TransEBBIdBind(EBBId id, EBBMissFunc mf, EBBMissArg arg) {
  EBBGTrans *gt = id2gt(id);
  gt->mf = mf;
  gt->arg = arg;
}

void
TransEBBIdUnBind(EBBId id, EBBMissFunc *mf, EBBMissArg *arg) {
  EBB_LRT_printf("%s: NYI\n", __func__);
}

// at this point translation hardware has been initialized
// software must setup the memory and any if its basic 
// managment up
void
trans_init(void)
{
  EBBAssert(sysTransValidate());
  __builtin_bzero((void *)mygmem(), mygmem_size());
  initLTable();
}
