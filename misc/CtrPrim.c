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
#include <lrt/io.h>
#include <l0/cobj/cobj.h>
#include <l0/lrt/trans.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <misc/Ctr.h>
#include <misc/CtrPrim.h>
#include <lrt/assert.h>


#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

CObject(EBBCtrPrim) {
  COBJ_EBBFuncTbl(EBBCtr);
  uintptr_t v;
  EBBCtrId id;
};

extern CObjInterface(EBBCtr) EBBCtrPrim_ftable;

static inline void 
EBBCtrPrimSetFT(EBBCtrPrimRef o) { o->ft = &EBBCtrPrim_ftable; }

#define PRIVATE static

// Representative Code
PRIVATE EBBRC
EBBCtrPrim_init(void *_self) 
{
  EBBCtrPrimRef self = _self;
  self->v = 0;
  return EBBRC_OK;
}

PRIVATE EBBRC 
EBBCtrPrim_inc(EBBCtrRef _self) 
{
  EBBCtrPrimRef self = (EBBCtrPrimRef)_self;
  //gcc built-in atomics
  __sync_fetch_and_add(&self->v,1);
  return EBBRC_OK;
}

PRIVATE EBBRC 
EBBCtrPrim_dec(EBBCtrRef _self) 
{
  EBBCtrPrimRef self = (EBBCtrPrimRef)_self;
  //gcc builtin atomics
  __sync_fetch_and_sub(&self->v,1);
  return EBBRC_OK;
}

PRIVATE EBBRC
EBBCtrPrim_val(EBBCtrRef _self, uintptr_t *v)
{
  EBBCtrPrimRef self = (EBBCtrPrimRef)_self;
  *v = self->v;
  return EBBRC_OK;
}

CObjInterface(EBBCtr) EBBCtrPrim_ftable = {
  .inc = EBBCtrPrim_inc, 
  .dec = EBBCtrPrim_dec, 
  .val = EBBCtrPrim_val
};


// Statically declared root and rep... this does
// not therefore account for memory locality 
EBBRC
EBBCtrPrimStaticSharedCreate(EBBCtrId *id)
{
  static EBBCtrPrim theRep;
  static CObjEBBRootShared theRoot;
  EBBRC rc;

  // use the statically declared root and rep instances
  EBBCtrPrimRef repRef = &theRep;
  CObjEBBRootSharedRef rootRef = &theRoot;

  CObjEBBRootSharedSetFT(rootRef);
  EBBCtrPrimSetFT(repRef);

  // setup my representative and root
  EBBCtrPrim_init(repRef);

  // shared root knows about only one rep so we 
  // pass it along for it's init
  rootRef->ft->init(rootRef, (EBBRep *)repRef);

  rc = EBBAllocPrimId((EBBId *)id);
  LRT_RCAssert(rc);

  rc = CObjEBBBind((EBBId)*id, rootRef); 
  LRT_RCAssert(rc);

  return rc;
}


EBBRC
EBBCtrPrimSharedCreate(EBBCtrId *id)
{
  EBBCtrPrimRef repRef;
  CObjEBBRootSharedRef rootRef;
  EBBRC rc;

  //Allocate a root and rep via Primitive Allocator
  EBBPrimMalloc(sizeof(EBBCtrPrim), &repRef, EBB_MEM_DEFAULT);
  EBBCtrPrimSetFT(repRef);
  EBBCtrPrim_init(repRef);

  rc = CObjEBBRootSharedCreate(&rootRef, (EBBRepRef)repRef);
  LRT_RCAssert(rc);

  rc = EBBAllocPrimId((EBBId *)id);
  LRT_RCAssert(rc);

  rc = CObjEBBBind((EBBId)*id, rootRef); 
  LRT_RCAssert(rc);

  return rc;
}

