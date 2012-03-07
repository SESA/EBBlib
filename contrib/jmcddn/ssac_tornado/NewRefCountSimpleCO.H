#ifndef NEWREFCOUNTSIMPLECO_DEFH
#define NEWREFCOUNTSIMPLECO_DEFH
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
 * $Id: NewRefCountSimpleCO.H,v 1.2 1998/01/03 05:13:02 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/
#include <tornado/ClusteredObject.H>
#include "lock/Lock.H"

class RefCountSimpleCO;
typedef RefCountSimpleCO **RefCountSimpleCORef;


class RefCountSimpleCO : public COSimple {
protected:
    int _localcount;
    SimpleSpinBlockLock _lock;
public:
    RefCountSimpleCO(int i=0);
    virtual ~RefCountSimpleCO();
    virtual TornStatus inc();
    virtual TornStatus dec();
    virtual TornStatus val(int&);
};

#endif /* REFCOUNTSIMPLECO_DEFH */
