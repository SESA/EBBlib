#ifndef __COBJ_EBB_ROOT_H_
#define __COBJ_EBB_ROOT_H_

CObjInterface(CObjEBBRoot) 
{
  uval (*handleMiss)(void *_self, void *obj, EBBLTrans *lt, FuncNum fnum);
};

CObject(CObjEBBRoot)
{
  CObjInterface(CObjEBBRoot) *ft;
};
#endif

