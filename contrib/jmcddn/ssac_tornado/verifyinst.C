/******************************************************************************
 *
 *                           Tornado:[tests]
 *
 *                       University of Toronto
 *                        Copyright 1994,1995
 *
 *      This software is free for all non-commercial use, and anyone in this
 * category may freely copy, modify, and redistribute any portion of it,
 * provided they retain this disclaimer and author list.
 *      The authors do not accept responsibility for any consequences of
 * using this program, nor do they in any way guarantee its correct operation.
 *
 * $Id: verifyinst.C,v 1.1 1998/01/20 21:18:05 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: main test driver
 * **************************************************************************/
#include <unistd.h>
#include <tornado/sys/WellKnown.H>
#include <tornado/sys/kernelcalls.h>
#include <tornado/TAssert.H>
#include "objtrans/ot.H"
#include "misc/ObjectRefs.H"
#include "misc/misc.h"
#include "objtrans/GOBJ.H"
#include <StubXAppl.H>
#include "misc/exception.H"
#include "lock/Barrier.H"

SpinBarrier *mybar;

void
worker()
{
    volatile int i=0;
    volatile int j=2;
    volatile SpinBarrier *foo;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
    i++;
    j=j+i;
    foo=mybar;
    foo+=j;
}

void
driver()
{
    register int vp;
    StubXAppl appl(myAppl->getOH());
    
    mybar=new SpinBarrier(NUMPROC); 
    GlobalUpdate((void **)&mybar);

    // start a perprocessor test on all processors
    for (vp=0;vp<NUMPROC;vp++) {
	// run tests asynchronously all processors skipping this processor
	if (vp!=MYVP) {
	    appl.setVP(vp);
	    appl.createProcess1( (tstatusfunc) worker,
				 (reg_t) 0);
	}
    }
    // run test on this processor and wait for it to complete before going on.
    // since all tests use a barrier this test will only return when all tests 
    // on all processors have completed
    appl.setVP(MYVP);
    appl.createProcess1AndWait( (tstatusfunc) worker, (reg_t) 0);
    delete mybar;
}

void
test()
{
  driver();
}
