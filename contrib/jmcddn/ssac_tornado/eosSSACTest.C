#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "EBBKludge.H"

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

// simple dumb spin based barrier to make life easier
class Barrier {
  int size;
  volatile int enterCount;
  volatile int leaveCount;
  void reset() { enterCount = leaveCount = size; }
  void leave() {
     __sync_sub_and_fetch(&leaveCount, 1);
     if (leaveCount == 0 ) reset();  // last one to leave resets barrier
  }
public:
  Barrier(int val) : size(val) { reset(); }
  int enter() {
    __sync_sub_and_fetch(&enterCount, 1);
    while (enterCount != 0); // spin
    leave();
  }
};

struct BarrierTestWorkerArgs {
  pthread_t tid;
  int num;
  int id;
  Barrier *b;
};

void *barrierTestWorker(void *arg)
{
  TRACE("BEGIN: %p", arg);
  struct BarrierTestWorkerArgs *parms = (struct BarrierTestWorkerArgs *)arg;
  
  for (int i=0; i<2; i++) {
    if (parms->id != (parms->num - 1) ) {
      TRACE("%d: Not last worker entering Barrier %p\n", parms->id, parms->b);
      parms->b->enter();
      TRACE("%d: Not last worker LEFT Barrier %p\n", parms->id, parms->b);
    } else {
      TRACE("%d: LAST worker Sleeping %p\n", parms->id, parms->b);
      for (volatile int i=0; i<1000000000; i++);
      TRACE("%d: Last worker entering Barrier %p\n", parms->id, parms->b);
      parms->b->enter();
      TRACE("%d: Last worker LEFT Barrier %p\n", parms->id, parms->b);
    }
    TRACE("%d: Worker doing test AGAIN\n", parms->id, parms->b);   
  }
  TRACE("END: %p", arg);
  return NULL;
}

void 
BarrierTest(int numWorkers)
{
  TRACE("BEGIN: %d\n", numWorkers);
  int i;
  struct BarrierTestWorkerArgs *args;
  Barrier bar(numWorkers);

  args = (struct BarrierTestWorkerArgs *)
    malloc(sizeof(struct BarrierTestWorkerArgs) * numWorkers);  
  tassert((args != NULL), ass_printf("malloc failed\n"));

  for (i=0; i<numWorkers; i++) {
    TRACE("crearing i=%d\n", i);
    args[i].num = numWorkers;
    args[i].id = i;
    args[i].b = &bar;
    if ( pthread_create( &(args[i].tid), NULL, 
			 barrierTestWorker, (void *)&(args[i])) != 0) {
      perror("pthread_create");
      exit(-1);
    }
      
  }

  for (i = 0; i<numWorkers; i++)
    pthread_join(args[i].tid, NULL );
  
  free(args);
  
  TRACE("END\n");
}

void
SSACSimpleSharedArrayTest(void)
{
  EBBRC status;
  TRACE("BEGIN");

  const int HASHTABLESIZE=128;
  CacheObjectIdSimple id(0);
  CacheEntrySimple *entry=0;

  SSACRef ssac = SSACSimpleSharedArray::create(HASHTABLESIZE);

  status=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
			 SSAC::GETFORREAD);
  entry->print();

  TRACE("END");
}

int 
main(int argc, char **argv)
{
  MySSAC *ssac;
  EBBRC rc;
  rc = MySSAC::create(&ssac);

  if (rc == 0) {
    ssac->flush();
  } else {
    fprintf(stderr, "ERROR: create failed\n");
  }

  SSACSimpleSharedArrayTest();

  BarrierTest(4);

  return 0;
}
