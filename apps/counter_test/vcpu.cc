#include <ebb.h>
#include <counter.h>
#include <l4io.h>
#include <l4/kdebug.h>

extern Counter *counter;

void vcpu_func() {
  int i;
  for (i = 0; i < 1000; i++) {
    INVK(counter, inc);
  }
  printf("val = %d\n", INVK(counter,val));
  while(1);
}
