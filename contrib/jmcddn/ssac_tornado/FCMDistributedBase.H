#ifndef FCMDISTRIBUTEDBASE_DEFH
#define FCMDISTRIBUTEDBASE_DEFH
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
 * $Id: FCMDistributedBase.H,v 1.2 1998/12/09 05:26:04 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: Abstract FCM (File Chunk Manager) class
 * **************************************************************************/
#include <tornado/ListSimpleKey.H>
#include <tornado/ListSimpleLocked.H>
#include "mem/FCM.H"

#define COGLOBAL(global) ( _globals -> global )

class FCMDistributedBase : public FCM {

    TornStatus toDefaultPM();
    
protected:
    struct globals {
	FCMDistributedBase **_repArray;
	PMRef                _pmdefault;         // reference to the default 
                                                 // Page Manager
	SimpleSpinBlockLock  _lock;
	ListSimpleLocked     _regionList;        // list of regions that are 
                                                 // mapping this FCM 
	int                  _timestamp;	 // used to sequence map/unmap
	                                         // requests
	CORRef               _cor;		 // cor to get file contents
                                                 // from
	PMRef                _pm;                // my page manager
	StringShort          _name;		 // string name of file
	enum boolean { NO, YES } 
                             _replaceable;       // are FCM pages replaceable?
	FCMtype              _fcmtype;
    } *_globals;

    // Miss Handler Definition
    class FCMDistributedBaseMH : public MHReplicate {
    protected:
	FCMDistributedBase::globals *_globals;
	virtual ~FCMDistributedBaseMH();
    public:
	FCMDistributedBaseMH( CORRef cor ); 
    };

    friend class FCMDistributedBase::FCMDistributedBaseMH;

    FCMDistributedBase(globals *g) { _globals=g; 
                                     COGLOBAL(_repArray[MYVP])=this; }
    virtual int  getPageBalance() = 0;
    virtual void lastRegionDetached();
    virtual void printAllPages();

public:
    virtual TornStatus find( addr_t fileOffset, size_t len,
			     AccessType accessType,
			     RegionRef reg, void *a0, void *a1,
			     addr_t &paddr, u_int &timestamp) = 0;
    virtual TornStatus attachCOR( CORRef c );
    virtual TornStatus detachCOR( CORRef c ) = 0;
    virtual TornStatus attachRegion( RegionRef regRef, PMRef pmref = 0 );
    virtual TornStatus detachRegion( RegionRef regRef,
				     BackStoreType type = VOLATILE );
    virtual TornStatus readDone(addr_t paddr, addr_t fileOffset,
				size_t len) = 0;
    virtual TornStatus writeDone(addr_t fileOffset, size_t len) = 0;
    virtual TornStatus writeBack( ) = 0;
    virtual TornStatus invalidate( ) = 0;
    virtual void       print( );
    // this is only a temp hack since diff FCMs will have diff pagelists
    virtual int        getNumPages() = 0;
    virtual char      *getName()     { return COGLOBAL(_name).getString(); }
    virtual FCMtype    getFCMtype() { return COGLOBAL(_fcmtype); }
    virtual int        isFCMReplaceable() { return (
	                                      (int) COGLOBAL(_replaceable) ); }
    virtual TornStatus cleanup() = 0;
    virtual TornStatus freeUpPages( int &, int & );
    virtual TornStatus markInterval( short interval );
    FCMRef ref()	{ return (FCMRef)_ref; }
};
#endif /* FCMDISTRIBUTEDBASE_DEFH */
