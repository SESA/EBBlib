#ifndef COUNTERARRAYPADDED_DEFH
#define COUNTERARRAYPADDED_DEFH
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
 * $Id: CounterArrayPadded.H,v 1.1 1998/04/24 17:59:45 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* SIMPLESHAREDCOUNTER_DEFH */
#include <machine/numachine.h>
#include "lock/Lock.H"
#include "./integerCounter.H"

class CounterArrayPadded : public integerCounter {
    struct counter {
	int val;
	char pad[SCACHELINESIZE - sizeof(int)];
    } *_count;
public:
    CounterArrayPadded()               {
	                            _count=new struct counter[NUMPROC];
                                    for (int i=0;i<NUMPROC;i++) 
					_count[i].val=0; 
                                 }
    ~CounterArrayPadded()              { delete[] _count; } 
    virtual void value(int &val) { 
                                   val=0; 
                                   for (int i=0;i<NUMPROC;i++) 
                                     val+=_count[i].val; 
                                 }
    virtual void increment()     { FetchAndAdd(&(_count[MYVP].val),1); }
    virtual void decrement()     { FetchAndAdd(&(_count[MYVP].val),-1); }
};                                                                    
