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
#include <l0/types.h>
#include <l0/cobj/cobj.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l0/cplus/CPlusEBB.H>
#include <l0/cplus/CPlusEBBRoot.H>
#include <l0/cplus/CPlusEBBRootShared.H>
#include <misc/CtrCPlus.H>
#include <lrt/assert.h>

/* virtual */ EBBRC
CtrCPlus::init( )
{
  v = 0;
  return EBBRC_OK;
}

/* virtual */ EBBRC
CtrCPlus::inc( )
{
  __sync_fetch_and_add(&v,1);
  return EBBRC_OK;
}

/* virtual */ EBBRC 
CtrCPlus::dec( )
{
  __sync_fetch_and_sub(&v,1);
  return EBBRC_OK;
}

/* virtual */ EBBRC 
CtrCPlus::val(uintptr_t &rv)
{
  rv = v;
  return EBBRC_OK;
}

void * 
CtrCPlus::operator new(size_t size)
{
  void *val;
  EBBRC rc;
  rc = EBBPrimMalloc(size, &val, EBB_MEM_DEFAULT);
  EBBRCAssert(rc);
  return val;
}

void 
CtrCPlus::operator delete(void * p, size_t size)
{
  // NYI
  EBBRCAssert(0);
}


#define DYNAMIC
/* static */ EBBRC 
CtrCPlus::Create(CtrCPlusId &ctr)
{
  EBBRC rc;
  CtrCPlus *rep;
  CPlusEBBRootShared *root;

#ifdef DYNAMIC
  rep = new CtrCPlus();
  root = new CPlusEBBRootShared();
  EBB_LRT_printf("c++ counter test using dynamic memory\n");
#else
  static CtrCPlus repObj;
  static CPlusEBBRootShared rootObj;
  rep = &repObj;
  root = &rootObj;
#endif

  // setup my representative and root
  rep->init();
  // shared root knows about only one rep so we 
  // pass it along for it's init
  root->init(rep);

  rc = EBBAllocPrimId(&ctr);
  EBBRCAssert(rc);

  rc = CPlusEBBRoot::EBBBind(ctr, root); 
  EBBRCAssert(rc);
}

void test_cplus_counter(void)
{
  uintptr_t res;
  CtrCPlusId ctr;

  EBB_LRT_printf("running c++ counter test\n");
  CtrCPlus::Create(ctr);

  CPLUS_EBBCALL(ctr, inc);
  CPLUS_EBBCALL(ctr, inc);
  CPLUS_EBBCALL(ctr, inc);
  CPLUS_EBBCALL(ctr, val, res);
  
  EBBAssert(res == 3);
  EBBId_DREF(ctr)->inc();
  EBBId_DREF(ctr)->val(res);
  EBBAssert(res == 4);
  EBB_LRT_printf("c++ counter test passed\n");

}
