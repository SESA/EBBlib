#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "EBBKludge.H"
#include "Test.H"

struct TestPThreadArgs {
  pthread_t tid;
  int id;
  Test *test;
};

void *
testPThreadFunc(void *args)
{
  struct TestPThreadArgs *arg = (struct TestPThreadArgs *)args;
  return (void *) arg->test->worker(arg->id);
}

Test::Test(int n) : numWorkers(n), iterations(0), bar(n) 
{
  wargs = (struct Test::WArgs *)
    malloc(sizeof(struct Test::WArgs) * numWorkers);  
  tassert((wargs != NULL), ass_printf("malloc failed\n"));
}

EBBRC 
Test::doWork() {
  struct TestPThreadArgs *args;
  int i;

  args = (struct TestPThreadArgs *)
    malloc(sizeof(struct TestPThreadArgs) * numWorkers);  
  tassert((args != NULL), ass_printf("malloc failed\n"));

  for (i=0; i<numWorkers; i++) {
    TRACE("creating i=%d\n", i);
    args[i].id = i;
    args[i].test = this;
    if ( pthread_create( &(args[i].tid), NULL, 
			 testPThreadFunc, (void *)&(args[i])) != 0) {
      perror("pthread_create");
      exit(-1);
    }
  }

  for (i = 0; i<numWorkers; i++)
    pthread_join(args[i].tid, NULL );

  free(args);
}

EBBRC
Test::worker(int id)
{
  struct WArgs *args = &(wargs[id]);

  bar.enter();

  args->start = now();
  work(id);
  args->end = now();

  bar.enter();
}


Test::~Test()
{
  free(wargs);
}

/* virtual */ EBBRC
Test::end()
{
  for (int i=0; i<numWorkers; i++) {
    printf("%d: start=%llu end=%llu\n", i, wargs[i].start, wargs[i].end);
  }
  return 0;
}
