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
 * $Id: tst.C,v 1.5 1998/04/24 17:53:52 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: main test driver
 * **************************************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <tornado/TAssert.H>
#include <tornado/Lock.H>

unsigned long
__runetype(int c){return (unsigned long)0;}

void
main()
{
    int j=0,i=0;
//    double d=0.0;
//
//    d=atof("0.5");
    j=atoi("10");
//    isspace(' ');

//    tr_printf("j=%d d=%g\n",j,d);
    tr_printf(
	"sizeof(int)=%d, sizeof(long)=%d, sizeof(char)=%d, sizeof(short)=%d,sizeof(double)=%d,sizeof(float)=%d,sizeof(SimpleSpinBlockLock)=%d\n",
	      sizeof(int),sizeof(long),sizeof(char),sizeof(short),sizeof(double),sizeof(float),sizeof(SimpleSpinBlockLock));
    j++; i=0;
    while(1) {
	tr_printf("i=%d\n",i);
	if (j==11) {
	    if ( j+i == 16) {
		tr_printf("HERE I AM\n");
		break;
	    }
	}
	i++;
    }
}
