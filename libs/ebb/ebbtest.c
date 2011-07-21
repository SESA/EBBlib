#include "../base/types.h"
#include "../cobj/cobj.h"
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "EBBMgrPrim.h"
#include "EBBMemMgr.h"
#include "EBBMemMgrPrim.h"
#include "EBBCtr.h"
#include "EBBCtrPrim.h"
#include "clrBTB.h"

#include <stdio.h>

void 
EBBCtrTest(void)
{
  EBBCtrPrimId c;
  EBBRC rc;
  uval v;
  sval i,j;

  EBBCtrPrimSharedCreate(&c);

  printf("id=%p\n", c);
  rc = EC(c)->val(EB(c), &v);
  printf("rc=%ld, v=%ld\n", rc, v);

#if 1
  rc = EC(c)->inc(EB(c)); rc = EC(c)->val(EB(c), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c)->inc(EB(c)); rc = EC(c)->val(EB(c), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c)->inc(EB(c)); rc = EC(c)->val(EB(c), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c)->dec(EB(c)); rc = EC(c)->val(EB(c), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);
#endif

#if 0
  EBBCtrPrimRef r = EB(c);
  EBBRC (*f) (void *_self) = r->ft->inc;
  EBBRC (**ftbl) (void *_self) = &f;

  for (i=0; i<2; i++ ) {

#if 0
    clrBTB();
#endif

    for (j=0; j<2; j++) {
#if 0
      rc = inc(r);
#endif
      
#if 0
      rc = f(r);
#endif
      
#if 0
      rc = ftbl[0](r);
#endif
      
#if 0
      rc = r->ft->inc(r);
#endif
      
#if 0
      rc = EC(c)->inc(EB(c)); 
#endif

      if (!EBBRC_SUCCESS(rc)) printf("error\n");
    }
  }

  rc = EC(c)->val(EB(c), &v);
  printf("i=%ld j=%ld rc=%ld, v=%ld\n", i, j, rc, v);
#endif
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

  EBBMemMgrPrimInit();

  char *mem;
  EBBMalloc(4, &mem);
  printf("0: mem=%p\n", mem);
  EBBFree(mem);
  EBBMalloc(4, &mem);
  printf("1: mem=%p\n", mem);
  EBBFree(mem);

  EBBCtrTest();

  return 0;
}
