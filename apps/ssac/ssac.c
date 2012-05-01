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
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <lrt/assert.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
/*
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
*/
#include <l0/cplus/CPlusEBB.H>
#include <l0/cplus/CPlusEBBRoot.H>
#include <l0/cplus/CPlusEBBRootShared.H>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/App.h>

/*
 *  typedef struct SSAC_obj SSAC;
 *  typedef SSAC* SSACRef
 *  struct SSAC_obj {
 *    struct App_if *ft;
 *  }
 * */
CObject(SSAC) {
  CObjInterface(App) *ft; //
};

/* EBB start function */
EBBRC
SSAC_start(AppRef _self, int argc, char **argv,
		 char **environ)
{
  // test specifics
  int n=4;      // thread count
  int m=1;      // no. of iterations
  int c=1000;   // no. of cache calls per thread
  bool p=1;     // bind threads (yes/no)
  double w=0.5; // cache call read/write percentage
  // create new Cplus root & rep
  uintptr_t res;
  EBBRC rc;
  TestId test;
  Test *rep;
  CPlusEBBRootShared *root;

  rep = new EBBTest();
  root = new CPlusEBBRootShared();
/** Static memory
  static CtrCPlus repObj;
  static CPlusEBBRootShared rootObj;
  rep = &repObj;
  root = &rootObj; */
  // setup my representative and root
  rep->init();
  root->init(rep);

  rc = EBBAllocPrimId((EBBId *)&test);
  LRT_RCAssert(rc);

  rc = CPlusEBBRoot::EBBBind((EBBId)test, root);
  LRT_RCAssert(rc);
  lrt_printf("SSAC Test LOADED\n");

/*
  SSATest test(n,m,c,p,w);
  test.doTest();
*/
  // run test
  CPLUS_EBBCALL(test, doTest);

  return EBBRC_OK;
}

/* *  struct App_if SSAC_ftable = ...  */
CObjInterface(App) SSAC_ftable = {
  .start = SSAC_start
};

APP(SSAC);
/* EBBRep * App_createRep(CObjeBBRootMultiRef _self)
  {
      SSAC * repRef;
      EBBPrimMalloc(sizeof(SSAC), &redRef, EBB_MEM_DEFAULT);
      repRef->ft = &SSAC_ftable;
      return (SSAC *) repRef;
  } */

/*******************************************************************/
/*******************************************************************/

class SSACTest : public Test {
protected:
  SSACRef ssac;
  enum {HASHTABLESIZE=8192};//size of hashq, each with 'associative' ammount of entries.
  EBBRC work(int id);
  EBBRC init();
  EBBRC end();
public:
  SSACTest(int n, int m, int c, bool p, double wpct): Test(n,m,c,p,wpct) {}

};

/* Initialise array */
EBBRC
SSACTest::init()
{
//  TRACE("BEGIN");
  CacheObjectIdSimple id(0);
  CacheEntrySimple *entry=0;
  EBBRC rc;
  // run through each entry of the hashqs, clear value & h
  DREF(ssac)->flush();
// chanhe
  for (unsigned long i=0; i<HASHTABLESIZE; i++) {
    id = i;
    /*
    rc=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry, SSAC::GETFORWRITE);
    entry->data = (void *)i; // set data pointer to i TODO: verify
    rc=DREF(ssac)->putback((CacheEntry * &)entry, SSAC::KEEP);
    */
  }
//  TRACE("END");
  return rc;
}

/* pull and update an entry from each hashq of the cache */
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
    //  rc = DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry, SSAC::GETFORWRITE);
      v=(intptr_t)entry->data; v++; entry->data=(void *)v;
      entry->dirty();
    //  rc =DREF(ssac)->putback((CacheEntry * &)entry, SSAC::KEEP);
    }
    for (int k=0; k<readCount; k++){
      id = k;
    //  rc = DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry, SSAC::GETFORREAD);
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
  ssac = SSACSimpleSharedArray::create(HASHTABLESIZE);
}

SSATest::~SSATest()
{
  // DREF(ssac)->destroy();
}

