#include "EBBRoot.H"

static EBBRC 
EBBCObjMissFunc(void *_self, EBBLTrans *lt, FuncNum fnum, EBBMissArg arg)
{
  return ((RootRef)arg)->handleMiss(root, _self, lt, fnum);
}

