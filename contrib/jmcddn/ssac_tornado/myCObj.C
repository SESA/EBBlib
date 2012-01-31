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
 * $Id: myCObj.C,v 1.1 1997/10/29 17:34:19 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/
#include <tornado/sys/GOBJ.H>
#include "myCObj.H"

myCOBj::myCObj(myCObjRef &ref) {
    // this line is to register the clustered in the Global translation
    // table
    this.ref = (myCObjRef) DREF(GOBJ::TheObjTransRef())->allocTransStruc( this);
    // What we get back can be considered as the super this reference
    // It is a pointer to the Global tranlation table entry which
    // holds a pointer to the actuall object.  It is zero if
    // there was an error. We must pass back this reference so
    // that client code can access the object view the global translation
    // table.
    ref=this.ref;
    // We can do any normal initialization at any point.
    // We can of course validate arguments prior to regestration
    count=0;
}

int myCObj::inc() {
    return ++count;
}

int myCObj::dec() {
    count && --count;
    return count;
}

int myCObj::val() {
    return count;
}
