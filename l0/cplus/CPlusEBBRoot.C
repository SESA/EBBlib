#include <config.h>
#include <stdint.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/cobj/cobj.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cplus/CPlusEBB.H>
#include <l0/cplus/CPlusEBBRoot.H>

/* static */ EBBRC 
CPlusEBBRoot::CPlusEBBMissFunc(EBBRep **rep, EBBLTrans *lt, FuncNum fnum, 
			       EBBMissArg arg)
{
  return ((CPlusEBBRoot *)arg)->handleMiss((CPlusEBB **)rep, lt, fnum);
}

/* static */ EBBRC 
CPlusEBBRoot::EBBBind(EBBId id, CPlusEBBRoot *root) {
  return EBBBindPrimId(id, CPlusEBBMissFunc, (EBBMissArg) root);
};

