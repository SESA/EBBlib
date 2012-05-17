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

#include <stddef.h>
#include <config.h>
#include <stdint.h>
#include <lrt/assert.h>
#include <l0/lrt/trans.h>
#include <l0/cobj/cobj.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>

#include <l0/cplus/CPlusEBB.H>
#include <l0/cplus/CPlusEBBRoot.H>
#include <l0/cplus/CPlusEBBRootShared.H>

/* virtual */ EBBRC
CPlusEBBRootShared::handleMiss(CPlusEBB **obj, EBBLTrans *lt, EBBFuncNum fnum)
{
  EBBCacheObj(lt, (EBBRep *)theRep);
  *obj = theRep;
  return EBBRC_OK;
}

void *
CPlusEBBRootShared::operator new(size_t size)
{
  void *val;
  EBBRC rc;
  rc = EBBPrimMalloc(size, &val, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);
  return val;
}

void
CPlusEBBRootShared::operator delete(void * p, size_t size)
{
  // NYI
  LRT_RCAssert(0);
}
