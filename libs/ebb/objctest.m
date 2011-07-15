#include "objc/Object.h"

#include "../base/types.h"

#include "../cobj/cobj.h"
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "EBBMgrPrim.h"
#include "EBBCtr.h"
#include "EBBCtrPrim.h"

#include <stdio.h>

@interface EBBObject : Object
{
}
+ alloc;
@end

@implementation EBBObject
+ (id) alloc
{
  return [super alloc];
}
@end

@interface Counter : EBBObject
{
  uval val;
}

- (EBBRC) init: (Counter **) c;
- (EBBRC) inc;
- (EBBRC) dec;
- (EBBRC) val: (uval *)v;

@end


@implementation Counter

- (EBBRC) init: (Counter **) c 
{
  self = [super init];
  val = 0;
  *c = self;
  return EBBRC_OK;
}

- (EBBRC) inc
{
  val++;
  return EBBRC_OK;
}

- (EBBRC) dec
{
  val--;
  return EBBRC_OK;
}

- (EBBRC) val: (uval *)v 
{
  *v = val;
   return EBBRC_OK;
}
@end



void 
EBBCtrTest(void)
{
  Counter *c;
  EBBRC rc;

  uval v;
  uval i;

  rc = [[Counter alloc] init:&c];

  printf("id=%p\n", c);
  rc = [c val: &v];
  printf("rc=%ld, v=%ld\n", rc, v);

#if 0
  rc = [c inc]; rc = [c val: &v];  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = [c inc]; rc = [c val: &v];  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = [c inc]; rc = [c val: &v];  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = [c dec]; rc = [c val: &v];  
  printf("rc=%ld, v=%ld\n", rc, v);

#endif

#if 1
  c = NULL;
#endif

  for (i=0; i<1000000000; i++ ) {

    rc = [c inc];

    if (!EBBRC_SUCCESS(rc)) printf("error\n");
  }

  rc = [c val: &v];
  printf("i=%ld rc=%ld, v=%ld\n", i, rc, v);
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
