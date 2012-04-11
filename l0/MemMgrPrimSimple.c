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

/* 
 * this implements a simple first fit allocator as described in 
 */

#include <config.h>

#include <stdint.h>
#include <stddef.h>

#include <lrt/io.h>
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

/*
 * this depends on posix compiler, since a long is assumed to be size of a pointer
 * on 64 bit machines this is 63 bit, else 31 bit
 */
static const uintptr_t MAX_BLOCK_SIZE = (~0ul >> 1);

CObject(EBBMemMgrPrimSimple) {
  COBJ_EBBFuncTbl(EBBMemMgr);

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
} Bumper;

#if 0 // OKRIEGFIXME use the right version of gcc
_Static_assert(sizeof(Bumper) == sizeof(uintptr_t),
  "Bumper is of unexpected size");
#endif

static EBBRC
init_rep(EBBMemMgrPrimSimpleRef self, CObjEBBRootMultiRef rootRef, uintptr_t end)
{
  Bumper *prologue_hdr, *prologue_ftr, *first_block_hdr,
    *first_block_ftr, *epilog_hdr, *epilog_ftr;
  
  // our allocatable memory region starts right after the memory we carved off
  // for ourselves:
  self->mem = (void *)((uintptr_t)self + sizeof(*self));
  // ... but we want it to be sizeof(uintptr_t) aligned, so move it up to the
  // next boundary if it's not:
  if((uintptr_t)self->mem % sizeof(uintptr_t) != 0)
    self->mem += sizeof(uintptr_t) - ((uintptr_t) self->mem % sizeof(uintptr_t));

  // Put a minimum size prolouge block at the beginning of memory that's considered
  // in-use. this prevents free from going off the beginning of our memory when
  // coalescing.
  prologue_hdr = self->mem;
  prologue_ftr = prologue_hdr + 1;
  prologue_hdr->used = 1;
  prologue_hdr->size = 2*sizeof(Bumper);
  prologue_ftr->raw = prologue_hdr->raw;

  // Put a epilog block at the end of memory that's considered
  // in-use. this prevents free from running off the end of our memory when
  // coalescing.
  epilog_hdr = (Bumper *)(end - 2*sizeof(Bumper));
  epilog_ftr = epilog_hdr + 1;
  epilog_hdr->used = 1;
  epilog_hdr->size = 2*sizeof(Bumper);
  epilog_ftr->raw = epilog_hdr->raw;
  
  // We want the first thing after prologue to be free:
  first_block_hdr = prologue_ftr + 1;
  first_block_hdr->used = 0;
  first_block_hdr->size = (uintptr_t)epilog_hdr - (uintptr_t)first_block_hdr;
  first_block_ftr = (Bumper*)(((uintptr_t)first_block_hdr)
    + first_block_hdr->size - sizeof(Bumper));
  first_block_ftr->raw = first_block_hdr->raw;  

  // don't handle blocks larger than a pointer can point to
  EBBAssert(((uintptr_t)epilog_hdr - (uintptr_t)first_block_hdr) <= MAX_BLOCK_SIZE);

  self->end = end;
  self->myRoot = rootRef;
  return EBBRC_OK;
}

static EBBRC
EBBMemMgrPrimSimple_alloc(EBBMemMgrRef _self, uintptr_t size, void **mem, EBB_MEM_POOL pool)
{
  EBBMemMgrPrimSimpleRef self = (EBBMemMgrPrimSimpleRef)_self;
  Bumper *hdr = (Bumper*)self->mem;
  Bumper *ftr;
  Bumper *next_hdr, *next_ftr;
 
  // adjust size to include header and footer:
  size += 2 * sizeof(Bumper);
  // and make sure it's a multiple of sizeof(uintptr_t) - this will ensure
  // everything stays aligned.
  if(size % sizeof(uintptr_t) != 0) {
    size += sizeof(uintptr_t) - (size % sizeof(uintptr_t));
  }

  // find a free block
  while((uintptr_t)hdr < self->end && (hdr->used || hdr->size < size)) {
    hdr = (Bumper*)(((uintptr_t)hdr) + (uintptr_t)hdr->size);
  }

  if((uintptr_t)hdr >= self->end) {
    // couldn't find a block.
    *mem = NULL;
    return EBBRC_OUTOFRESOURCES;
  } else {
    hdr->used = 1;
    next_ftr = (Bumper*)(((uintptr_t)hdr) + hdr->size - sizeof(Bumper));
    next_ftr->size -= size;
    hdr->size = size;
    ftr = (Bumper*)(((uintptr_t)hdr) + hdr->size - sizeof(Bumper));
    ftr->raw = hdr->raw;
    next_hdr = ftr + 1;
    next_hdr->raw = next_ftr->raw;
    *mem = hdr+1;
  }
  return EBBRC_OK;
}

