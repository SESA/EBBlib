#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libs/base/types.h"
#include "../../libs/cobj/cobj.h"
#include "../../libs/ebb/EBBTypes.h"
#include "../../libs/ebb/CObjEBB.h"
#include "../../libs/ebb/EBBMgrPrim.h"
#include "../../libs/ebb/EBBMemMgr.h"
#include "../../libs/ebb/EBBMemMgrPrim.h"
#include "../../libs/ebb/EBBCtr.h"
#include "../../libs/ebb/EBBCtrPrim.h"
#include "../../libs/ebb/EBBCtrPrimDistributed.h"
#include "../../libs/ebb/clrBTB.h"
#include <sys/time.h>
#include <unistd.h>
#include <sched.h>

pthread_key_t ELKey;
pthread_barrier_t ELBarrier;
pthread_t *threads;

long *sec;
long *usec;

EBBCtrPrimDistributedId c;

void *vp_thread(void *arg) {
  int i;
  uval v;
  EBBRC rc;
  struct timeval tv1, tv2;
  cpu_set_t set;
  
  pthread_setspecific(ELKey, arg);
  CPU_ZERO(&set);
  CPU_SET(LRTEBBMyEL(), &set);
  pthread_setaffinity_np(threads[LRTEBBMyEL()], sizeof(cpu_set_t), &set);
  pthread_barrier_wait(&ELBarrier);
  gettimeofday(&tv1, NULL);
  for(i = 0; i < 1000000000; i++) {
    rc = EC(c)->inc(EB(c));
  }
  gettimeofday(&tv2, NULL);
  sec[LRTEBBMyEL()] = tv2.tv_sec - tv1.tv_sec;
  usec[LRTEBBMyEL()] = tv2.tv_usec - tv1.tv_usec;
  if (usec[LRTEBBMyEL()] < 0) {
    sec[LRTEBBMyEL()]--;
    usec[LRTEBBMyEL()] += 1000000;
  }
}
		
int main () {
  long i;
  long numCpu = sysconf(_SC_NPROCESSORS_ONLN);
/*   long numCpu = 1; */

  threads = malloc(sizeof(pthread_t)*numCpu);
  sec = malloc(sizeof(long)*numCpu);
  usec = malloc(sizeof(long)*numCpu);

  EBBMgrPrimInit();
  EBBMemMgrPrimInit();

  pthread_barrier_init(&ELBarrier, NULL, numCpu);
  pthread_key_create(&ELKey, NULL);
  EBBCtrPrimDistributedCreate(&c);
  for (i = 0; i < numCpu; i++) {
    sec[i] = 0;
    usec[i] = 0;
    pthread_create(&threads[i], NULL, vp_thread, (void *)i);
  }
  for (i = 0; i < numCpu; i++) {
    pthread_join(threads[i], NULL);
    printf("%ld.%.6ld\n",
	   sec[i], usec[i]);
  }
}
