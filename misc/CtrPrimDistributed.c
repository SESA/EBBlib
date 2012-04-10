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

#include <stdint.h>
#include <stddef.h>

#include <lrt/assert.h>
#include <l0/cobj/cobj.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <misc/Ctr.h>
#include <misc/CtrPrimDistributed.h>

CObject(EBBCtrPrimDistributed) {
  COBJ_EBBFuncTbl(EBBCtr);

  uintptr_t localValue;
  CObjEBBRootMultiRef theRoot;
};

extern CObjInterface(EBBCtr) EBBCtrPrimDistributed_ftable;

static inline void
EBBCtrPrimDistributedSetFT(EBBCtrPrimDistributedRef o)
{
  o->ft = &EBBCtrPrimDistributed_ftable;
}

typedef EBBCtrPrimDistributedRef *EBBCtrPrimDistributedId;

static EBBRC 
EBBCtrPrimDistributed_inc(EBBCtrRef _self) 
{
  EBBCtrPrimDistributedRef self = (EBBCtrPrimDistributedRef)_self;
  //gcc built-in atomics
  __sync_fetch_and_add(&self->localValue,1);
  return EBBRC_OK;
}

static EBBRC 
EBBCtrPrimDistributed_dec(EBBCtrRef _self) 
{
  EBBCtrPrimDistributedRef self = (EBBCtrPrimDistributedRef)_self;
  //gcc builtin atomics
  __sync_fetch_and_sub(&self->localValue,1);
  return EBBRC_OK;
}

static EBBRC
EBBCtrPrimDistributed_val(EBBCtrRef _self, uintptr_t *v)
{
  EBBCtrPrimDistributedRef self = (EBBCtrPrimDistributedRef)_self;
  uintptr_t val = 0;
  RepListNode *node;
  EBBRep *rep = NULL;
  for (node = self->theRoot->ft->nextRep(self->theRoot, 0, &rep);
       node; 
       node = self->theRoot->ft->nextRep(self->theRoot, node, &rep)) {
    val += ((EBBCtrPrimDistributedRef)rep)->localValue;
  }
  *v = val;
  return EBBRC_OK;
}

static EBBRep *
EBBCtrPrimDistributed_createRep(CObjEBBRootMultiRef _self) {
  EBBCtrPrimDistributedRef repRef;
  EBBPrimMalloc(sizeof(EBBCtrPrimDistributed), &repRef, EBB_MEM_DEFAULT);
  EBBCtrPrimDistributedSetFT(repRef);
  repRef->theRoot = _self;
  repRef->localValue = 0;
  return (EBBRep *)repRef;
}

CObjInterface(EBBCtr) EBBCtrPrimDistributed_ftable = {
  .inc = EBBCtrPrimDistributed_inc, 
  .dec = EBBCtrPrimDistributed_dec, 
  .val = EBBCtrPrimDistributed_val
};

EBBRC
EBBCtrPrimDistributedCreate(EBBCtrId *id)
{
  EBBRC rc;
  CObjEBBRootMultiImpRef rootRef;

  rc = CObjEBBRootMultiImpCreate(&rootRef, 
				 EBBCtrPrimDistributed_createRep);
  EBBRCAssert(rc);

  rc = EBBAllocPrimId((EBBId *)id);
  EBBRCAssert(rc);

  rc = CObjEBBBind((EBBId)*id, rootRef);
  EBBRCAssert(rc);

  return EBBRC_OK;
}
