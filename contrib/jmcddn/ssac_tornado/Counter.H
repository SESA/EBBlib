#ifndef COUNTER_DEFH
#define COUNTER_DEFH
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
 * $Id: Counter.H,v 1.1 1997/10/29 17:34:24 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* COUNTER_DEFH */

#include "lock/Lock.H"

class Counter {
    int count;
    SimpleSpinBlockLock lock;
public:
    Counter() { count=0; }
    Counter(const int &i) { count=i; }
    int val(){ return count;}
    int inc();
    int dec();
    int add(const int &);
};
