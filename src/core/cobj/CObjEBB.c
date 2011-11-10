#include <config.h>
#include <types.h>
#include <core/cobj/cobj.h>
#include <core/types.h>
#include <core/cobj/CObjEBB.h>
#include <core/cobj/CObjEBBRoot.h>

EBBRC 
CObjEBBMissFunc(void *_self, EBBLTrans *lt, FuncNum fnum, EBBMissArg arg)
{
  return ((CObjEBBRootRef)arg)->ft->handleMiss((void *)arg, _self, lt, fnum);
}
