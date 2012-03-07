#ifndef FCMREPLICATEDSIMPLE_DEFH
#define FCMREPLICATEDSIMPLE_DEFH
/******************************************************************************
 *
 *                           Tornado: FCM (File Chunk Manager)
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
 * $Id: FCMReplicatedSimple.H,v 1.3 1998/12/09 05:26:06 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: Abstract FCM (File Chunk Manager) class
 * **************************************************************************/
#include "mem/PageList.H"
#include "FCMDistributedBase.H"

class FCMReplicatedSimple : public FCMDistributedBase {

    // Miss Handler Definition
    class FCMReplicatedSimpleMH : public FCMDistributedBaseMH {
    public:
	    FCMReplicatedSimpleMH( CORRef cor ) :
		FCMDistributedBaseMH(cor) {}
	    
	    virtual ClusteredObject * createFirstRep() {
		return (ClusteredObject *)
		    new FCMReplicatedSimple(_globals);
	    }
	    virtual ClusteredObject * createRep() {
		return (ClusteredObject *)
		    new FCMReplicatedSimple(_globals);
	    }
    };
    
    friend class FCMReplicatedSimple::FCMReplicatedSimpleMH;

    PageList      _pagelist;
//    int           _pageFaultCounter;

    Page *findPage( addr_t fileOffset );
    Page *addPage( addr_t fileOffset, addr_t paddr, addr_t len );
    void writeBackPageCache();
    void deallocPageCache();
    void prepareToDie();
    virtual int getPageBalance();
    FCMReplicatedSimple(FCMDistributedBase::globals *g) :
	FCMDistributedBase(g) {}
    
public:
    static FCMRef create( CORRef cor ) {
	return (FCMRef)((new FCMReplicatedSimpleMH(cor))->ref());
    }
    virtual TornStatus find( addr_t fileOffset, size_t len,
			     AccessType accessType,
			     RegionRef reg, void *a0, void *a1,
			     addr_t &paddr, u_int &timestamp);
    virtual TornStatus detachCOR( CORRef c );
    virtual TornStatus readDone(addr_t paddr, addr_t fileOffset, size_t len);
    virtual TornStatus writeDone(addr_t fileOffset, size_t len);

    virtual int getNumPages( );
    virtual TornStatus writeBack( );
    virtual TornStatus invalidate( );
    virtual TornStatus cleanup();
};
#endif /* FCMREPLICATEDSIMPLE_DEFH */
