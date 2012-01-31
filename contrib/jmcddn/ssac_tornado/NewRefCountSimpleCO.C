#include <tornado/TAssert.H>
#include "objtrans/ot.H"
#include "misc/ObjectRefs.H"
#include "objtrans/GOBJ.H"
#include "NewRefCountSimpleCO.H"
    
   
 
// Here is the RefCount Clustered Object
// methods.

RefCountSimpleCO :: RefCountSimpleCO(int start)
{
    tr_printf(">>>RefCountSimpleCO::RefCountSimpleCO(n=%lx) ref=%lx vp=%d\n",
	      _ref,MYVP);
    _localcount=start; 
}

RefCountSimpleCO :: ~RefCountSimpleCO()
{
    tr_printf(">>>***RefCountSimpleCO::~RefCountSimpleCO() ref=%lx vp=%d\n",
	      _ref,MYVP);
    return;
}

 
TornStatus
RefCountSimpleCO :: dec()
{
    _lock.acquireLock();
    tr_printf(">>>RefCountSimpleCO::dec called on ref=%lx vp=%d\n",
	      _ref,MYVP );
      _localcount--;
    _lock.releaseLock();
    return 0;
}
 
TornStatus
RefCountSimpleCO :: inc()
{
    _lock.acquireLock();
    tr_printf(">>>RefCountSimpleCO::inc called on ref=%lx vp=%d\n",
	      _ref,MYVP );
      _localcount++;
    _lock.releaseLock();
    return 0;
}


TornStatus
RefCountSimpleCO :: val(int& v)
{
    tr_printf(">>>RefCountSimpleCO::myrepval called on ref=%lx vp=%d\n",_ref,MYVP);
    v=_localcount;
    return 0;
}
 
