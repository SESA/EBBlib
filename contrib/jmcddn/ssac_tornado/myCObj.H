#ifndef myCObj_DFH
#define myCObj_DFH
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
 * $Id: myCObj.H,v 1.1 1997/10/29 17:34:37 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/
#include <IObj.H>

class myCObj;
typedef myCObj **myCObjRef;

class myCObj : public IObj {
private:
    myCObjRef ref;
    int count;
public:
    myCObj();
    int inc();
    int dec();
    int val();
}
#endif /* myCObj_DFH */
