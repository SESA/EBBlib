#include <stdio.h>
#include "../types.h"
#include "counter.h"
#include "counter_1.h"

int main () {
  Counter_1 theObj;
  Counter_1Ref ref = &theObj;

  ref->ftable = &Counter_1_ftable;

  CounterRef absRef = (CounterRef)ref;
  printf("val = %ld\n", absRef->ftable->val(absRef));
  absRef->ftable->inc(absRef);
  printf("val = %ld\n", absRef->ftable->val(absRef));

  return 0;
}
