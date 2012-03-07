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
 * $Id: scenario6a.C,v 1.1 1997/11/11 18:47:07 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: main test driver
 * **************************************************************************/
#include <unistd.h>
#include <tornado/sys/WellKnown.H>
#include <tornado/TAssert.H>
#include "misc/misc.h"
#include "misc/GLocal.H"
#include <StubXAppl.H>
#include "lock/Barrier.H"
#include "testAndDecSharedCounter.H"

#include "scenario6a.H"


BlockBarrier *tstbar;
BlockBarrier *startworkerbar;


class Enterance {
    testAndDecSharedCounter *F;
public:
    Enterance(testAndDecSharedCounter *f) { F=f;}
    void enter();
    void exit();
    void admitcar();
    void turncaraway();
};


void
Enterance::enter()
{
    if (F->testAndDecrement())
	admitcar();
    else
	turncaraway();
}

void
Enterance::exit()
{
    F->increment();
    tr_printf("Car Exited at Enterance %d\n",MYVP);
}


void
Enterance::admitcar()
{
    tr_printf("Car Admitted at Enterance %d\n",MYVP);
}

void
Enterance::turncaraway()
{
    tr_printf("Car Turned away at Enterance %d\n",MYVP);
}

void
worker(reg_t counter)
{
    register int i,j;
    
    Enterance E((testAndDecSharedCounter *)counter);
    for (i=0;i<ITERATIONSPERWORKER;i++) {
	for (j=0;j<CARSINPERITERATION;j++) 
	    E.enter();
	for (j=0;j<CARSOUTPERITERATION;j++)
	    E.exit();
    }
}

void
startworker(reg_t counter)
{
//    tr_printf("Scenario 0a: Starting startworker ...\n");
    startworkerbar->enter();
    worker(counter);
    tstbar->enter();
//    tr_printf("Scenario 0a: End startworker ...\n");
}

void
driver()
{

    StubXAppl appl(myAppl->getOH());
    int vp;

    startworkerbar=new BlockBarrier(NUMPROC);
    GlobalUpdate((void **)&startworkerbar);

    testAndDecSharedCounter *globalcounter=new testAndDecSharedCounter(CAPACITY);
    GlobalUpdate((void **)&globalcounter);
    
    // start a workers up one per processor
    for (vp=0;vp<NUMPROC;vp++) {
	appl.setVP(vp);
	appl.createProcess1( (tstatusfunc) startworker, (reg_t)globalcounter);
    }
}

void 
test()
{
 
//    tr_printf("Scenario 0a: Starting Test ...\n");
    tstbar=new BlockBarrier(NUMPROC+1);
    GlobalUpdate((void **)&tstbar);
    driver();
    tstbar->enter();
//    tr_printf("Scenario 0a: Main process done.\n");
}
