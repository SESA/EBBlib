#include "../types.h"
#include "myobj.h"

int main () {
  MyObj theObj;
  MyObjRef ref = &theObj;
  char buf[80];

  ref->ftable = &MyObj_ftable;

  ref->ftable->Print(ref);
  ref->ftable->Init(ref,10);
  ref->ftable->Counter_if.inc(ref);
  ref->ftable->File_if.open(ref, "testFile", 0); 
  ref->ftable->File_if.write(ref, "data", 4);
  ref->ftable->File_if.read(ref, buf, sizeof(buf));
  ref->ftable->File_if.close(ref);
  
  ref->ftable->Print();  
  
  return 0;
}
