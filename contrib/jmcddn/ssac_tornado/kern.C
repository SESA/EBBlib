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
 * $Id: kern.C,v 1.2 1998/11/20 04:09:07 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: main test driver
 * **************************************************************************/
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sgi/mips_cpu_more.h>
#include <tornado/sys/WellKnown.H>
#include <tornado/sys/kernelcalls.h>
#include <tornado/TAssert.H>
#include "misc/GLocal.H"
#include "misc/KernelObj.H"
#include "misc/misc.h"
#include "misc/Timer.H" 
#include "lock/Barrier.H"   
#include <tornado/Lock.H>
 
extern int _on_simos_sim;
extern unsigned long DramBaseAddress; // from KernelObj.C

void startServer( addr_t load_paddr, char *name, int asynch ); // SrvAddr.C

extern "C" void check_kargs();		// simos triggers on this call
void check_kargs()
{
    tr_printf("kparms bytes:\n");
    for( int i = 0; i < 8; i++ ) {
	tr_printf("%lx ", KParms.args[i]);
    }
} 
 
void
simos_putchar_delay_on()
{
    if( _on_simos_sim ) {
	extern unsigned long _simos_delay_putchar_usec;
	unsigned long delay = 1450;
	tr_printf("simos putchar delay: %ld usec\n", delay);
	_simos_delay_putchar_usec = delay;
    }
}

void
simos_putchar_delay_off()
{
    if( _on_simos_sim ) {
	extern unsigned long _simos_delay_putchar_usec;
	_simos_delay_putchar_usec = 0;
	tr_printf("simos putchar delay: %ld usec\n",_simos_delay_putchar_usec);
    }
}

