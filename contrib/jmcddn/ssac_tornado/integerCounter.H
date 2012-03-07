#ifndef INTEGERCOUNTER_DEFH
#define INTEGERCOUNTER_DEFH
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
 * $Id: integerCounter.H,v 1.1 1998/04/24 17:59:58 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* SIMPLESHAREDCOUNTER_DEFH */


class integerCounter {
public:
    virtual void value(int &val)=0;
    virtual void increment()    =0;
    virtual void decrement()    =0;
    virtual      ~integerCounter(){}
};

