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
#include <l0/cobj/cobj.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>

#define MEMSIZE (1 << 20)

static uint8_t theMemory[MEMSIZE];

static EBBRC
EBBMemMgrPrim_init(void *_self)
{
  EBBMemMgrPrimRef self = _self;
  self->mem = theMemory;
  self->len = MEMSIZE;
  return EBBRC_OK;
}

//just grab from the beginning of the memory and move
//the pointer forward until we run out
static EBBRC
EBBMemMgrPrim_alloc(void *_self, uintptr_t size, void *mem, EBB_MEM_POOL pool)
{
  EBBMemMgrPrimRef self = _self;
  if (size > self->len) {
    *((void **)mem) = NULL; //Do I return some error code here??
  } else {
    *((void **)mem) = self->mem;
    self->mem += size;
    self->len -= size;
  }
  return EBBRC_OK;
}

//freeing is a nop in this implementation
static EBBRC
EBBMemMgrPrim_free(void *_self, void *mem) {
  return EBBRC_OK;
}

CObjInterface(EBBMemMgr) EBBMemMgrPrim_ftable = {
  .init = EBBMemMgrPrim_init, 
  .alloc = EBBMemMgrPrim_alloc, 
  .free = EBBMemMgrPrim_free
};


EBBMemMgrPrimRef *theEBBMemMgrPrimId;

EBBRC
EBBMemMgrPrimInit()
{
  EBBRC rc;
  static EBBMemMgrPrim theRep;
  static CObjEBBRootShared theRoot;
  EBBMemMgrPrimRef repRef = &theRep;
  CObjEBBRootSharedRef rootRef = &theRoot;

  // setup function tables
  CObjEBBRootSharedSetFT(rootRef);
  EBBMemMgrPrimSetFT(repRef);

  // setup my representative and root
  repRef->ft->init(repRef);
  // shared root knows about only one rep so we 
  // pass it along for it's init
  rootRef->ft->init(rootRef, &theRep);

  rc = EBBAllocPrimId(&theEBBMemMgrPrimId);
  //  EBBRCAssert(rc);

  rc = CObjEBBBind(theEBBMemMgrPrimId, rootRef); 
  //  EBBRCAssert(rc);

  return EBBRC_OK;
}
