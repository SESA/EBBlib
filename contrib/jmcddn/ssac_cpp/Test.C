#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#ifdef __APPLE__
#include <sys/sysctl.h>
#include <mach/mach_interface.h>
#include <mach/thread_policy.h>
#endif

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

int
static num_phys_cores()
{
#ifdef __APPLE__
  // based on doc I could find on net about OSX/mach internals
  int mib[4], numcores;
  size_t len, size;
  
  len = 4;
  sysctlnametomib("hw.physicalcpu_max", mib, &len);

  size = sizeof(numcores);
  if (sysctl(mib, len, &numcores, &size, NULL, 0)==-1) {
	perror("sysctl");
	return -1;
  }
  return numcores;
#else 
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

#ifndef __APPLE__
  struct linux_thread_init_arg {
    void *(*func)(void *);
    void *args;
    int proc; 
  };

void *
linux_thread_init(void *arg)
{
  struct linux_thread_init_arg *a = (struct linux_thread_init_arg *)arg;
  void *(*func)(void *) = a->func;
  int processor = a->proc;
  cpu_set_t mask; 
  size_t mask_size; 
  // Pin process to cpu
  CPU_ZERO( &mask );
  CPU_SET(processor, &mask);
  if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) == -1) {//sched_setaffinity(0, mask_size,mask) == -1) {
    perror("ERROR: Could not set CPU Affinity, exiting...\n");
    exit(-1);
  }
  return func(a->args); 
}
#endif

int
create_bound_thread(pthread_t *tid, int id,  void *(*func)(void *), void *arg)
{ 
  int numcores, pid, rc;
  numcores = num_phys_cores();
  pid = id % numcores;  
  
  if ((id < 0)) return -1;

#ifdef __APPLE__
  // Apple bind code 
  thread_affinity_policy_data_t affinityinfo;
  rc = pthread_create_suspended_np(tid, NULL, func, arg);// TODO: verify value of arg is correct for OSX
  if (rc != 0) {
    perror("pthread_create_suspended_np");
    return -1;
  }
  affinityinfo.affinity_tag = pid+1; // why +1? 

  rc = thread_policy_set(pthread_mach_thread_np(*tid), 
			 THREAD_AFFINITY_POLICY,
			 (int *)&affinityinfo,
			 THREAD_AFFINITY_POLICY_COUNT);
  if (rc != KERN_SUCCESS) {
    perror("thread_policy_set");
    return -1;
  }
  thread_resume(pthread_mach_thread_np(*tid));
#else

  // Linux bind code 
  struct linux_thread_init_arg *lnxargs; 
  lnxargs = (linux_thread_init_arg *)malloc(sizeof(linux_thread_init_arg));// FIXME: memory leak... 
  lnxargs->func = func;
  lnxargs->args = arg;
  lnxargs->proc = pid;
  rc = pthread_create(tid, NULL, linux_thread_init, lnxargs);
  if (rc != 0) {
    perror("pthread_create");
    return -1;
  }

#endif
  return 0;
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

Test::Test(int n, int m, bool p) :  bar(n), numWorkers(n), iterations(m), bindThread(p)
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
//    TRACE("creating thread #%d\n", i);
      args[i].id = i;
      args[i].index = (j*numWorkers)+i;
      args[i].test = this; // TODO: look up behavior of 'this'
    // create bound threads if specified
    if (bindThread > 0){
      if ( create_bound_thread( &(args[i].tid), i, testPThreadFunc, (void *)&(args[i])) < 0) {
	    perror("pthread_create");
	    exit(-1);
      } 
    }else{ 
      if ( pthread_create( &(args[i].tid), NULL, testPThreadFunc, (void *)&(args[i])) != 0) {
	    perror("create_bound_thread");
	    exit(-1);
      }
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
