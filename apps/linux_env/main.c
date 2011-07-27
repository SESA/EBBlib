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

pthread_key_t ELKey;
pthread_barrier_t ELBarrier;

EBBCtrPrimId c;

void *vp_thread(void *arg) {
  uval v;
  EBBRC rc;
  pthread_setspecific(ELKey, arg);
  pthread_barrier_wait(&ELBarrier);
  
  rc = EC(c)->inc(EB(c));

  pthread_barrier_wait(&ELBarrier);
  rc = EC(c)->val(EB(c), &v);
  printf("EL = %ld, c = %ld\n",
	 LRTEBBMyEL(), v);
}
		
int main () {
  long i;
  pthread_t threads[LRT_MAX_VPS];

  EBBMgrPrimInit();
  EBBMemMgrPrimInit();

  pthread_barrier_init(&ELBarrier, NULL, LRT_MAX_VPS+1);
  pthread_key_create(&ELKey, NULL);
  for (i = 0; i < LRT_MAX_VPS; i++) {
    pthread_create(&threads[i], NULL, vp_thread, (void *)i);
  }
  pthread_barrier_wait(&ELBarrier);
  EBBCtrPrimSharedCreate(&c);
  pthread_barrier_wait(&ELBarrier);
}
