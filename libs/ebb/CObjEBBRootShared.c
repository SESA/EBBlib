#include "../base/types.h"
#include "../cobj/cobj.h"
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "CObjEBBRoot.h"
#include "CObjEBBRootShared.h"

//What you want to do here is install theRep into the EBBLTrans
//then set obj to point to theRep (obj is really of type Object **)
//Then simply return EBBRC_OK and the default func will
//handle the rest. Return any failure code to have the call fail
uval
CObjEBBRootShared_handleMiss(void *_self, void *obj, EBBLTrans *lt, 
			     FuncNum fnum)
{
  CObjEBBRootSharedRef self = _self;
  EBBCacheObj(lt, self->theRep);
  *(void **)obj = self->theRep;
  return EBBRC_OK;
}

//Jonathan's code
/* uval */
/* CObjEBBRootShared_handleMiss(void *_self, void *obj, EBBLTrans *lt,  */
/* 			     FuncNum fnum) */
/* { */
/*   CObjEBBRootSharedRef self = _self; */
/*   return (uval)(self->theRep); */
/* } */
 
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
