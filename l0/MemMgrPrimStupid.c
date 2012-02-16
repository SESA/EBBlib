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
#include <l0/MemMgrPrimStupid.h>
#include <l0/lrt/mem.h>

CObject(EBBMemMgrPrimStupid) {
  CObjInterface(EBBMemMgr) *ft;
  CObjEBBRootMultiRef myRoot;
  void *mem;
  uintptr_t len;
};

static EBBRC
init_rep(EBBMemMgrPrimStupidRef self, CObjEBBRootMultiRef rootRef, uintptr_t end)
{
  self->mem = (void *)((uintptr_t)self + sizeof(*self));
  self->len = end - (uintptr_t)self->mem;
  self->myRoot = rootRef;
  return EBBRC_OK;
}

//just grab from the beginning of the memory and move
//the pointer forward until we run out
static EBBRC
EBBMemMgrPrimStupid_alloc(EBBMemMgrRef _self, uintptr_t size, void **mem, EBB_MEM_POOL pool)
{
  EBBMemMgrPrimStupidRef self = (EBBMemMgrPrimStupidRef)_self;
  if (size > self->len) {
    *mem = NULL; //Do I return some error code here??
  } else {
    *mem = self->mem;
    self->mem += size;
    self->len -= size;
  }
  return EBBRC_OK;
}

//freeing is a nop in this implementation
static EBBRC
EBBMemMgrPrimStupid_free(EBBMemMgrRef _self, uintptr_t size, void *mem) {
  return EBBRC_OK;
}

CObjInterface(EBBMemMgr) EBBMemMgrPrimStupid_ftable = {
  .alloc = EBBMemMgrPrimStupid_alloc, 
  .free = EBBMemMgrPrimStupid_free
};

static inline void
EBBMemMgrPrimStupidSetFT(EBBMemMgrPrimStupidRef o) {o->ft = &EBBMemMgrPrimStupid_ftable; }


static EBBRep *
MemMgrPrimStupid_createRep(CObjEBBRootMultiRef _self)
{
  EBBAssert(0);
  return NULL;
}

EBBRC
EBBMemMgrPrimStupidInit()
{
  static CObjEBBRootMultiImp theRoot;
  CObjEBBRootMultiImpRef rootRef = &theRoot;
  EBBMemMgrPrimStupidRef repRef;
  EBBLTrans *lt;
  EBBRC rc;
  EBBId id;
  
  if (__sync_bool_compare_and_swap(&(theEBBMemMgrPrimId), 0, -1)) {
    CObjEBBRootMultiImpStaticInit(rootRef, MemMgrPrimStupid_createRep);
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
  repRef = (EBBMemMgrPrimStupidRef)lrt_mem_start();

  // initialize the rep memory
  EBBMemMgrPrimStupidSetFT(repRef); 
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

CObject(EBBMemMgrPrimStupidQueen) {
  CObjInterface(EBBMemMgr) *ft;
  CObjEBBRootMulti root;
  struct EBBMemMgrData data;
};

CObject(EBBMemMgrPrimStupidDrone) {
  CObjInterface(EBBMemMgr) *ft;
  EBBMemMgrPrimStupidQueenRef *queen;
  struct EBBMemMgrData data;
};

EBBRC
EBBMemMgrPrimStupidInit()
{
  EBBRC rc;
  EBBId id;

  repRef = (EBBMemMgrPrimStupidRef)lrt_mem_start();
  if (__sync_bool_compare_and_swap(&(theEBBMemMgrPrimId), 0, -1)) {
    EBBMemMgrPrimStupidQueen_init(repRef, lrt_mem_end());              
    __sync_bool_compare_and_swap(&(theEBBMemMgrPrimId), -1, id);
  } else {   
    // races on root setup is taken care of here
    while (((volatile uintptr_t)theEBBMemMgrPrimId)==-1);
    EBBMemMgrPrimStupidDrone_init(repRef, lrt_mem_end());              
  }
  theRoot.addRepOn(lrt_pic_id, theRep);               // Add my rep to the Root
  return EBBRC_OK;
}
#endif







