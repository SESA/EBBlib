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

#include <config.h>
#include <inttypes.h>
#include <lrt/io.h>
#include <lrt/assert.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <l0/sys/trans-def.h>

const EBBGTrans *ALLOCATED = (EBBGTrans *)-1;

static inline 
EBBId
gt2id(EBBGTrans *gt) {
  return (EBBId)lrt_trans_gt2id((struct lrt_trans *)gt);
}

static inline
EBBGTrans *
id2gt(EBBId id) {
  return (EBBGTrans *)lrt_trans_id2gt((lrt_transid) id);
}

void
EBBCacheObj(EBBLTrans *lt, EBBRep *obj) {
  lt->obj = obj;
}

//get number of GTrans in the table
uintptr_t
myGTableSize() {
  return lrt_trans_gmem_size() / sizeof(EBBGTrans);
}

//get my portion of the gtable
EBBGTrans *
myGTable() {
  EBBGTrans *gt = (EBBGTrans *)lrt_trans_gmem();
  return gt + (lrt_pic_myid * myGTableSize());
}

//get number of LTrans in the table
uintptr_t
myLTableSize() {
  return LRT_TRANS_TBLSIZE / sizeof(EBBLTrans);
}

//get my portion of the gtable
EBBLTrans *
myLTable() {
  return (EBBLTrans *)lrt_trans_lmem();
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

  len = myGTableSize();
  
  for (i = 0; i < len; i++) {
    EBBInitGTrans(&gt[i], mf, arg);
  }
}

void 
initLTable() {
  EBBLTrans *lt = myLTable();
  uintptr_t i, len;

  len = myLTableSize();

  for (i = 0 ; i < len; i++) {
    EBBInitLTrans(&lt[i]);
  }
}

EBBId
EBBIdAlloc() {
  EBBGTrans *gt;
  uintptr_t i, len;

  len = myGTableSize();
  
  gt = myGTable(); //Get my piece of the global table
  for (i = 0; i < len; i++) {
    if (gt[i].free != ALLOCATED) {
      gt[i].free = (EBBGTrans *)ALLOCATED;
      return gt2id(&gt[i]);
    }
  }
  return NULL; 
}

void
EBBIdFree(EBBId id) {
  EBBGTrans *gt = id2gt(id);
  gt->free = (EBBGTrans *)ALLOCATED;
}

void
EBBIdBind(EBBId id, EBBMissFunc mf, EBBMissArg arg) {
  EBBGTrans *gt = id2gt(id);
  gt->mf = mf;
  gt->arg = arg;
}

void
EBBIdUnBind(EBBId id, EBBMissFunc *mf, EBBMissArg *arg) {
  EBB_LRT_printf("%s: NYI\n", __func__);
  EBBAssert(0);
}
