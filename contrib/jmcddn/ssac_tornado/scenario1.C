/******************************************************************************
 *
 *                           Tornado:[tests]
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
 * $Id: scenario1.C,v 1.1 1997/10/29 17:34:21 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: main test driver
 * **************************************************************************/
#include <unistd.h>
#include <tornado/sys/WellKnown.H>
#include <tornado/TAssert.H>
#include "SimpleSharedCounter.H"

SimpleSharedCounter I;

void
test()
{
    tr_printf("Starting senario 1 Test ...\n");
    for (int i=0;i<5;i++ ) {
	I.inc();
    }
    tr_printf("I.val()=%d\n",I.val());
    tr_printf("Done senario 1 Test.\n");
}
