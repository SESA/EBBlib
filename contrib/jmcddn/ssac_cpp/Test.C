#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "EBBKludge.H"
#include "Test.H"

struct TestPThreadArgs {
  pthread_t tid;
  int id;
  int index;
  Test *test;
};

void *
testPThreadFunc(void *args)
{
  struct TestPThreadArgs *arg = (struct TestPThreadArgs *)args;
  return  (void *) (long) arg->test->worker(arg->index);
}

Test::Test(int n) :  bar(n), numWorkers(n), iterations(1)
{
  wargs = (struct Test::WArgs *)
  malloc(sizeof(struct Test::WArgs) * numWorkers);  
  tassert((wargs != NULL), ass_printf("malloc failed\n"));
}

Test::Test(int n, int m) :  bar(n), numWorkers(n), iterations(m)
{
  wargs = (struct Test::WArgs *)
  malloc(sizeof(struct Test::WArgs) * (numWorkers * iterations));  
  tassert((wargs != NULL), ass_printf("malloc failed\n"));
}

EBBRC 
Test::doWork() {
  struct TestPThreadArgs *args;
  int i,j;

  // test iteration loop
  for(j=0; j<iterations; j++){ 
    args = (struct TestPThreadArgs *)
    malloc(sizeof(struct TestPThreadArgs) * numWorkers);  
    tassert((args != NULL), ass_printf("malloc failed\n"));
   for (i=0; i<numWorkers; i++) {
      TRACE("creating thread #%d\n", i);
      args[i].id = i;
      args[i].index = (j*numWorkers)+i;
      args[i].test = this; // TODO: look up behavior of 'this'
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
  return 0;
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

  return 0;
}


Test::~Test()
{
  free(wargs);
}

/* virtual */ EBBRC
Test::end()
{
  int index;
  printf("Test, Process, Start, End, Dif\n");
  for (int i=0; i<iterations; i++)
    for (int j=0; j<numWorkers; j++){
      index = (i*numWorkers)+j;
      printf("%d, %d, %llu, %llu, %llu\n", i, j, wargs[index].start, wargs[index].end, wargs[index].end - wargs[index].start);
    }
  
  return 0;
}
