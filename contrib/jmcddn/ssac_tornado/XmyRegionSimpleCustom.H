#ifndef XMYREGSIMPLECUSTOMDEF_DEF
#define XMYREGSIMPLECUSTOMDEF_DEF
/******************************************************************************
 *
 *                     Tornado: Memory managment
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
 * $Id: XmyRegionSimpleCustom.H,v 1.3 1998/12/11 07:04:02 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: external interface object for regionsimple with
 * custom page replacement policy.
 * **************************************************************************/

#include <sys/types.h>
#include "mem/XRegion.H"
#include "mem/MetaXRegion.H"
#include "misc/ObjectRefs.H"

#if XOBJ_DECL
META:#include "mem/MetaXRegion.H"
STUB:#include <StubXRegion.H>
STUB:typedef enum enum_FCMtype {FCM_DEFAULT, FCM_SIMPLE, FCM_LRU,
STUB:				FCM_FIFO, FCM_TRACE, 
STUB:				FCM_PARTITIONEDSIMPLE } FCMtype;
#endif

typedef enum enum_FCMtype {
    FCM_DEFAULT, FCM_SIMPLE, FCM_LRU, FCM_FIFO, FCM_TRACE,
    FCM_PARTITIONEDSIMPLE 
} FCMtype;

class XmyRegionSimpleCustom : public XRegion {
    
    XmyRegionSimpleCustom( Badge badge, RegionRef ref );
    
    static TornStatus BindRegion( ObjectHandle prog, ObjectHandle cor,
				  FCMtype fcmtype, reg_t size,
				  int forwrite, addr_t & vaddr );

public:
    TYPE_DECL(XRegion, XmyRegionSimpleCustom);

    static NOSTUB XRegionRef create( Badge badge, RegionRef ref ) {
	return (new XmyRegionSimpleCustom(badge,ref))->ref();
    }
    
    /*
     * if address is 0, address bound to is returned, else 
     * is the virtual address that the region should be bound to
     */
    static void BindReadRegion( ObjectHandle prog, ObjectHandle cor,
				FCMtype fcmtype, reg_t size,
				addr_t INOUT vaddr );
    
    static void BindWriteRegion( ObjectHandle prog, ObjectHandle cor,
				 FCMtype fcmtype, reg_t size,
				 addr_t INOUT vaddr );
    
};

#endif /*XREGSIMPLEDEF_DEF*/
