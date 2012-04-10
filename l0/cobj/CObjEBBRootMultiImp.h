#ifndef __COBJ_EBB_ROOT_MULTI_IMP_H__
#define __COBJ_EBB_ROOT_MULTI_IMP_H__
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

#include <l0/cobj/CObjEBBRootMulti.h>

// JA: DO NOT GET RID OF IMPLEMENTATION WE NEED
//     STATICS OF THESE

// Got rid of abstract type since we may commonly need
// to delare statics of these and embed them
CObject(CObjEBBRootMultiImp) {
  CObjInterface(CObjEBBRootMulti) *ft;
  CreateRepFunc createRep;
  RepListNode *head;
  uintptr_t lock;
};

extern EBBRC CObjEBBRootMultiImpStaticInit(CObjEBBRootMultiImpRef o, CreateRepFunc func);
extern EBBRC CObjEBBRootMultiImpCreate(CObjEBBRootMultiImpRef *o, CreateRepFunc func);

#endif
