#ifndef COUNTERLOCALIZED_DEFH
#define COUNTERLOCALIZED_DEFH
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
 * $Id: CounterLocalized.H,v 1.1 1998/04/24 17:59:46 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description:
 * **************************************************************************/

#endif /* SIMPLESHAREDCOUNTER_DEFH */
#include <machine/numachine.h>
#include <StubXAppl.H>
#include "lock/Lock.H"
#include "./integerCounter.H"


void doremoteinit(reg_t obj);

class CounterLocalized : public integerCounter {
    struct counter {
	int val;
	char pad[SCACHELINESIZE - sizeof(int)];
    } **_count;
public:
    CounterLocalized()           {  // Assume serilization between
	                            // instantiation and use
	                            StubXAppl appl(myAppl->getOH());
	                            _count=new struct counter *[NUMPROC];
                                    for (int i=0;i<NUMPROC;i++) {
					if ( i != MYVP )
					{
					    appl.setVP(i);
					    appl.createProcess1AndWait(
						(tstatusfunc)doremoteinit,
						(reg_t)this);
					} else
					    init();
				    }
                                 }
     
    virtual void init()          {   
	                            _count[MYVP]=new struct counter;
	                            _count[MYVP]->val=0;
                                 }

    ~CounterLocalized()          {
	                            for (int i=0;i<NUMPROC;i++) 
					delete _count[i];
                                 } 
    virtual void value(int &val) { 
                                   val=0; 
                                   for (int i=0;i<NUMPROC;i++)
                                          val+=_count[i]->val; 
                                 }
    virtual void increment()     { FetchAndAdd(&(_count[MYVP]->val),1); }
    virtual void decrement()     { FetchAndAdd(&(_count[MYVP]->val),-1); }
};                                                                    

void doremoteinit( reg_t obj)
{
    ((CounterLocalized *)obj)->init();
}
