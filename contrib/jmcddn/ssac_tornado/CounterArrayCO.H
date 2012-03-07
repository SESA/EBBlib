#ifndef COUNTERARRAYCO_DEFH
#define COUNTERARRAYCO_DEFH
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
 * $Id: CounterArrayCO.H,v 1.1 1998/04/24 17:59:43 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* COUNTERARRAYCO_DEFH */

#include <machine/numachine.h>
#include "lock/Lock.H"
#include "./integerCounterCO.H"


class CounterArrayCO : public integerCounter {
    class CounterArrayCOMH : public MHReplicate {
	CounterArrayCO **reparray;
    public:
	virtual ClusteredObject * createFirstRep() {
	    reparray=new CounterArrayCO *[NUMPROC];
	    for (int i=0;i<NUMPROC;i++) reparray[i]=0;
	    return new CounterArrayCO(reparray);
	}
	virtual ClusteredObject * createRep() {
	    return new CounterArrayCO(reparray);
	}
	virtual ~CounterArrayCOMH() {
	    delete[] reparray;
	}
	       
    };
    friend class CounterArrayCO::CounterArrayCOMH;
    
    int _count;
    char pad[SCACHELINESIZE - sizeof(int)];
    CounterArrayCO **_reps;
    
    CounterArrayCO(CounterArrayCO **reparray)
    {
	_count=0;
	_reps=reparray;
	reparray[MYVP]=this;
    }
    
public:
    static integerCounterRef create() {
	return (integerCounterRef)((new CounterArrayCOMH())->ref());
    }
    virtual void value(int &val) {
        val=0;
        for (int i=0;i<NUMPROC;i++)
	    if (_reps[i]) val+=_reps[i]->_count;
    }
    virtual void increment() { FetchAndAdd(&_count,1); }
    virtual void decrement() { FetchAndAdd(&_count,-1); }
};                 
