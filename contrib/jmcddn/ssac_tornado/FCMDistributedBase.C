/******************************************************************************
 *
 *                           Tornado: FCMSimple
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
 * $Id: FCMDistributedBase.C,v 1.3 1998/12/11 07:04:00 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: Common functionality of an FCM
 * **************************************************************************/

#include <sys/types.h>
#include <tornado/sys/WellKnown.H>
#include <tornado/TAssert.H>
#include <tornado/ListSimple.H>
#include "misc/kernelcalls.h"
#include "objtrans/ot.H"
#include "objtrans/GOBJ.H"
#include "misc/ObjectRefs.H"
#include "ppc/Program.H"
#include "mem/COR.H"
#include "mem/PM.H"
#include "FCMDistributedBase.H"


FCMDistributedBase :: FCMDistributedBaseMH :: FCMDistributedBaseMH( 
    CORRef cor )
{

    _globals=new FCMDistributedBase::globals;

    COGLOBAL(_repArray) = new FCMDistributedBase *[NUMPROC];
    for (int i=0;i<NUMPROC;i++) COGLOBAL(_repArray[i])=0; 
    /* note that 'c' can be NULL if fcm is created before cor, in that
       case, a subsecuent 'attachCOR' call will bring in the correct cor
       reference */
    if( !cor ) 
	COGLOBAL(_cor) = (CORRef)OBJTRANS_DELETED;
    else {
	COGLOBAL(_cor) = cor;  
	COGLOBAL(_name).setString( DREF(COGLOBAL(_cor))->getName() );
    }

    COGLOBAL(_replaceable) = FCMDistributedBase::globals::NO;
                                  /* by default an FCM is not replaceable,
				     but this flag may be ovewritten by a
				     derived class */
    COGLOBAL(_pm) = 0;
    COGLOBAL(_fcmtype) = FCM_DEFAULT;  // unless overwritten by derived class
    // get the default PM reference
    DREF(GOBJ::TheKernelRef())->getPM( COGLOBAL(_pmdefault) );
    tassert( COGLOBAL(_pmdefault),
	     ass_printf("could not get default PM reference\n") );
}

FCMDistributedBase :: FCMDistributedBaseMH :: ~FCMDistributedBaseMH() 
{
    delete[] COGLOBAL(_repArray);
    delete _globals;
}

TornStatus
FCMDistributedBase :: attachCOR( CORRef c )
{
    tassert( (COGLOBAL(_cor) == 0) || (COGLOBAL(_cor) == (CORRef)OBJTRANS_DELETED), ass_printf(
	"trying to attach another cor, we have one already!\n") );
    COGLOBAL(_cor) = c;
    COGLOBAL(_name).setString( DREF(COGLOBAL(_cor))->getName() );
    return 0;
}

TornStatus
FCMDistributedBase :: freeUpPages( int &referenced, int &unreferenced )
{
    tassert( 0, ass_printf("should never be called\n") );
    return -1;
}

TornStatus
FCMDistributedBase :: markInterval( short interval )
{
    tassert( 0, ass_printf("should never be called\n") );
    return -1;
}

void
FCMDistributedBase :: print( )
{
    AutoLock<SimpleSpinBlockLock>
	al(&COGLOBAL(_lock)); // locks now, unlocks on return
    printAllPages();
}

void
FCMDistributedBase :: printAllPages( )
{
    // implemented by derived classes, if not, just return
    return;
}

TornStatus
FCMDistributedBase :: attachRegion( RegionRef regRef, PMRef pmref )
{
    TornStatus rc = 0;
    int firstRegion;

    // FCM lock
    AutoLock<SimpleSpinBlockLock>
	al(&COGLOBAL(_lock)); // locks now, unlocks on return

    tassert( COGLOBAL(_cor), ass_printf( "no COR attached yet?\n" ) );
    
    // if the region list is empty, this is the first region
    firstRegion = COGLOBAL(_regionList).isEmpty();  
    COGLOBAL(_regionList).add( (addr_t) regRef );
    trace( MM, TR_DEBUG, tr_printf( "Region %lx attached to FCM %lx\n",
			       regRef, ref() ) );

    if( firstRegion ) {
	tassert( pmref, ass_printf("no PM to get attached to\n"));
	if( COGLOBAL(_pm) && COGLOBAL(_pm) != pmref ) {
	    /* we were already attached to another PM, but no region is
               mapping us, so we can freely detach from 'pm' and move to
               'pmref' */
	    trace( MM, TR_DEBUG, tr_printf(
		"FCM %lx: detaching from PM %lx and attaching to PM %lx\n",
		ref(), COGLOBAL(_pm), pmref ) );
	    // detach from pm
	    rc = DREF(COGLOBAL(_pm))->detachFCM( ref(), getPageBalance() ); 
	    /* FIXME: besides detaching, we need to notify pm how many
               pages we are taking with us */
	}
	/* attach to PM, "replaceable" is a flag set when fcm was
           created that specifies whether or not page replacement is
           desired for this fcm */
	rc = DREF(pmref)->attachFCM( ref(), getPageBalance(),
				     (int) COGLOBAL(_replaceable) );
	COGLOBAL(_pm) = pmref;
    }
    else if( pmref != COGLOBAL(_pm) &&
	     COGLOBAL(_pm) != COGLOBAL(_pmdefault) ) {
	/* if the new region brings a pmref different than pm, and pm is
           not the default one, then we should detach from pm and attach
           to the default PM */
//	trace( MM, ANY, tr_printf( "switching to default PM\n" ) );

	/* hack: since this FCM is going to be shared now, we make it
           non-replaceable */
	COGLOBAL(_replaceable) = globals::NO;
	
	toDefaultPM();
    }
    return rc;
}

