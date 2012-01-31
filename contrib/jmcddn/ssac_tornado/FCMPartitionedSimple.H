#ifndef FCMPARTITIONEDSIMPLE_DEFH
#define FCMPARTITIONEDSIMPLE_DEFH
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
 * $Id: FCMPartitionedSimple.H,v 1.4 1998/12/11 07:04:01 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: Abstract FCM (File Chunk Manager) class
 * **************************************************************************/

#include "mem/PageList.H"
#include "FCMDistributedBase.H"


class FCMPartitionedSimple : public FCMDistributedBase {

    // Miss Handler Definition
    class FCMPartitionedSimpleMH : public FCMDistributedBaseMH {
    public:
	    FCMPartitionedSimpleMH( CORRef cor ) :
		FCMDistributedBaseMH(cor) {}
	    
	    virtual ClusteredObject * createFirstRep() {
		return (ClusteredObject *)
		    new FCMPartitionedSimple(_globals);
	    }
	    virtual ClusteredObject * createRep() {
		return (ClusteredObject *)
		    new FCMPartitionedSimple(_globals);
	    }
    };
    
    friend class FCMPartitionedSimple::FCMPartitionedSimpleMH;

    SimpleSpinBlockLock _lock;         // local lock
    PageList      _pagelist;           // local cache of pages
    ListSimpleKey _regionCallbackList; // local list for callbacks on I/O
                                       //  key is file offset

//    int           _pageFaultCounter;

    FCMPartitionedSimple(FCMDistributedBase::globals *g) :
	FCMDistributedBase(g) { }

    virtual Page *findPage( addr_t fileOffset );
    virtual Page *addPage( addr_t fileOffset, addr_t paddr, addr_t len );
    void writeBackPageCache();
    void deallocPageCache();
    void prepareToDie();
    virtual int getPageBalance();

    enum findStatus { HIT=0, BUSY=1, MISS=2 };
    enum missType   { HANDLED_ASYNC, NOT_HANDLED_SYNC };
 
    int findLocal ( addr_t fileOffset, size_t len,
			   AccessType accessType,
			   RegionRef reg, void *a0, void *a1,
			   addr_t &paddr, u_int &timestamp);

    int findGlobal( addr_t fileOffset, size_t len,
			  AccessType accessType,
			  RegionRef reg, void *a0, void *a1,
			  addr_t &paddr, u_int &timestamp);
    

    int handleAndClassifyMiss( addr_t fileOffset, size_t len,
				    AccessType accessType,
				    RegionRef reg, void *a0, void *a1,
				    addr_t &paddr, u_int &timestamp,
				    FCMRegionCallback * &cb);
    
    int cleanCallBacks ( addr_t fileOffset, size_t len,
			 addr_t &result, u_int &ts, 
			 FCMRegionCallback *cb );		       
    
public:

    static FCMRef create( CORRef cor ) {
	return (FCMRef)((new FCMPartitionedSimpleMH(cor))->ref());
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
#endif /* FCMPARTITIONEDSIMPLE_DEFH */
