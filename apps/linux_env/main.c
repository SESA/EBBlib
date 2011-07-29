#include <pthread.h>
#include <stdio.h>

#include "../../libs/ebb/lrt/ulnx/EBBConst.h"
#include "../../libs/base/types.h"
#include "../../libs/cobj/cobj.h"
#include "../../libs/ebb/EBBTypes.h"
#include "../../libs/ebb/CObjEBB.h"
#include "../../libs/ebb/EBBMgrPrim.h"
#include "../../libs/ebb/EBBMemMgr.h"
#include "../../libs/ebb/EBBMemMgrPrim.h"
#include "../../libs/ebb/EBBCtr.h"
#include "../../libs/ebb/EBBCtrPrim.h"
#include "../../libs/ebb/clrBTB.h"
#include <sys/time.h>

pthread_key_t ELKey;
pthread_barrier_t ELBarrier;

long sec[LRT_MAX_VPS];
long usec[LRT_MAX_VPS];

EBBCtrPrimId c;

void *vp_thread(void *arg) {
  int i;
  uval v;
  EBBRC rc;
  struct timeval tv1, tv2;
  
  pthread_setspecific(ELKey, arg);
  pthread_barrier_wait(&ELBarrier);
  gettimeofday(&tv1, NULL);
  for(i = 0; i < 100000000; i++) {
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
  pthread_t threads[LRT_MAX_VPS];

  EBBMgrPrimInit();
  EBBMemMgrPrimInit();

  pthread_barrier_init(&ELBarrier, NULL, LRT_MAX_VPS);
  pthread_key_create(&ELKey, NULL);
  EBBCtrPrimSharedCreate(&c);
  for (i = 0; i < LRT_MAX_VPS; i++) {
    sec[i] = 0;
    usec[i] = 0;
    pthread_create(&threads[i], NULL, vp_thread, (void *)i);
  }
  for (i = 0; i < LRT_MAX_VPS; i++) {
    pthread_join(threads[i], NULL);
    printf("%ld.%ld\n",
	   sec[i], usec[i]);
  }
}
