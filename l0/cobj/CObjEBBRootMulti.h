#ifndef __COBJ_EBB_ROOT_MULTI_H__
#define __COBJ_EBB_ROOT_MULTI_H__

#include <l0/cobj/CObjEBBRoot.h>

CObject(CObjEBBRootMulti);

typedef void *(*CreateRepFunc) (CObjEBBRootMultiRef rootRef);
typedef struct RepListNode_s RepListNode;

CObjInterface(CObjEBBRootMulti)
{
  CObjImplements(CObjEBBRoot);
  void (*init)(void *_self, CreateRepFunc func);
  RepListNode *(*nextRep) (void *_self, RepListNode *curr, void *rep);
};

CObjectDefine(CObjEBBRootMulti)
{
  CObjInterface(CObjEBBRootMulti) *ft;
  CreateRepFunc createRep;
  RepListNode *head;
};

extern CObjInterface(CObjEBBRootMulti) CObjEBBRootMulti_ftable;
				       
static inline void
CObjEBBRootMultiSetFT(CObjEBBRootMultiRef o)
{
  o->ft = &CObjEBBRootMulti_ftable;
}

#endif