static EBBRC
EBBMemMgrPrimSimple_free(EBBMemMgrRef _self, uintptr_t size, void *mem) {
  EBBMemMgrPrimSimpleRef self = (EBBMemMgrPrimSimpleRef)_self;
  Bumper *hdr, *ftr, *left_ftr, *right_hdr;
  uintptr_t newsize;

  hdr = mem;
  hdr--;
  ftr = (Bumper*)(((uintptr_t)hdr) + hdr->size - sizeof(Bumper));
  newsize = hdr->size; 


  // FIXME: return memory to right allocator
  EBBWAssert((mem > self->mem) && (mem < (void *)self->end));

  // sanity check that size is the same as the recorded
  if(size % sizeof(uintptr_t) != 0) {
    size += sizeof(uintptr_t) - (size % sizeof(uintptr_t));
  }
  EBBAssert(size == (hdr->size-2*sizeof(Bumper)));

  // See if we can coalesce to the left:
  left_ftr = hdr - 1;
  if(!left_ftr->used) {
    hdr = (Bumper*)(((uintptr_t)left_ftr) - left_ftr->size + sizeof(Bumper));
    newsize += hdr->size;
  }

  // and the right:
  right_hdr = ftr + 1;
  if(!right_hdr->used) {
    ftr = (Bumper*)(((uintptr_t)right_hdr) + right_hdr->size - sizeof(Bumper));
    newsize += ftr->size;
  }

  hdr->size = newsize;
  hdr->used = 0;
  ftr->raw = hdr->raw;

  return EBBRC_OK;
}

CObjInterface(EBBMemMgr) EBBMemMgrPrimSimple_ftable = {
  .alloc = EBBMemMgrPrimSimple_alloc, 
  .free = EBBMemMgrPrimSimple_free
};

static inline void
EBBMemMgrPrimSimpleSetFT(EBBMemMgrPrimSimpleRef o) {o->ft = &EBBMemMgrPrimSimple_ftable; }


static EBBRep *
MemMgrPrimRB_createRep(CObjEBBRootMultiRef _self)
{
  EBBAssert(0);
  return NULL;
}

EBBRC
EBBMemMgrPrimSimpleInit()
{
  static CObjEBBRootMultiImp theRoot;
  CObjEBBRootMultiImpRef rootRef = &theRoot;
  EBBMemMgrPrimSimpleRef repRef;
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
  repRef = (EBBMemMgrPrimSimpleRef)lrt_mem_start();

  // initialize the rep memory
  EBBMemMgrPrimSimpleSetFT(repRef); 
  init_rep(repRef, (CObjEBBRootMultiRef)rootRef, lrt_mem_end());

  // manually install rep into local table so that memory allocations 
  // can work immediate without recursion
  lt = (EBBLTrans *)lrt_trans_id2lt((uintptr_t)theEBBMemMgrPrimId);
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
  return EBBMemMgrPrimSimpleInit();
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

CObject(EBBMemMgrPrimSimpleQueen) {
  CObjInterface(EBBMemMgr) *ft;
  CObjEBBRootMulti root;
  struct EBBMemMgrData data;
};

CObject(EBBMemMgrPrimSimpleDrone) {
  CObjInterface(EBBMemMgr) *ft;
  EBBMemMgrPrimSimpleQueenRef *queen;
  struct EBBMemMgrData data;
};

EBBRC
EBBMemMgrPrimSimpleInit()
{
  EBBRC rc;
  EBBId id;

  repRef = (EBBMemMgrPrimSimpleRef)lrt_mem_start();
  if (__sync_bool_compare_and_swap(&(theEBBMemMgrPrimId), 0, -1)) {
    EBBMemMgrPrimSimpleQueen_init(repRef, lrt_mem_end());              
    __sync_bool_compare_and_swap(&(theEBBMemMgrPrimId), -1, id);
  } else {   
    // races on root setup is taken care of here
    while (((volatile uintptr_t)theEBBMemMgrPrimId)==-1);
    EBBMemMgrPrimSimpleDrone_init(repRef, lrt_mem_end());              
  }
  theRoot.addRepOn(lrt_pic_id, theRep);               // Add my rep to the Root
  return EBBRC_OK;
}
#endif







