#include <tornado/TAssert.H>
#include "objtrans/ot.H"
#include "misc/ObjectRefs.H"
#include "objtrans/GOBJ.H"
#include "NewRefCountSharedCO.H"
    
   
 
// Here is the RefCount Clustered Object
// methods.

RefCountSharedCO :: RefCountSharedCO(int start):_mh(this)
{
    tr_printf(">>>RefCountSharedCO::RefCountSharedCO(n=%lx) ref=%lx vp=%d\n",
	      _ref,MYVP);
    _localcount=start; 
}

RefCountSharedCO :: ~RefCountSharedCO()
{
    tr_printf(">>>***RefCountSharedCO::~RefCountSharedCO() ref=%lx vp=%d\n",
	      _ref,MYVP);
    return;
}

 
TornStatus
RefCountSharedCO :: dec()
{
    _lock.acquireLock();
    tr_printf(">>>RefCountSharedCO::dec called on ref=%lx vp=%d\n",
	      _ref,MYVP );
      _localcount--;
    _lock.releaseLock();
    return 0;
}
 
TornStatus
RefCountSharedCO :: inc()
{
    _lock.acquireLock();
    tr_printf(">>>RefCountSharedCO::inc called on ref=%lx vp=%d\n",
	      _ref,MYVP );
      _localcount++;
    _lock.releaseLock();
    return 0;
}


TornStatus
RefCountSharedCO :: val(int& v)
{
    tr_printf(">>>RefCountSharedCO::myrepval called on ref=%lx vp=%d\n",_ref,MYVP);
    v=_localcount;
    return 0;
}
 
