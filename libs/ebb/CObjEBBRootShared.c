#include "../base/types.h"
#include "../cobj/cobj.h"
#include "EBBTypes.H"
#include "CObjEBB.h"
#include "CObjEBBRoot.h"
#include "CObjEBBRootShared.h"

uval
CObjEBBRootShared_handleMiss(void *_self, void *obj, EBBLTrans *lt, 
			     FuncNum fnum)
{
  CObjEBBRootSharedRef self = _self;
  return (uval)(self->theRep);
}
 
void
CObjEBBRootShared_init(void *_self, void *rep)
{
  CObjEBBRootSharedRef self = _self;
  self->theRep = rep;
}

CObjInterface(CObjEBBRootShared) CObjEBBRootShared_ftable = {
  { CObjEBBRootShared_handleMiss },
  CObjEBBRootShared_init
};
