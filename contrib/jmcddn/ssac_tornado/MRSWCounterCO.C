#include <tornado/TAssert.H>
#include "objtrans/ot.H"
#include "misc/ObjectRefs.H"
#include "objtrans/GOBJ.H"
#include "MRSWCounterCO.H"

MRSWCounterCO :: MRSWCounterCO(int &v)
{
    tr_printf(">>>MRSWCounterCO::MRSWCounterCO().ref=%lx vp=%d\n",ref,MYVP);
    ival=v;
    first=(MRSWCounterCORep *)NULL;
    last=(MRSWCounterCORep *)NULL;
}

MRSWCounterCO :: ~MRSWCounterCO() {
    tr_printf(">>>MRSWCounterCO::MRSWCounterCO().ref=%lx vp=%d\n",ref,MYVP);
    return;
}

Obj *
MRSWCounterCO :: handleMiss(TransEntry *te, MHOEntry *mho, int op)
{
    // create a new representative
    Representative *rep=new MRSWCounterCORep(ref,first);

    if (first==NULL)
    {
	owner=first;
	first=(MRSWCounterCORep *)rep;
	rep->setval(ival);
    }
    else
    {
	last->next=(MRSWCounterCORep *)rep;
    }
    last=(MRSWCounterCORep *)rep;
    tr_printf(">>>MRSWCounterCO::handleMiss(%lx,%lx,%d) on vp=%d ref=%lx\n",
	      te, mho, op, MYPROC,ref);
    return Replicate::handleMiss(te,mho,op,rep);
}


// Here is the RefCount Clustered Object Representative's
// methods.

MRSWCounterCORep :: MRSWCounterCORep(ObjRef r,MRSWCounterCORep *n,int &v) : Representative(r)
{
    tr_printf(">>>MRSWCounterCORep::MRSWCounterCORep(r=%lx,n=%lx) vp=%d\n",r,n,MYVP);
    count=v;
    if (n==NULL) next=this;
    else next=n;
}

MRSWCounterCORep :: ~MRSWCounterCORep()
{
    tr_printf(">>>MRSWCounterCORep::~MRSWCounterCORep() ref=%lx vp=%d\n",ref,MYVP);
    return;
}


TornStatus
MRSWCounterCORep :: destroy()
{
    tr_printf(">>>MRSWCounterCORep::destroy called on vp=%d ref=%lx\n",MYVP,ref );
    DREF(GOBJ::TheObjTransRef())->cleanupRefObj( (ObjRef)ref,(Obj *)0 );
    return 0; 
}

TornStatus
MRSWCounterCORep :: dec()
{
    tr_printf(">>>MRSWCounterCORep::dec called on vp=%d ref=%lx\n",MYVP,ref );
    val(count-1);
    return 0;
}

TornStatus
MRSWCounterCORep :: inc()
{
    tr_printf(">>>MRSWCounterCORep::inc called on vp=%d ref=%lx\n",MYVP,ref );
    val(count+1);  
    return 0;
}

TornStatus
MRSWCounterCORep :: val(int& v)
{
    tr_printf(">>>MRSWCounterCORep::val called on vp=%d ref=%lx\n",MYVP,ref);
    v=0;
    slock.aquire();
    if (state==invalid) {
	count=owner->count;
	owner->slock.aquire();
	  owner->state=share;
	owner->slock.require();
	state=share;
    }
    slock.release();
    v=count;
    return 0;
}

void
MRSWCounterCORep :: invalidate()
{
    while
}
TornStatus
MRSWCounterCORep :: setval(int& v)
{
    tr_printf(">>>MRSWCounterCORep::myrepval called on vp=%d ref=%lx\n",MYVP,ref);
    slock.aquire();
    wlock->aquire();
    switch (state)
    {
    case exclusive:
	count=v;
	break;
    case shared:
    case invalid:
	state=exclusive;
	count=v;
	invalidate();
	break;
    }
    wlock->release();
    return 0;
}

