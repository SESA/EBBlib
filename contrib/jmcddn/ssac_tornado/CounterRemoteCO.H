#ifndef COUNTERLOCALIZEDCO_DEFH
#define COUNTERLOCALIZEDCO_DEFH
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
 * $Id: CounterRemoteCO.H,v 1.1 1998/04/24 17:59:48 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* SIMPLESHAREDCOUNTER_DEFH */

#include <machine/numachine.h>
#include "lock/Lock.H"
#include <tornado/sys/GOBJ.H>
#include <tornado/ot.H>
#include "./integerCounterCO.H"


class CounterRemoteCO : public integerCounter {
    class CounterRemoteCOMH : public MHReplicate {
    public:
	virtual ClusteredObject * createFirstRep() {
	    return (ClusteredObject *)new CounterRemoteCO;
	}
	virtual ClusteredObject * createRep() {
	    return (ClusteredObject *)new CounterRemoteCO;
	}
    };
    friend class CounterRemoteCO::CounterRemoteCOMH;
    struct counter {
	int val;
	char pad[SCACHELINESIZE - sizeof(int)];
    } _count;
    CounterRemoteCO() { _count.val=0; }
    
    virtual TornStatus sum(int *val) { *val+=_count.val; return 0; }
    
public:

    static integerCounterRef create() {
	return (integerCounterRef)((new CounterRemoteCOMH())->ref());
    }
    virtual void value(int &val) {
	int *res=new int;
        *res=_count.val;
//	val=_count.val;
//	tr_printf("value: on %d myval=%d res=%d calling: ",MYVP,_count.val,*res);
        for (int i=0;i<NUMPROC;i++) 
	    if (i!=MYVP) {
//		tr_printf("%d:%d ",i,*res);
		RFUNC1(i,(CounterRemoteCO **)_ref,
		       //	       CounterRemoteCO::sum,&val);
       		       CounterRemoteCO::sum,res);
		
	    }
	val=*res;
//	tr_printf("\n  Total=%d res=%d\n",val,*res);
	delete res;
    }
    
    virtual void increment() { FetchAndAdd(&(_count.val),1); }
    virtual void decrement() { FetchAndAdd(&(_count.val),-1); }
};                 
