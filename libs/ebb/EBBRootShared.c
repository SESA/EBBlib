#include "../base/types.h"
#include "../cobj/cobj.h"
#include "EBBRoot.H"
#include "EBBRootShared.H"

uval
EBBRootShared_handleMiss(void *_self)
{
  EBBRootSharedRef self = _self;
  return (uval)(self->theRep);
}

void
EBBRootShared_init(void *_self, void *rep)
{
  EBBRootSharedRef self = _self;
  self->theRep = rep;
}

CObjInterface(EBBRootShared) EBBRootShared_ftable = {
  { EBBRootShared_handleMiss },
  EBBRootShared_init
};
