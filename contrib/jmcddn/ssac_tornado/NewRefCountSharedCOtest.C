#include <unistd.h>
#include <tornado/sys/WellKnown.H>
#include <tornado/sys/kernelcalls.h>
#include <tornado/TAssert.H>
#include "objtrans/ot.H"
#include "misc/ObjectRefs.H"
#include "objtrans/GOBJ.H"
#include <StubXAppl.H>
#include "misc/exception.H"
#include "lock/Barrier.H"
#include "NewRefCountSharedCO.H"

SpinBarrier *mybar;
      
void 
inctst(RefCountSharedCORef count, int num)
{
    int vp=MYVP;
    tr_printf("Doing inctst on vp %d: count=%lx num=%d\n",vp,count,num);

    IncActiveCalls();
    for(int i=0;i<num;i++) {
	DREF(count)->inc();    
    }
    {
	int v;
	DREF(count)->val(v);
	tr_printf("The local count on vp %d rep is %d\n",vp,v);
    }
    DecActiveCalls();
    mybar->enter();
    return;
}


void
driver()
{
    StubXAppl appl(myAppl->getOH());
    int vp;
//    long oldtm=TraceMask;
//    long oldtl=TraceLevel;
    
    tr_printf("Starting RefCountCO Test..\n");
    tr_printf("Creating a RefCountCO object...\n");

    TraceMask |= OT | LIBS;
    TraceLevel = TR_DEBUG;

     
    RefCountSharedCORef rcount=RefCountSharedCO::create();
    tr_printf("Object ID = %lx\n",rcount);

    mybar=new SpinBarrier(NUMPROC); 

    // start an increment test of the reference count on all processors
    for (vp=0;vp<NUMPROC;vp++) {
	// run tests asynchronously all processors skipping this processor
	if (vp!=MYVP) {
	    appl.setVP(vp);
	    appl.createProcess( (tstatusfunc) inctst, (reg_t) rcount, (reg_t) vp+1, (reg_t) 0,
				(reg_t) 0, (reg_t)0,(reg_t) 0);
	}
    }
    // run increment test on this processor and wait for it to complete before going on.
    // since all tests use a barrier this test will only return when all tests on all
    // processors have completed
    appl.setVP(MYVP);
    appl.createProcessAndWait( (tstatusfunc) inctst, (reg_t) rcount, (reg_t) MYVP+1, (reg_t) 0,
			       (reg_t) 0, (reg_t)0,(reg_t) 0);
    delete mybar;
    
    // get global value
    {
	int v;
	IncActiveCalls();
	DREF(rcount)->val(v);
	DecActiveCalls();
	tr_printf("System wide value of rcount->val()=%d\n",v);
    }

    //*** This only needs to be done once per CO
    // The object Translation system will ensure that all processors
    // are cleaned up as necessary.
    tr_printf("About to destroy the rcount.\n");
    IncActiveCalls();
    DREF(rcount)->destroy();
    DecActiveCalls();
    
    tr_printf("All done\n");
}

void
test()
{
    int an;
    while (1) {
	tr_printf("\n Press  key to start the test (e to exit)");
	while( (an= PeekChar()) == 0);
	if ( an == 'e' ) break;
	driver();
    }
}

