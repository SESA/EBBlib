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
 * $Id: CounterLocalizedCO.H,v 1.1 1998/04/06 20:14:00 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* SIMPLESHAREDCOUNTER_DEFH */

#include <machine/numachine.h>
#include "lock/Lock.H"
#include "./integerCounterCO.H"


class CounterLocalizedCO : public integerCounter {
    class CounterLocalizedCOMH : public MHReplicate {
    public:
	virtual ClusteredObject * createFirstRep() {
	    return (ClusteredObject *)new CounterLocalizedCO;
	}
	virtual ClusteredObject * createRep() {
	    return (ClusteredObject *)new CounterLocalizedCO;
	}
    };
    friend class CounterLocalizedCO::CounterLocalizedCOMH;
    struct counter {
	int val;
	char pad[SCACHELINESIZE - sizeof(int)];
    } _count;
    CounterLocalizedCO() { _count.val=0; }
public:
    static integerCounterRef create() {
	return (integerCounterRef)((new CounterLocalizedCOMH())->ref());
    }
    virtual void value(int &val) {
        MHReplicate *mymh=(MHReplicate *)MYMHO;
        CounterLocalizedCO *rep=0;
        val=0;
	mymh->lockReps();
        for (void *curr=mymh->nextRep(0,(ClusteredObject *&)rep);
             curr; curr=mymh->nextRep(curr,(ClusteredObject *&)rep))
           val+=rep->_count.val;
	mymh->unlockReps();
    }
    virtual void increment() { FetchAndAdd(&(_count.val),1); }
    virtual void decrement() { FetchAndAdd(&(_count.val),-1); }
};                 