TornStatus
FCMDistributedBase :: detachRegion( RegionRef regRef,
				    BackStoreType backstoretype )
{
    TornStatus rc = 0;

    // FCM lock
    AutoLock<SimpleSpinBlockLock>
	al(&COGLOBAL(_lock)); // locks now, unlocks on return

    // if no cor, destroy myself
    if( COGLOBAL(_cor) == (CORRef)OBJTRANS_DELETED ) {
	/* following assert is just for reference, check that this is
	   happening as a result of a legitimate reason, such as fcm was
	   created before the _cor was actually created and then the cor
	   refussed to use it, so we have to destroy it */
//	wassert( 0, ass_printf("FCM with no COR being destroyed!\n") );
	// just make sure region list is empty, otherwise something is wrong!
	tassert( COGLOBAL(_regionList).isEmpty(), ass_printf(
	    "Region list is not empty?\n" ) );
	trace( MM, ANY, tr_printf("Destroying fcm %lx\n", ref()));
//	FIXME: is this a violation of locking protocol??
	rc = DREF(GOBJ::TheObjTransRef())->dealloc( _ref );
	return rc;
    }

    rc = COGLOBAL(_regionList).remove( (addr_t) regRef );
    tassert( rc != 0, ass_printf( "remove region from list failed\n" ));
    rc = 0;   // because regionlist remove returns positive if successful!

//    writeBackPageCache();
    
    if( COGLOBAL(_regionList).isEmpty() ) { // was this the last region?
	trace( MM, TR_DEBUG, tr_printf( "last region detached\n" ) );

	lastRegionDetached( );
#if 0
	trace( ANY, TR_INFO,
	       tr_printf( "FCM %lx (%s): pagefault count = %d\n",
			  ref(), getName(), COGLOBAL(_pageFaultCounter)) );
#endif
	/* if we were attached to a pm other than pmdefault and we are a
	   persitent fcm, then we should detach from pm and attach to
	   default PM. The reason is that it is likely that pm will be
	   removed soon, for instance if the program is terminating. */
	if( COGLOBAL(_pm) != COGLOBAL(_pmdefault) &&
	    backstoretype == PERSISTENT ) {
//	    trace( MM, ANY, tr_printf( "switching to default PM\n" ) );
	    rc = toDefaultPM();
	}
    }
    return rc;
}

void
FCMDistributedBase :: lastRegionDetached( )
{
/*  to be used by some derived classes to perform special actions when
    last region is detached */
    return;
}


TornStatus
FCMDistributedBase :: toDefaultPM()
{
    TornStatus rc;

    tassert( COGLOBAL(_lock).isLocked(), ass_printf(
	"FCM lock should be held before entering this function\n" ) );

    trace( MM, ANY, tr_printf( "fcm %lx: switching to default PM\n", ref()));

    trace( MM, ANY, tr_printf(
	"fcm %s before being attached to default PM:\n", getName() );
	   printAllPages() );
//    printf( "fcm %s before being attached to default PM:\n", getName() );
//    printAllPages();
    
    // detach from pm
    rc = DREF(COGLOBAL(_pm))->detachFCM( ref(), getPageBalance() );
    tassert( rc == 0, ass_printf("detach from PM failed\n") );
    // set pm to the default PM
    COGLOBAL(_pm) = COGLOBAL(_pmdefault);
    // attach to it
    rc = DREF(COGLOBAL(_pm))->attachFCM( ref(), getPageBalance(), 
					 (int) COGLOBAL(_replaceable) );
    tassert( rc == 0, ass_printf("attach to default PM failed\n") );
    return rc;
}
