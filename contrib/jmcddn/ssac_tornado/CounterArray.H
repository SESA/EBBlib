#ifndef COUNTERARRAY_DEFH
#define COUNTERARRAY_DEFH
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
 * $Id: CounterArray.H,v 1.1 1998/04/24 17:59:41 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* SIMPLESHAREDCOUNTER_DEFH */

#include "lock/Lock.H"
#include "./integerCounter.H"

class CounterArray : public integerCounter {
    int *_count;
public:
    CounterArray()               {
	                            _count=new int[NUMPROC];
                                    for (int i=0;i<NUMPROC;i++) 
					_count[i]=0; 
                                 }
    ~CounterArray()              { delete[] _count; } 
    virtual void value(int &val) { 
                                   val=0; 
                                   for (int i=0;i<NUMPROC;i++) 
                                     val+=_count[i]; 
                                 }
    virtual void increment()     { FetchAndAdd(&(_count[MYVP]),1); }
    virtual void decrement()     { FetchAndAdd(&(_count[MYVP]),-1); }
};                                                                    