void
main()
{
    
    tr_printf("in main; Config reg %lx KParms.args[0]=%llx\n", GetCP0Reg( C0_CONFIG ),KParms.args[0]);

    //TraceMask = ~0; TraceLevel = TR_EXTREME-1;
    //TraceMask |= MM; TraceLevel = ANY; 

    abortSelect( ALWAYS_DEBUG /*ALWAYS_EXIT*/ /*ALWAYS_ASK*/ );

    check_kargs();

 cpt_retry:

    // exempt certain special tests
    switch ( KParms.args[0] ) {
    case 4:
    case 5:
	break;
    default:
	if( MYPROC != 0 ) {
	    tr_printf("Not 0, blocking forever: %ld\n", KParms.args[0]);
	    while( 1 ) {
		block();
	    }
	}
    }

    tr_printf("Running kernel test %ld\n", KParms.args[0]);

    reg_t *p = KParms.args;

    switch( p[0] ) {

#if 0
    case 0:
	LoadServersAndBlock();
	break;

    case 1:
	void testTiming();
	testTiming();
	break;

    case 2:
	void DoPpcTest( int numworkers, int numiters );
	DoPpcTest( NUMPROC, p[1] );
	break;
#endif
    case 3:
    {
	tr_printf("Doing userlevel test\n");
	//simos_putchar_delay_on();
	unsigned long start = getCurrentNsec();
	startServer( 0x0800000L | DramBaseAddress, "usrlvl", 0 );
	unsigned long end = getCurrentNsec();
//	simos_putchar_delay_off();
	tr_printf("Done userlevel test: time %ld usec\n", (end - start)/1000);
	break;
    }
#if 0
    case 4:
	extern void testCO();
	testCO();
	break;

    case 5:
	if( MYVP == 0 ) {
	    tr_printf("checkpointing\n");
	    _do_checkpoint();
	}
	InitBarrier->enter();
	check_kargs();			// check for new kargs; cpt restart
	if( p[0] != 5 ) goto cpt_retry;
	break;

    case 6:
	extern void DoRemoteIntTest( int numIters );
	DoRemoteIntTest( 256 );
	DoRemoteIntTest( 2560 );
	break;
	
    case 7:
	extern void DoRemoteFuncTest( int numIters );
	DoRemoteFuncTest( 256 );
	break;

    case 8:
	extern void cobjalloc( int numIters );
	cobjalloc( 256*100 );
	extern void cobjallocsep( int numIters );
	cobjallocsep( 10 );
	break;
	
    case 9:
	extern void missTest( int numIters, int triggerit );
	missTest( 256*100, 1 );
	// for tracing
	missTest( 5, 0 );
	break;
	
    case 10:
	extern void gcTest( int numIters, int triggerit );
	gcTest( 256, 0 );
	gcTest( 256, 0 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	break;

    case 11:
	extern void DoConcGCTest( int numWorkers, int numIters, int triggerit);
	for( i = 1; i <= NUMPROC; i++ ) {
	    DoConcGCTest( i, 10, 0 );
	    DoConcGCTest( i, 10, 0 );
	    DoConcGCTest( i, 10, 1 );
	}
	break;

    case 12:
	extern void DoConcMissTest( int numWorkers, int numIters );
	for( i = 1; i <= NUMPROC; i++ ) {
	    DoConcMissTest( i, 256 );
	    DoConcMissTest( i, 256*10 );
	}
	break;

    case 13:
	extern void DoConcKMATest( int numWorkers, int numIters, int size,
				   int triggerit );
	for( i = 1; i <= NUMPROC; i++ ) {
	    DoConcKMATest( i, 256*10, 8, 0 );
	}
	//DoConcKMATest( NUMPROC, 5, 1, 1 );
	break;

    case 14:
	extern void DoConcPpcTest(int numWorkers, int numiters, int inout,
				  int triggerit);
	for( i = 1; i <= NUMPROC; i++ ) {
	    DoConcPpcTest( i, 256, 0, 0 );
	    DoConcPpcTest( i, 256*10, 0, 1 );
	}
	//DoConcPpcTest( NUMPROC, 3, 0, 0 );
	break;

    case 15:
	extern void DoRemotePpcTest( int numIters, int triggerit );
	DoRemotePpcTest( 256, 0 );
	DoRemotePpcTest( 2560, 1 );
	//DoRemotePpcTest( 3, 1 );
	break;

    case 16:
	extern void lockTest( int numIters, int triggerit );
	lockTest( 256, 0 );
	lockTest( 5, 1 );
	break;

    case 17:
	extern void DoConcPpcTest(int numWorkers, int numiters, int inout,
				  int triggerit);
	DoConcPpcTest( NUMPROC, 256, 1, 0 );
	DoConcPpcTest( NUMPROC, 256*10, 1, 0 );
	DoConcPpcTest( NUMPROC, 3, 1, 1 );
	break;

    case 18:
	tr_printf("Doing multi userlevel test\n");
	StartMulti( 0x0800000L | DramBaseAddress );
	tr_printf("Done userlevel test\n");
	break;

    case 20:
	extern void DoConcMyCOTest();
	tr_printf("Doing My Clustered Object test\n");
	DoConcMyCOTest();
	break;
	
    case 21:
	tr_printf("Doing nfs + userlevel test:\n");
	startServer( 0x0800000L | DramBaseAddress, "nfs", 0 );
	startServer( (0x0800000L+0x200000) | DramBaseAddress, "usrlvl", 0 );
	tr_printf("Done nfs + userlevel test\n");
	break;

    case 22:
    {
//        long oTraceMask = TraceMask;
//	long oTraceLevel = TraceLevel;
//	TraceMask |= MM | LIBS;
//	TraceMask |= INIT;
//	TraceLevel = TR_DEBUG;
	startReplicatedPgmServer( 0x0800000L | DramBaseAddress, "usrlvl", 0 );
//	TraceMask = oTraceMask;
//	TraceLevel = oTraceLevel;
//	tr_printf("Done Replicated Program test: time %ld usec\n", (end - start)/1000);
	break;
    }

    case 23:
	extern void DoConcProgramReplicatedTest();
	tr_printf("Doing ProgramReplicated Object test\n");
	DoConcProgramReplicatedTest();
	break;

    case 24:
	tr_printf("Doing nfs + userlevel multiprogrammed test:\n");
	startServer( 0x0800000L | DramBaseAddress, "nfs", 0 );
	StartMulti( (0x0800000L+0x200000) | DramBaseAddress );
	tr_printf("Done nfs + userlevel multiprogammed test\n");
	break;
	
    case 25:
	tr_printf("Doing nfs + userlevel (repl prog) test:\n");
	startServer( 0x0800000L | DramBaseAddress, "nfs", 0 );
	startReplicatedPgmServer( (0x0800000L+0x200000) | DramBaseAddress,
				  "usrlvl", 0 );
	tr_printf("Done nfs + userlevel test\n");
	break;

    case 26:
	tr_printf("Doing multi userlevel test (rep)\n");
	StartMultiRep( 0x0800000L | DramBaseAddress );
	tr_printf("Done userlevel test\n");
	break;

    case 27:
	tr_printf("Doing multi nfs+userlevel test (rep)\n");
	startServer( 0x0800000L | DramBaseAddress, "nfs", 0 );
	StartMultiRep( (0x0800000L+0x200000) | DramBaseAddress );
	tr_printf("Done userlevel test\n");
	break;

    case 100:
    {
	tr_printf("Doing collection of tests\n");
	void testTiming();
	testTiming();
	extern void DoRemoteIntTest( int numIters );
	DoRemoteIntTest( 2560 );
	DoRemoteFuncTest( 2560 );
	extern void cobjalloc( int numIters );
	cobjalloc( 256*100 );
	extern void missTest( int numIters, int triggerit );
	missTest( 256*100, 1 );
	extern void gcTest( int numIters, int triggerit );
	gcTest( 256, 0 );
	gcTest( 256, 0 );
	gcTest( 25, 1 );
	gcTest( 25, 1 );
	for( i = 1; i <= NUMPROC; i++ ) {
	    extern void DoConcGCTest( int numWorkers, int numIters,
				      int triggerit);
	    DoConcGCTest( i, 10, 0 );
	    DoConcGCTest( i, 10, 0 );
	    DoConcGCTest( i, 10, 1 );
	}
	for( i = 1; i <= NUMPROC; i++ ) {
	    extern void DoConcMissTest( int numWorkers, int numIters );
	    DoConcMissTest( i, 256*10 );
	}
	for( i = 1; i <= NUMPROC; i++ ) {
	    extern void DoConcKMATest( int numWorkers, int numIters, int size,
				       int triggerit );
	    DoConcKMATest( i, 256*10, 8, 0 );
	}
	for( i = 1; i <= NUMPROC; i++ ) {
	    extern void DoConcPpcTest(int numWorkers, int numiters, int inout,
				      int triggerit);
	    DoConcPpcTest( i, 256, 0, 0 );
	    DoConcPpcTest( i, 256*10, 0, 1 );
	}
	extern void DoRemotePpcTest( int numIters, int triggerit );
	DoRemotePpcTest( 256, 0 );
	DoRemotePpcTest( 2560, 1 );
	extern void lockTest( int numIters, int triggerit );
	lockTest( 256, 0 );
	for( i = 1; i <= NUMPROC; i++ ) {
	    extern void DoConcPpcTest(int numWorkers, int numiters, int inout,
				      int triggerit);
	    DoConcPpcTest( i, 256, 1, 0 );
	    DoConcPpcTest( i, 256*10, 1, 1 );
	}
	tr_printf("Done collection of tests\n");
	break;
    }
#endif
    default:
	tr_printf("Unknown test: %d\n", KParms.args[0]);
	break;
    }

    tr_printf("\nALL DONE WITH ALL TESTS\n");
    if( ! _on_simos_sim ) {
	while( 1 ) {
	    block();
	}
    } else {
	exit(0);
    }
}
