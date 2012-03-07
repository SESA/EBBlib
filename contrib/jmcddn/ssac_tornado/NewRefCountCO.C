#include <tornado/TAssert.H>
#include "objtrans/ot.H"
#include "misc/ObjectRefs.H"
#include "objtrans/GOBJ.H"
#include "NewRefCountCO.H"
     
// static construct method
RefCountCORef
RefCountCO :: construct(int i)
{
    return (new RefCountCOMH(i))->ref();
}
   

// Here is the RefCount Clustered Object
// methods.

RefCountCO :: RefCountCO(RefCountCO *n,int start) 
{
    tr_printf(">>>RefCountCO::RefCountCO(n=%lx) ref=%lx vp=%d\n",
	      n,_ref,MYVP);
    _localcount=start; 
    if (n==NULL) _next=this;
    else _next=n;
}

RefCountCO :: ~RefCountCO()
{
    tr_printf(">>>***RefCountCO::~RefCountCO() ref=%lx vp=%d\n",
	      _ref,MYVP);
    return;
}


TornStatus
RefCountCO :: dec()
{
    _lock.acquireLock();
    tr_printf(">>>RefCountCO::dec called on ref=%lx vp=%d\n",
	      _ref,MYVP );
      _localcount--;
    _lock.releaseLock();
    return 0;
}

TornStatus
RefCountCO :: inc()
{
    _lock.acquireLock();
    tr_printf(">>>RefCountCO::inc called on ref=%lx vp=%d\n",
	      _ref,MYVP );
      _localcount++;
    _lock.releaseLock();
    return 0;
}

TornStatus
RefCountCO :: val(int& v)
{
    tr_printf(">>>RefCountCO::val called on ref=%lx vp=%d\n",
	      _ref,MYVP );
    v=0;
    
    for (RefCountCO *p=_next;p!=this;p=p->_next) 
	v+=p->_localcount;

    v+=_localcount;
    return 0;
}

TornStatus
RefCountCO :: myrepval(int& v)
{
    tr_printf(">>>RefCountCO::myrepval called on ref=%lx vp=%d\n",_ref,MYVP);
    v=_localcount;
    return 0;
}

// The following are the 

RefCountCO :: RefCountCOMH :: RefCountCOMH(int i)
{
    tr_printf(">>>RefCountCOMH::RefCountCOMH(int i) i=%d ref=%lx vp=%d\n",
	      i,_ref,MYVP);
    _first=(RefCountCO *)NULL;
    _last=(RefCountCO *)NULL;
    _initialvalue=i;
}

RefCountCO :: RefCountCOMH :: ~RefCountCOMH() {
    tr_printf(">>>***RefCountCOMH::~RefCountCOMH().ref=%lx vp=%d\n",_ref,MYVP);
    return;
}

ClusteredObject *
RefCountCO :: RefCountCOMH :: createFirstRep()
{
    // create a new representative
    ClusteredObject *rep=new RefCountCO(_first,_initialvalue);

    tr_printf(">>>RefCountCOMH::createFirststRep()on vp=%d ref=%lx\n",
	      MYPROC,_ref);
    _first=(RefCountCO *)rep;
    _last=(RefCountCO *)rep;

    return rep;
}

ClusteredObject *
RefCountCO :: RefCountCOMH :: createRep()
{
    // create a new representative
    ClusteredObject *rep=new RefCountCO(_first,_initialvalue);
    
    tr_printf(">>>RefCountCOMH::createRep() on vp=%d ref=%lx\n",
	      MYPROC,_ref);    
    _last->_next=(RefCountCO *)rep;
    _last=(RefCountCO *)rep;

    return rep;
}



