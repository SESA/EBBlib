#ifndef __EBB_ROOT_SHARED_H__
#define __EBB_ROOT_SHARED_H__

CObjInterface(EBBRootShared)
{
  CObjImplements(EBBRoot);
  void (*init)(void *_self, void *rep);
};

CObject(EBBRootShared) 
{
  CObjInterface(EBBRootShared) *ft;
  void *theRep;
};

extern CObjInterface(EBBRootShared) EBBRootShared_ftable;

static inline void
EBBRootSharedSetFT(EBBRootSharedRef o) { o->ft = &EBBRootShared_ftable; }

#endif
