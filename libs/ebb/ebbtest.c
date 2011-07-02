#include "../cobj/cobj.h"
#include "sys/trans.h" //FIXME: move EBBTransLSys out of this header
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "EBBMgrPrim.h"
#include "EBBCtr.h"
#include "EBBCtrPrim.h"

#include <stdio.h>

void 
EBBCtrTest(void)
{
  EBBCtrPrimId c;
  EBBRC rc;
  uval v;

  EBBCtrPrimSharedCreate(&c);

  printf("id=%p\n", c);
  rc = EC(c)->val(EB(c), &v);

  printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c)->inc(EB(c)); rc = EC(c)->val(EB(c), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c)->inc(EB(c)); rc = EC(c)->val(EB(c), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c)->inc(EB(c)); rc = EC(c)->val(EB(c), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c)->dec(EB(c)); rc = EC(c)->val(EB(c), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);
}


int main () {
  EBBId id1, id2;
  EBBRC rc;

  EBBMgrPrimInit();

  printf("0: EBBId_DREF(theEBBMgrPrimId)=%p: ", EBBId_DREF(theEBBMgrPrimId));
  rc = EBBAllocPrimId(&id1);
  printf("rc = %ld id1=%p\n", rc, id1);

  printf("1: EBBId_DREF(theEBBMgrPrimId)=%p: ", EBBId_DREF(theEBBMgrPrimId));
  rc = EBBAllocPrimId(&id2);
  printf("rc = %ld id2=%p\n", rc, id2);

  EBBCtrTest();

  return 0;
}
