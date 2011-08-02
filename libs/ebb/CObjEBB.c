#include "../base/include.h"
#include "../base/types.h"
#include "../cobj/cobj.h"
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "CObjEBBRoot.h"

EBBRC 
CObjEBBMissFunc(void *_self, EBBLTrans *lt, FuncNum fnum, EBBMissArg arg)
{
  return ((CObjEBBRootRef)arg)->ft->handleMiss((void *)arg, _self, lt, fnum);
}
