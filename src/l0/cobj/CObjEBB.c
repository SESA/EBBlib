#include <config.h>
#include <types.h>
#include <l0/cobj/cobj.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/cobj/CObjEBBRoot.h>

EBBRC 
CObjEBBMissFunc(void *_self, EBBLTrans *lt, FuncNum fnum, EBBMissArg arg)
{
  return ((CObjEBBRootRef)arg)->ft->handleMiss((void *)arg, _self, lt, fnum);
}
