#ifndef __MRSWCOUNTER_H__
#define __MRSWCOUNTER_H__
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
 * $Id: MRSWCounterCO.H,v 1.1 1997/10/29 17:34:26 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/
#include "MissHandler.H"
#include "Representative.H"
#include "lock/Lock.H"

class MRSWCounterCORep;
typedef MRSWCounterCORep **MRSWCounterCORef;

class MRSWCounterCO : public Replicate {
protected:
    MRSWCounterCORep *first;
    MRSWCounterCORep *last;
    MRSWCounterCORep *owner;
    SimpleSpinBlockLock *wlock;
    friend class MRSWCounterCORep;
public:
    MRSWCounterCO(int &);
    virtual ~MRSWCounterCO();
    MRSWCounterCORef getRef() { return (MRSWCounterCORef) ref; }
    // Manditory miss function which allocates the representative you
    // would like and then calls Replicate::handleMiss
    virtual Obj* handleMiss( TransEntry *, MHOEntry *, int);
};


class MRSWCounterCORep : public Representative {
    int count;
    enum states {modified,exclusive,shared,invalid} state;
    SimpleSpinBlockLock *slock;
    MRSWCounterCORep *next;
    MRSWCounterCORep *owner;
    friend class MRSWCounterCO;
public:
    MRSWCounterCORep(ObjRef, MRSWCounterCORep *, MRSWCounterCORep *, SimpleSpinBlockLock &);
    virtual ~MRSWCounterCORep();
    virtual TornStatus destroy();
    virtual TornStatus inc();
    virtual TornStatus dec();
    virtual TornStatus setval(int &);
    virtual TornStatus val(int&);
};

#endif /* MRSWCOUNTER_DEFH */
