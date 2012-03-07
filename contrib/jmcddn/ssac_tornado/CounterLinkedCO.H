#ifndef COUNTERLINKEDCO_DEFH
#define COUNTERLINKEDCO_DEFH
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
 * $Id: CounterLinkedCO.H,v 1.2 1998/04/24 17:53:44 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* COUNTERLINKEDCO_DEFH */

#include <machine/numachine.h>
#include "lock/Lock.H"
#include "./integerCounterCO.H"


class CounterLinkedCO : public integerCounter {
    class CounterLinkedCOMH : public MHReplicate {
	CounterLinkedCO *_first,*_last;
    public:
	virtual ClusteredObject * createFirstRep() {
	    _first=_last=new CounterLinkedCO;
	    _last->_next=_first;
	    return _last;
	}
	virtual ClusteredObject * createRep() {
	    CounterLinkedCO *tmp=new CounterLinkedCO();
	    tmp->_next=_first; _last->_next=tmp; _last=tmp;
	    return _last;
	}
    };
    friend class CounterLinkedCO::CounterLinkedCOMH;
    
    int _count;
    CounterLinkedCO *_next;
    char pad[SCACHELINESIZE - sizeof(int) - sizeof(CounterLinkedCO *)];
    
    CounterLinkedCO() { _count=0; _next=0; }
public:
    static integerCounterRef create() {
	return (integerCounterRef)((new CounterLinkedCOMH())->ref());
    }
    virtual void value(int &val) {
        val=_count;
        for (CounterLinkedCO *p=_next;
	     p!=this; p=p->_next)
	    val+=p->_count;
    }
    virtual void increment() { FetchAndAdd(&_count,1); }
    virtual void decrement() { FetchAndAdd(&_count,-1); }
};                 
