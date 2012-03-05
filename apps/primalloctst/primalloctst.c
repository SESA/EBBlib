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

#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <lrt/io.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <lrt/assert.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/lrt/pic.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/MsgMgr.h>
#include <l1/MsgMgrPrim.h>
#include <l1/App.h>

// Do some basic testing of the primitive memory allocator.

CObject(PrimAllocTst) {
  CObjInterface(App) *ft;
};

EBBRC 
PrimAllocTst_start(AppRef _self)
{
  uintptr_t i, j;
  void *ptrs[128];
  
  EBB_LRT_printf("Starting primitive memory allocator test...\n");
  for(i = 0; i < 5; i++) {
    for(j = 1; j < 128; j++) {
      // XXX: we pass a null EBB_MEM_POOL here; this is exploiting temporary
      // knowledge that the impelmentations don't use this parameter.
      if(EBBPrimMalloc(j, &ptrs[j], (EBB_MEM_POOL)NULL) != EBBRC_OK) {
        EBB_LRT_printf("allocation failed: i = %lu, j = %lu.\n", i, j);
      } else {
        EBB_LRT_printf("allocated pointer %lx for i = %lu, j = %lu\n", (uintptr_t)ptrs[j], i, j);
      }
    }
    for(j = 1; j < 128; j++) {
      if(EBBPrimFree(j, ptrs[j]) != EBBRC_OK) {
        EBB_LRT_printf("free failed: i = %lu, j = %lu.\n", i, j);
      } else {
        EBB_LRT_printf("freed pointer %lx for i = %lu, j = %lu\n", (uintptr_t)ptrs[j], i, j);
      }
    }
  }
  return EBBRC_OK;
}

CObjInterface(App) PrimAllocTst_ftable = {
  .start = PrimAllocTst_start
};

APP(PrimAllocTst);

