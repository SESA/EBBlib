#include <unistd.h>
#include <tornado/sys/WellKnown.H>
#include <tornado/sys/kernelcalls.h>
#include <tornado/TAssert.H>

extern void test();

void
main()
{
    if (MYVP != 0)
	while( 1 ) block();
    
    tr_printf("**** main:START\n" );
    test();
    tr_printf("**** main:END\n" );
    
}

