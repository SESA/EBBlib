/******************************************************************************
 *
 *                           Tornado: Memory management
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
 * $Id: XmyRegionSimpleCustom.C,v 1.3 1998/12/11 07:04:02 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: external interface object for regionsimple with
 * custom page replacement policy.
 * **************************************************************************/
 
#include <sys/types.h>
#include <tornado/sys/WellKnown.H>
#include <tornado/TAssert.H>
#include "misc/ObjectRefs.H"
#include "ppc/ppc.h"
#include "objtrans/ot.H"
#include "objtrans/XObjCallHandler.H"
#include "mem/RegionSimpleCustom.H"
#include "XmyRegionSimpleCustom.H"
#include "MetaXmyRegionSimpleCustom.H"
#include "XmyRegionSimpleCustomInfo.h"
#include "mem/FCMSimple.H"
#include "mem/FCMLRU.H"
#include "mem/FCMFIFO.H"
#include "mem/FCMTrace.H"
#include "FCMPartitionedSimple.H"
#include "mem/XCORapp.H"
#include "ppc/XProgram.H"
#include "misc/TypeServer.H"
#include "objtrans/GOBJ.H"

#define CHK 0x37

MetaXmyRegionSimpleCustom TheMetaXmyRegionSimpleCustom;
MetaXmyRegionSimpleCustom :: MetaXmyRegionSimpleCustom() : MetaXRegion(this)
{
    ObjectHandle oh;
    (void)new MHShared( this, UNSAFE_BADGE,
			METAXMYREGIONSIMPLECUSTOM_NUM_METHODS,CHK );
    oh.oid  = INTERNAL_TO_EXTERNAL( _ref );
    oh.port = myPortId;
    DREF(GOBJ::TheTypeServerRef())->addType( XmyRegionSimpleCustom::TYPE_ID,
					     oh );
}

#ifndef XMYREGIONSIMPLECUSTOM_NUM_METHODS
#define XMYREGIONSIMPLECUSTOM_NUM_METHODS XREGION_NUM_METHODS
#endif

XmyRegionSimpleCustom :: XmyRegionSimpleCustom( Badge b,
					    RegionRef region) :
    	 XRegion(b,region, XMYREGIONSIMPLECUSTOM_NUM_METHODS, CHK)
{
}


TornStatus
XmyRegionSimpleCustom :: BindRegion( ObjectHandle xp, ObjectHandle xc,
				   FCMtype fcmtype, reg_t size, int forwrite,
				   addr_t & vaddr )
{
    TornStatus  rc;
    ProgramRef  prog;
    CORRef      cor;
    reg_t       badgeOp = GetBadgeOpFromReg();
    Badge       rbadge = PPC_BADGE(badgeOp);
    
    trace(MM, TR_INFO, tr_printf(
	"doing XmyRegionSimpleCustom bind: size %lx, prog %lx add %lx"
	" bo %lx\n", 
	size, xp.oid, vaddr, badgeOp ) );

    if( size == 0 ) {
	wassert(0, ass_printf("region size is zero\n"));
	vaddr = 0;
	return -1;
    }

    if( size & MIN_PAGE_MASK ) {
	wassert( 0, ass_printf("size (%lx) not aligned, rouding up\n", size) );
	size = ((size + MIN_PAGE_SIZE) & ~MIN_PAGE_MASK);
    }
    rc = GetInternalObj(TCTYPE(XProgram),xp.oid,prog,rbadge);

    if( rc != 0 ) {
	wassert(0, ass_printf("Couldn't get program ref\n"));
	vaddr = 0;
	return rc;
    }

    rc = GetInternalObj(TCTYPE(XCORapp),xc.oid,cor,rbadge,forwrite);
    if( rc != 0 ) {
	wassert(0, ass_printf("Couldn't get cor ref\n"));
	vaddr = 0;
	return rc;
    }

    // selecting right type of FCM
    FCMRef fcmref;
    switch( fcmtype ) {
    case FCM_SIMPLE: {
	fcmref = FCMSimple::create(0);
	break;
    }
    case FCM_LRU: {
	trace( MM, ANY, tr_printf(
	    "XmyRegionSimpleCustom: FCM_LRU selected\n" ) );
	fcmref = FCMLRU::create(0);
	break;
    }
    case FCM_FIFO: {
	trace( MM, ANY, tr_printf(
	    "XmyRegionSimpleCustom: FCM_FIFO selected\n" ) );
	fcmref = FCMFIFO::create(0,20);
	break;
    }
    case FCM_TRACE: {
	trace( MM, ANY, tr_printf(
	    "XmyRegionSimpleCustom: FCM_TRACE selected\n" ) );
	fcmref = FCMTrace::create(0);
	break;
    }
    case FCM_PARTITIONEDSIMPLE: {
	trace( MM, ANY, tr_printf(
	    "XmyRegionSimpleCustom: FCM_PARTITONEDSIMPLE selected\n" ) );
	fcmref = FCMPartitionedSimple::create(0);
	break;
    }
    default:
	trace( MM, ANY, tr_printf(
	    "XmyRegionSimpleCustom: no FCM was selected\n" ) );
	fcmref = 0;
    }

    (void)RegionSimpleCustom::create( prog, cor, fcmref, 0, vaddr, size, rc,
				      0, forwrite );
    trace(MM, TR_INFO,
	  tr_printf("address from Bind from kernel address %lx\n", vaddr ) );
    
    return rc;
}



void
XmyRegionSimpleCustom :: BindReadRegion( ObjectHandle xp, ObjectHandle xc,
				       FCMtype fcmtype, reg_t size,
				       addr_t INOUT vaddr )
{
    TornStatus rc = BindRegion( xp, xc, fcmtype, size, 0, vaddr );
    XReturn( rc, vaddr );
}

void
XmyRegionSimpleCustom :: BindWriteRegion( ObjectHandle xp, ObjectHandle xc,
					FCMtype fcmtype, reg_t size,
					addr_t INOUT vaddr )
{
    TornStatus rc = BindRegion( xp, xc, fcmtype, size, 1, vaddr );
    XReturn( rc, vaddr );
}
