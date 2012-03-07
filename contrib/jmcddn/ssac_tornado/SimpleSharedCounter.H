#ifndef SIMPLESHAREDCOUNTER_DEFH
#define SIMPLESHAREDCOUNTER_DEFH
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
 * $Id: SimpleSharedCounter.H,v 1.1 1997/10/29 17:34:35 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* SIMPLESHAREDCOUNTER_DEFH */

#include "lock/Lock.H"

class SimpleSharedCounter {
    int count;
    SimpleSpinBlockLock lock;
public:
    SimpleSharedCounter() { count=0; }
    SimpleSharedCounter(const int &i) { count=i; }
    int val(){ return count;}
    int inc();
    int dec();
    int add(const int &);
};
