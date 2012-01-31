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
 * $Id: NewRefCountSharedCO.H,v 1.4 1998/01/03 05:13:00 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/
#include <tornado/ClusteredObject.H>
#include "lock/Lock.H"

class RefCountSharedCO;
typedef RefCountSharedCO **RefCountSharedCORef;


class RefCountSharedCO : public ClusteredObject {
protected:
    int _localcount;
    SimpleSpinBlockLock _lock;
    MHShared _mh;
    RefCountSharedCORef ref() { return (RefCountSharedCORef)_ref; }
    RefCountSharedCO(int);
public:
    static RefCountSharedCORef
    create(int i=0)	{
	return ((new RefCountSharedCO(i))->ref());
    }
    virtual ~RefCountSharedCO();
    virtual TornStatus inc();
    virtual TornStatus dec();
    virtual TornStatus val(int&);
};

#endif /* REFCOUNTCO_DEFH */
