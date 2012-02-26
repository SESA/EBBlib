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
#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/EBBMgrPrimBoot.h>
#include <l0/EventMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l0/lrt/mem.h>

static const uintptr_t RB_MAX_BLOCK_SIZE = (~0ul >> 1);

CObject(EBBMemMgrPrimRB) {
  CObjInterface(EBBMemMgr) *ft;
  CObjEBBRootMultiRef myRoot;
  void *mem;
  uintptr_t end;
};

// A "Bumper" is stored in the memory immediately before and after an allocated block.
// We use one bit to store whether the block is used or not, the rest of the bumper is
// the size of the block.
typedef union {
  struct {
   uintptr_t size : 8 * sizeof(uintptr_t) - 1;
   uintptr_t used : 1;
  };
  uintptr_t raw;
} PrimRBBumper;

_Static_assert(sizeof(PrimRBBumper) == sizeof(uintptr_t),
  "PrimRBBumper is of unexpected size");

static EBBRC
init_rep(EBBMemMgrPrimRBRef self, CObjEBBRootMultiRef rootRef, uintptr_t end)
{
  PrimRBBumper *prologue_hdr, *prologue_ftr, *first_block_hdr,
    *first_block_ftr;
  
  // our allocatable memory region starts right after the memory we carved off
  // for ourselves:
  self->mem = (void *)((uintptr_t)self + sizeof(*self));
  // ... but we want it to be sizeof(uintptr_t) aligned, so move it up to the
  // next boundary if it's not:
  if((uintptr_t)self->mem % sizeof(uintptr_t) != 0)
    self->mem += sizeof(uintptr_t) - ((uintptr_t) self->mem % sizeof(uintptr_t));

  // Put a prolouge block at the beginning of minimum size that's considered
  // in-use. this prevents free from running off the end of our memory when
  // coalescing.
  prologue_hdr = self->mem;
  prologue_ftr = prologue_hdr + 1;
  prologue_hdr->used = 1;
  prologue_hdr->size = 2*sizeof(PrimRBBumper);
  prologue_ftr->raw = prologue_hdr->raw;
  
  // We want the first thing after this to be free:
  first_block_hdr = prologue_ftr + 1;
  first_block_hdr->used = 0;
  if((uintptr_t)first_block_hdr - end <= RB_MAX_BLOCK_SIZE)
    first_block_hdr->size = end - (uintptr_t)first_block_hdr;
  else
    first_block_hdr->size = RB_MAX_BLOCK_SIZE;

  first_block_ftr = (PrimRBBumper*)(((uintptr_t)first_block_hdr)
    + first_block_hdr->size - sizeof(PrimRBBumper));
  first_block_ftr->raw = first_block_hdr->raw;  

  self->end = end;
  self->myRoot = rootRef;
  return EBBRC_OK;
}

static EBBRC
EBBMemMgrPrimRB_alloc(EBBMemMgrRef _self, uintptr_t size, void **mem, EBB_MEM_POOL pool)
{
  EBBMemMgrPrimRBRef self = (EBBMemMgrPrimRBRef)_self;
  PrimRBBumper *hdr = (PrimRBBumper*)self->mem;
  PrimRBBumper *ftr;
 
  // adjust size to include header and footer:
  size += 2 * sizeof(PrimRBBumper);
  // and make sure it's a multiple of sizeof(uintptr_t) - this will ensure
  // everything stays aligned.
  if(size % sizeof(uintptr_t) != 0)
    size += sizeof(uintptr_t) - (size % sizeof(uintptr_t));

  // find a free block
  while((uintptr_t)hdr < self->end && (hdr->used || hdr->size < size)) {
    hdr = (PrimRBBumper*)(((uintptr_t)hdr) + (uintptr_t)hdr->size);
  }

  if((uintptr_t)hdr > self->end) {
    // couldn't find a block. TODO: maybe return an error code?
    *mem = NULL;
  } else {
    hdr->used = 1;
    ftr = (PrimRBBumper*)(((uintptr_t)hdr-1) + hdr->size);
    ftr->raw = hdr->raw;
    *mem = hdr+1;
  }

  return EBBRC_OK;
}

static EBBRC
EBBMemMgrPrimRB_free(EBBMemMgrRef _self, uintptr_t size, void *mem) {
  PrimRBBumper *hdr, *ftr, *left_hdr, *left_ftr, *right_hdr, *right_ftr;

  hdr = mem;
  hdr--;

  // TODO: might want to put a sanity check here; we expect size == hdr->size - 2*sizeof(PrimRBBumper).
  // We may also want to check that mem is within the bounds of our heap.

  // See if we can coalesce to the left:
  left_ftr = hdr - 1;
  if(!left_ftr->used) {
    left_hdr = (PrimRBBumper*)(((uintptr_t)left_ftr+1) - left_ftr->size);
    left_hdr->size += hdr->size;
    hdr = left_hdr;
  }

  // find our footer. update its size.  
  ftr = (PrimRBBumper*)(((uintptr_t)hdr-1) + hdr->size);
  ftr->raw = hdr->raw;

  // and the right:
  right_hdr = ftr + 1;
  if(!right_hdr->used) {
    right_ftr = (PrimRBBumper*)(((uintptr_t)right_hdr-1) + right_hdr->size);
    right_ftr->size += ftr->size;
    ftr = right_ftr;
  }

  // flag as free.
  hdr->used = ftr->used = 0;

  return EBBRC_OK;
}

