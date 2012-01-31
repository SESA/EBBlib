#ifndef SHAREDCOUNTERCO_DEFH
#define SHAREDCOUNTERCO_DEFH
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
 * $Id: SharedCounterCO.H,v 1.1 1998/04/24 17:59:57 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* SIMPLESHAREDCOUNTER_DEFH */

#include "lock/Lock.H"
#include "./integerCounterCO.H"

class SharedCounterCO : public integerCounter {
    int _count;
    MHShared _mh;
    SharedCounterCO() : _mh(this) { _count=0; }
    integerCounterRef ref() { return (integerCounterRef)_ref; }
public:
    static integerCounterRef create() {
	return (integerCounterRef)((new SharedCounterCO()->ref()));
    }
    virtual void value(int &val)  { val=_count; return; }
    virtual void increment()      { FetchAndAdd(&_count,1); }
    virtual void decrement()      { FetchAndAdd(&_count,-1); }
};
