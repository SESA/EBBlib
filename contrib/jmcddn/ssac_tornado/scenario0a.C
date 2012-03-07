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
 * $Id: scenario0a.C,v 1.1 1997/10/29 15:48:53 jonathan Exp $
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
#include "SimplePositiveSharedCounter.H"

BlockBarrier *tstbar;
BlockBarrier *startworkerbar;

class Data {
    SimplePositiveSharedCounter refcount;
public:
    ~Data() { tr_printf("Data:~Data() delete is happening\n");};
    void newreference(){refcount.inc();};
    void dosomething(){ tr_printf("Data:Dosomething\n");};
    void destroy() {refcount.dec(); if (!refcount.val()) delete this;};
};

void
worker(Data *data)
{
//    int i=0;
   // add a sleep for some random amount of time
//    tr_printf("Scenario 0a: Starting worker ...\n");
//    while(i<10000) {
//	i++;
//    }
    data->dosomething();
    data->destroy();
//    tr_printf("Senario 0a: Ending Worker done vp=%d\n",MYVP);
}

void
startworker(Data *data)
{
//    tr_printf("Scenario 0a: Starting startworker ...\n");
    startworkerbar->enter();
    worker(data);
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
//    tr_printf("Scenario 0a: Starting driver ...\n");
    Data *data=new Data;
    data->newreference();
    
    // start a workers up one per processor
    for (vp=0;vp<NUMPROC;vp++) {
	appl.setVP(vp);
	data->newreference();
	appl.createProcess1( (tstatusfunc) startworker, (reg_t) data);
    }
    data->destroy();
//    tr_printf("Scenario 0a: End driver ...\n");
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
