#ifndef TESTANDDECSHAREDCOUNTER_DEFH
#define TESTANDDECSHAREDCOUNTER_DEFH
/******************************************************************************
 *
 *                           Tornado:[COMPONENT]
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
 * $Id: testAndDecSharedCounterCO.H,v 1.1 1997/11/11 18:47:17 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* TESTANDDECSHAREDCOUNTER_DEFH */
#include <MissHandler.H>
#include "lock/Lock.H"

class testAndDecSharedCounterCO:Shared {
    int count;
    SimpleSpinBlockLock lock;
public:
    testAndDecSharedCounter(int I) { count=I; }
    virtual TornStatus testAndDecrement();
    virtual TornStatus increment();
    virtual TornStatus val() { return count; }
};

typedef testAndDecSharedCounterCO **testAndDecSharedCounterCORef;
