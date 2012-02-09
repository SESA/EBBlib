#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "EBBKludge.H"

#include "Test.H"

#include "SSACSimpleSharedArray.H"

class MySSAC : public SSAC {
public:
  virtual EBBRC get( CacheObjectId &id, CacheEntry* &ce,
		     const gettype &type ); 
  virtual EBBRC putback( CacheEntry* &ce, const putflag &flag );
  virtual EBBRC flush();
  virtual EBBRC snapshot();
  static  EBBRC create(MySSAC **o);
};

/* static */ EBBRC 
MySSAC::create(MySSAC **o)
{
  *o = new(MySSAC);
  return 0;
}

/* virtual */ EBBRC 
MySSAC::get( CacheObjectId &id, CacheEntry* &ce,
	     const gettype &type ) 
{
  return (EBBRC)0;
}

/* virtual */ EBBRC 
MySSAC::putback( CacheEntry* &ce, const putflag &flag )
{
  return (EBBRC)0;
}

/* virtual */ EBBRC 
MySSAC::flush()
{
  TRACE("BEGIN");
  return (EBBRC)0;
}

/* virtual */ EBBRC 
MySSAC::snapshot()
{
  return (EBBRC)0;
}


class BTest : public Test {
  Barrier b;
protected:
  EBBRC work(int id);
  EBBRC init() {};
public:
  BTest(int n) : Test(n), b(n) {};
};

EBBRC
BTest::work(int id)
{
  TRACE("%d: BEGIN: %p", id, this);
  for (int j=0; j<2; j++) {
    if (id != (numWorkers - 1) ) {
      TRACE("%d: Not last worker entering Barrier %p\n", id, &b);
      b.enter();
      TRACE("%d: Not last worker LEFT Barrier %p\n", id, &b);
    } else {
      TRACE("%d: LAST worker Sleeping %p\n", id, &b);
      for (volatile int i=0; i<1000000000; i++);
      TRACE("%d: Last worker entering Barrier %p\n", id, &b);
      b.enter();
      TRACE("%d: Last worker LEFT Barrier %p\n", id, &b);
    }
    TRACE("%d: Worker doing test AGAIN\n", id, &b);   
  }
  TRACE("%d: END: %p", id, this);
  return 0;
}


void 
BarrierTest(int numWorkers)
{
  TRACE("BEGIN: %d\n", numWorkers);
 
  BTest b(numWorkers);
  
  b.doTest();

  TRACE("END\n");
}


class SSACTest : public Test {
protected:
  SSACRef ssac;
  enum {HASHTABLESIZE=128};
  EBBRC work(int id);
  EBBRC init();
  EBBRC end();
public:
  SSACTest(int n): Test(n) {}
};

EBBRC
SSACTest::init()
{
  TRACE("BEGIN");
  EBBRC rc;
  CacheObjectIdSimple id(0);
  CacheEntrySimple *entry=0;
  
  // just as an initial test call a function of the ssac
  DREF(ssac)->flush();
  
  for (int i=0; i<HASHTABLESIZE; i++) {
    id = i;
    rc=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
		       SSAC::GETFORWRITE);
    entry->data = (void *)i;
    rc=DREF(ssac)->putback((CacheEntry * &)entry, SSAC::KEEP);
  }
  TRACE("END");
  return 0;
} 

EBBRC
SSACTest::work(int myid) 
{
  //  TRACE("BEGIN");
  EBBRC rc;
  CacheObjectIdSimple id(0);
  CacheEntrySimple *entry=0;
  intptr_t v;

  for (int j=0; j<1;j++) {
    for (int i=0; i<HASHTABLESIZE; i++) {
      id = i;
      rc = DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
			   SSAC::GETFORWRITE);
      v=(intptr_t)entry->data; v++; entry->data=(void *)v;
      entry->dirty();
      rc =DREF(ssac)->putback((CacheEntry * &)entry, SSAC::KEEP);
    }
  }
  //  TRACE("END");
  return 0;
}

EBBRC
SSACTest::end()
{
  TRACE("BEGIN");
  DREF(ssac)->snapshot();
  printf("Test::end: \n");
  Test::end();
  TRACE("END");
  return 0;
}

class SSATest : public SSACTest {
public:
  SSATest(int n);
  virtual ~SSATest();
};

SSATest::SSATest(int n) : SSACTest(n) 
{
  // where is 'ssac' defined?
  ssac = SSACSimpleSharedArray::create(HASHTABLESIZE);
}

SSATest::~SSATest()
{
  // DREF(ssac)->destroy();
}

void
SSACSimpleSharedArrayTest(int numWorkers)
{
  TRACE("BEGIN");
  SSATest test(numWorkers);
  test.doTest();
  TRACE("END");
}

int 
main(int argc, char **argv)
{
  int n=4;

  if (argc>1) n=atoi(argv[1]);

#if 0
  BarrierTest(n);
#endif
  
  SSACSimpleSharedArrayTest(n);

  return 0;
}
