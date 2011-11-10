#ifndef __COBJ_EBB_ROOT_SHARED_H__
#define __COBJ_EBB_ROOT_SHARED_H__

CObjInterface(CObjEBBRootShared)
{
  CObjImplements(CObjEBBRoot);
  void (*init)(void *_self, void *rep);
};

CObject(CObjEBBRootShared) 
{
  CObjInterface(CObjEBBRootShared) *ft;
  void *theRep;
};

extern CObjInterface(CObjEBBRootShared) CObjEBBRootShared_ftable;

static inline void
CObjEBBRootSharedSetFT(CObjEBBRootSharedRef o) 
{
  o->ft = &CObjEBBRootShared_ftable; 
}

#endif
