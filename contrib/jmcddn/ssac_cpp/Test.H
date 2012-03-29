#ifndef __TEST_H__
#define __TEST_H__

#define OUTPUT_CSV

inline unsigned long long
now(void)
{
  unsigned int a,d; // 32 bits
  // The instruction "RDTSC" returns the TSC in EDX:EAX.
  __asm__ __volatile__("rdtsc" : "=a" (a), "=d" (d));
  return ((unsigned long long)a) | (((unsigned long long)d) << 32);
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
  void init(int val) { size=val; reset(); }
  Barrier() { init(0); }
  Barrier(int val)  { init(val); }
  int enter() {
    while(leaveCount != size); // ensure barrier is empty for reuse
    __sync_sub_and_fetch(&enterCount, 1);
    while (enterCount != 0); // FIXME: better-than-spinwait
    leave();
    return 0;
  }
};

class Test {
  static int num_psy_cores(void);
  friend void *testPThreadFunc(void *);
#ifndef __APPLE__
  friend void *linux_thread_init(void *args, int processor);
#endif
  friend int create_bound_thread (pthread_t *tid, int id, void *(*func)(void *), void *arg);
private:
  EBBRC worker(int id);
  EBBRC doWork();
protected:
  Barrier bar;
#ifndef __APPLE__
  pthread_barrier_t bar2;
#endif
  int numWorkers;
  int iterations;
  int numEvents;
  bool bindThread;
  double writePct;
  struct WArgs {
    int id;
    unsigned long long start;
    unsigned long long end;
    Test *theTest;
  } *wargs;
 virtual EBBRC init(void)=0;
 virtual EBBRC work(int id)=0;
 virtual EBBRC end(void);
public:
  Test(int n, int m, int c, bool p, double wpct);
  EBBRC doTest(void) {
    if (init()<0) return -1;
    doWork();
    end();
    return 0;
  }
  virtual ~Test();
};


#endif
