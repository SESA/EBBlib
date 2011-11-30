#ifndef __EBB_MEMMGRPRIM_H__
#define __EBB_MEMMGRPRIM_H__
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

CObject(EBBMemMgrPrim) {
  CObjInterface(EBBMemMgr) *ft;
  void *mem;
  uintptr_t len;
};

extern CObjInterface(EBBMemMgr) EBBMemMgrPrim_ftable;

static inline void
EBBMemMgrPrimSetFT(EBBMemMgrPrimRef o) {o->ft = &EBBMemMgrPrim_ftable; }

typedef EBBMemMgrPrimRef *EBBMemMgrPrimId;
extern EBBRC EBBMemMgrPrimInit(void);
extern EBBMemMgrPrimRef *theEBBMemMgrPrimId;

static inline EBBRC
EBBPrimMalloc(uintptr_t size, void *mem, EBB_MEM_POOL pool) {
  return EC(theEBBMemMgrPrimId)->alloc(EB(theEBBMemMgrPrimId),
				       size, mem, pool);
}

static inline EBBRC
EBBPrimFree(void *mem) {
  return EC(theEBBMemMgrPrimId)->free(EB(theEBBMemMgrPrimId),
				      mem);
}

#endif