#ifndef __EBB_CTR_PRIM_DISTRIBUTED_H__
#define __EBB_CTR_PRIM_DISTRIBUTED_H__

#include "CObjEBBRootMulti.h"

CObject(EBBCtrPrimDistributed) {
  CObjInterface(EBBCtr) *ft;
  uval localValue;
  CObjEBBRootMultiRef theRoot;
};

extern CObjInterface(EBBCtr) EBBCtrPrimDistributed_ftable;

static inline void
EBBCtrPrimDistributedSetFT(EBBCtrPrimDistributedRef o)
{
  o->ft = &EBBCtrPrimDistributed_ftable;
}

typedef EBBCtrPrimDistributedRef *EBBCtrPrimDistributedId;
extern EBBRC EBBCtrPrimDistributedCreate(EBBCtrPrimDistributedId *id);

#endif
