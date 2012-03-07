#ifndef SIMPLEPOSITIVESHAREDCOUNTER_DEFH
#define SIMPLEPOSITIVESHAREDCOUNTER_DEFH
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
 * $Id: SimplePositiveSharedCounter.H,v 1.1 1997/10/29 17:34:33 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* SIMPLEPOSITIVESHAREDCOUNTER_DEFH */

#include "lock/Lock.H"

class SimplePositiveSharedCounter {
    int count;
    SimpleSpinBlockLock lock;
public:
    SimplePositiveSharedCounter() { count=0; }
    int val(){ return count;}
    int inc();
    int dec();
};
