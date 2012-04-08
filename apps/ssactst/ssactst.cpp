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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

extern "C" {
#include <lrt/assert.h>
#include <lrt/io.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/lrt/types.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <l0/cobj/cobj.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EBBMgrPrim.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/App.h>
}
#include <l0/cplus/CPlusEBB.H>
#include <l0/cplus/CPlusEBBRoot.H>
#include <l0/cplus/CPlusEBBRootShared.H>

#include <contrib/jmcddn/ssac_cpp/EBBKludge.H>
#include <contrib/jmcddn/ssac_cpp/Test.H>
#include <contrib/jmcddn/ssac_cpp/SSACSimpleSharedArray.H>

class SSACTest : public Test {
protected:
  SSACId ssac;
  enum {HASHTABLESIZE=8192};//this is the size of hashqs, each with an 'associative' ammount of entries.
  EBBRC work(int id);
  EBBRC init();
  EBBRC end();
public:
  SSACTest(int n, int m, int c, bool p, double wpct): Test(n,m,c,p,wpct) {}

};

EBBRC
SSACTest::init()
{
//  TRACE("BEGIN");
  CacheObjectIdSimple id(0);
  CacheEntrySimple *entry=0;
  EBBRC rc;
  // run through each entry of the hashqs, clear value & h
  DREF(ssac)->flush();

  for (unsigned long i=0; i<HASHTABLESIZE; i++) {
    id = i;
    rc=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
		       SSAC::GETFORWRITE);
    entry->data = (void *)i; // set data pointer to i TODO: verify
    rc=DREF(ssac)->putback((CacheEntry * &)entry, SSAC::KEEP);
  }
//  TRACE("END");
  return rc;
}

EBBRC
SSACTest::work(int myid)
{
  //  TRACE("BEGIN");
  CacheObjectIdSimple id(0);
  CacheEntrySimple *entry=0;
  int readCount, writeCount;
  EBBRC rc;
  intptr_t v;

  readCount = (1-writePct) * numEvents;
  writeCount = writePct * numEvents;

  for (int j=0; j<1;j++) {
    // write to SSAC data object (increase pointer by 1)
    for (int i=0; i<writeCount; i++) {
      id = i;
      rc = DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
			   SSAC::GETFORWRITE);
      v=(intptr_t)entry->data; v++; entry->data=(void *)v;
      entry->dirty();
      rc =DREF(ssac)->putback((CacheEntry * &)entry, SSAC::KEEP);
    }
    for (int k=0; k<readCount; k++){
      id = k;
      rc = DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
			   SSAC::GETFORREAD);
      v=(intptr_t)entry->data;
    }
  }
  //  TRACE("END");
  return rc;
}

EBBRC
SSACTest::end()
{
 // TRACE("BEGIN");
 // DREF(ssac)->snapshot();
//  TRACE("Tests:end: \n");
  Test::end();
//  TRACE("END");
  return 0;
}

class SSATest : public SSACTest {
public:
  SSATest(int n, int m, int c, bool p, double wpct);
  virtual ~SSATest();
};

SSATest::SSATest(int n, int m, int c, bool p, double wpct) : SSACTest(n,m,c,p,wpct)
{
  // init hash table
  SSACSimpleSharedArray::Create(ssac, HASHTABLESIZE);
}

SSATest::~SSATest()
{
  // DREF(ssac)->destroy();
}

void
SSACSimpleSharedArrayTest(int numWorkers, int numIterations, int numEvents, bool bindThread, double wpct)
{
  SSATest test(numWorkers, numIterations, numEvents, bindThread, wpct);
  test.doTest();
}
/* *************************************** */

EBBRC
SSACTST_start(AppRef _self, int argc, char **argv,
	      char **environ)
{
  EBB_LRT_printf("SSACTST LOADED\n");
  int n=4; // thread count
  int m=1; // no. of iterations
  bool p=1; // bind threads?
  double w=0.5; // test read/write percentage
  int c=1000; // test event no
  if (argc>1) n=atoi(argv[1]);
  if (argc>2) m=atoi(argv[2]);
  if (argc>3) c=atoi(argv[3]);
  if (argc>4) w=atof(argv[4]);
  if (argc>5) p=atoi(argv[5]);
  SSACSimpleSharedArrayTest(n,m,c,p,w);
  EBB_LRT_printf("finished simpleTst\n");
  return EBBRC_OK;
}

CObject(SSACTST) {
  CObjInterface(App) *ft;
};

CObjInterface(App) SSACTST_ftable = {SSACTST_start};

extern "C" {
APP(SSACTST);
}
