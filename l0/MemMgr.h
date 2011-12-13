#ifndef __EBBMEMMGR_H__
#define __EBBMEMMGR_H__
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

typedef enum {EBB_MEM_LOCAL, EBB_MEM_LOCAL_PADDED,
	      EBB_MEM_GLOBAL, EBB_MEM_GLOBAL_PADDED,
	      EBB_MEM_DEFAULT = EBB_MEM_GLOBAL} EBB_MEM_POOL;

CObject(EBBMemMgr) {
  CObjInterface(EBBMemMgr) *ft;
};

CObjInterface(EBBMemMgr) {
  EBBRC (*alloc) (EBBMemMgrRef _self, uintptr_t size, void **mem, EBB_MEM_POOL pool);
  EBBRC (*free) (EBBMemMgrRef _self, void *mem);
};

typedef EBBMemMgrRef *EBBMemMgrId;

#endif
