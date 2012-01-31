#ifndef NEWREFCOUNTCO_DEFH
#define NEWREFCOUNTCO_DEFH
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
 * $Id: NewRefCountCO.H,v 1.5 1998/02/12 03:48:56 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/
#include <tornado/ClusteredObject.H>
#include "lock/Lock.H"
 
class RefCountCO;
typedef RefCountCO **RefCountCORef;

class RefCountCO : public ClusteredObject {
protected:
    class RefCountCOMH : public MHReplicate {
    protected:
	RefCountCO *_first;
	RefCountCO *_last;
	int _initialvalue;
    public:
	RefCountCOMH(int);
	virtual ~RefCountCOMH();
	// Manditory functions that a Replicated MissHandler must
	// provide.  The COS will call these functions when it
	// requires the creation of a new representative.
	// createFirstRep will be called on when the first 
	// representative needs to be created.  After that createRep
	// will be called.  These functions must  pass back a poiner to
	// a new Representative.
	virtual ClusteredObject * createFirstRep();
	virtual ClusteredObject * createRep();
	RefCountCORef ref() { return (RefCountCORef)_ref; }
    };
    int _localcount;
    RefCountCO *_next;
    SimpleSpinBlockLock _lock;

    RefCountCO(RefCountCO *,int);
    friend class RefCountCO ::RefCountCOMH;
    
public:
    static RefCountCORef construct(int i=0);
    virtual ~RefCountCO();
    virtual TornStatus inc();
    virtual TornStatus dec();
    virtual TornStatus val(int&);
    virtual TornStatus myrepval(int&);
};

#endif /* REFCOUNTCO_DEFH */