CObjInterface(EBBMemMgr) EBBMemMgrPrimRB_ftable = {
  .alloc = EBBMemMgrPrimRB_alloc, 
  .free = EBBMemMgrPrimRB_free
};

static inline void
EBBMemMgrPrimRBSetFT(EBBMemMgrPrimRBRef o) {o->ft = &EBBMemMgrPrimRB_ftable; }


static EBBRep *
MemMgrPrimRB_createRep(CObjEBBRootMultiRef _self)
{
  EBBAssert(0);
  return NULL;
}

EBBRC
EBBMemMgrPrimRBInit()
{
  static CObjEBBRootMultiImp theRoot;
  CObjEBBRootMultiImpRef rootRef = &theRoot;
  EBBMemMgrPrimRBRef repRef;
  EBBLTrans *lt;
  EBBRC rc;
  EBBId id;
  
  if (__sync_bool_compare_and_swap(&(theEBBMemMgrPrimId), 0, -1)) {
    CObjEBBRootMultiImpStaticInit(rootRef, MemMgrPrimRB_createRep);
    rc = EBBAllocPrimIdBoot(&id);
    EBBRCAssert(rc);
    rc = CObjEBBBindBoot(id, rootRef); 
    EBBRCAssert(rc);
    
    __sync_bool_compare_and_swap(&(theEBBMemMgrPrimId), -1, id);
  } else {   
    // racing with root creation...wait till root is ready
    while (((volatile uintptr_t)theEBBMemMgrPrimId)==-1);
  }
  // no where to alloc rep from other than the memory
  // we are creating this rep to manage so we do the obvious
  // and hack off some memory for the rep itself.
  // "create the rep"
  repRef = (EBBMemMgrPrimRBRef)lrt_mem_start();

  // initialize the rep memory
  EBBMemMgrPrimRBSetFT(repRef); 
  init_rep(repRef, (CObjEBBRootMultiRef)rootRef, lrt_mem_end());

  // manually install rep into local table so that memory allocations 
  // can work immediate without recursion
  lt = (EBBLTrans *)lrt_trans_id2lt((lrt_transid)theEBBMemMgrPrimId);
  EBBCacheObj(lt, (EBBRep *)repRef); 

  // it is now safe to call the allocator assuming that the 
  // ltrans is stable between last and the next one that 
  // may use dynamic memory to add the rep to the root
  rootRef->ft->addRepOn((CObjEBBRootMultiRef)rootRef, MyEL(), (EBBRep *)repRef);

  // Ok at this point the memory manager is up on this EL
  // and missing on the local table is also safe for this EL
  // as the rep has been added explicity to the root.

  return EBBRC_OK;
}

EBBRC EBBMemMgrPrimInit(void) {
  return EBBMemMgrPrimRBInit();
}

#if 0
// ALTENATIVE -- CHIMERA: QUEENS, CLONES, DRONES
// hmmm not sure if it would not be better to start off with thinking
// about the role of firstRep embedding the Root
// this implies a single rep object and a multirep are more similar
// and my evolve more naturally ... this makes it more natural to think
// of hybrids and devolve the role of the root and thus centeralized data
struct EBBMemMgrData {
  void *mem;
  uintptr_t len;
};

CObject(EBBMemMgrPrimRBQueen) {
  CObjInterface(EBBMemMgr) *ft;
  CObjEBBRootMulti root;
  struct EBBMemMgrData data;
};

CObject(EBBMemMgrPrimRBDrone) {
  CObjInterface(EBBMemMgr) *ft;
  EBBMemMgrPrimRBQueenRef *queen;
  struct EBBMemMgrData data;
};

EBBRC
EBBMemMgrPrimRBInit()
{
  EBBRC rc;
  EBBId id;

  repRef = (EBBMemMgrPrimRBRef)lrt_mem_start();
  if (__sync_bool_compare_and_swap(&(theEBBMemMgrPrimId), 0, -1)) {
    EBBMemMgrPrimRBQueen_init(repRef, lrt_mem_end());              
    __sync_bool_compare_and_swap(&(theEBBMemMgrPrimId), -1, id);
  } else {   
    // races on root setup is taken care of here
    while (((volatile uintptr_t)theEBBMemMgrPrimId)==-1);
    EBBMemMgrPrimRBDrone_init(repRef, lrt_mem_end());              
  }
  theRoot.addRepOn(lrt_pic_id, theRep);               // Add my rep to the Root
  return EBBRC_OK;
}
#endif







