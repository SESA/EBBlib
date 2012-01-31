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
 * $Id: CounterArrayCOCO.H,v 1.1 1998/04/24 17:59:44 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* COUNTERARRAYCO_DEFH */

#include <machine/numachine.h>
#include "lock/Lock.H"
#include "./integerCounterCO.H"

class ArrayofRepPointers;
typedef ArrayofRepPointers **ArrayofRepPointersRef;


class ArrayofRepPointers : public ClusteredObject {
    int _size;
    ClusteredObject **_reps;
    MHShared _mh;
    
    ArrayofRepPointers(int &size) : _mh(this) {
	_size=size;
	_reps=new ClusteredObject *[_size];
	for (int i=0;i<_size;i++) _reps[i]=0;
    }
    ArrayofRepPointersRef ref() { return (ArrayofRepPointersRef) _ref; }
public:
    static ArrayofRepPointersRef create(int &size) {
	return (new ArrayofRepPointers(size))->ref();
    }
    virtual void getValueAt(int &index, ClusteredObject* &value) {
	if (index>_size) value=0;
	else value=_reps[index];
    }
    virtual void setValueAt(int &index, ClusteredObject* &value) {
	if (index<_size) _reps[index]=value;
    }
};

class CounterArrayCOCO : public integerCounter {
    class CounterArrayCOCOMH : public MHReplicate {
	ArrayofRepPointersRef _reparray;
    public:
	virtual ClusteredObject * createFirstRep() {
	    int procs=NUMPROC;
	    _reparray=ArrayofRepPointers::create(procs);
	    return new CounterArrayCOCO(_reparray);
	}
	virtual ClusteredObject * createRep() {
	    return new CounterArrayCOCO(_reparray);
	}
	virtual ~CounterArrayCOCOMH() {
	    DREF(_reparray)->destroy();
	}
	       
    };
    friend class CounterArrayCOCO::CounterArrayCOCOMH;
    
    int _count;
    char pad[SCACHELINESIZE - sizeof(int)];
    ArrayofRepPointersRef _reps;
    
    CounterArrayCOCO(ArrayofRepPointersRef reparray)
    {
	_count=0;
	_reps=reparray;
	ClusteredObject *me=this;
	DREF(_reps)->setValueAt(MYVP,me);
    }
    
public:
    static integerCounterRef create() {
	return (integerCounterRef)((new CounterArrayCOCOMH())->ref());
    }
    virtual void value(int &val) {
	ClusteredObject *rep;
        val=0;
        for (int i=0;i<NUMPROC;i++)
	{
	    DREF(_reps)->getValueAt(i,rep);
	    if (rep) val+=((CounterArrayCOCO *)rep)->_count;
        }
    } 
    virtual void increment() { FetchAndAdd(&_count,1); }
    virtual void decrement() { FetchAndAdd(&_count,-1); }
};                 
