#ifndef __COBJ_EBB_UTILS_H__
#define __COBJ_EBB_UTILS_H__

static inline
EBBRC
CObjEBBBind(void *id, void *root)
{
  return EBBBindPrimId(id, CObjEBBMissFunc, (EBBMissArg) root);
}

#endif
