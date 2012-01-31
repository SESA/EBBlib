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
 * $Id: FCMPartitionedSimple.C,v 1.5 1998/12/11 07:04:00 jonathan Exp $
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
#include "ppc/PD.H"
#include "mem/PM.H"
#include "mem/Region.H"
#include "mem/FCMList.H"
#include "mem/CORPhys.H"
#include "FCMPartitionedSimple.H"
 
TornStatus
FCMPartitionedSimple :: find( addr_t fileOffset, size_t len,
			      AccessType accessType,
			      RegionRef reg, void *a0, void *a1,
			      addr_t &result, u_int &ts )
{
    findStatus rtn;
    FCMRegionCallback *cb;
    trace( MM, TR_DEBUG, tr_printf("FCMPartitionedSimple::find\n"));
 retry:
    _lock.acquireLock();
    rtn=(findStatus) findLocal(fileOffset, len, accessType, 
			       reg, a0, a1, result, ts);
    _lock.releaseLock();

    if ( rtn == HIT  ) 	return 0;
    if ( rtn == BUSY )  return 1;

    // Miss locally must attempt a global find
    // FindGlobal takes care of all locking
    rtn = (findStatus) findGlobal(fileOffset, len, accessType, 
				  reg, a0, a1, result, ts);

    if ( rtn == HIT  )  return 0;
    if ( rtn == BUSY )  return 1;

    // Miss Globally must now add to our local cache
    tassert( _lock.isLocked(), ass_printf(
	"FCM Rep lock should be held on a Global Miss\n"));

    if ( handleAndClassifyMiss( fileOffset, len, accessType, reg,
				a0, a1, result, ts, cb )
	 == HANDLED_ASYNC ) {
	trace( MM, TR_DEBUG,
	       tr_printf("FCMPartitionedSimple::find(): ASYNC return\n") );
	return 1;
    } else { // NOT_HANDLED_SYNC
	// FIXME:  make explicit that cleanCallBacks must release rep
	//         lock.  Of course we can't assert that fact as someone
	//         else may have already grabbed the lock.
	if ( !cleanCallBacks( fileOffset, len, result, ts, cb ) ) goto retry;
    }
    return 0;
}


int
FCMPartitionedSimple :: findGlobal( addr_t fileOffset, size_t len,
				    AccessType accessType,
				    RegionRef reg, void *a0, void *a1,
				    addr_t &result, u_int &ts )
{
    trace( MM, TR_DEBUG, tr_printf("FCMPartitionedSimple::findGlobal()\n") );
    FCMPartitionedSimple *rep;
    MHReplicate *myMH=(MHReplicate *)MYMHO;
    findStatus rtn;
    
    // Lock the current representative set via the Misshandler
    myMH->lockReps();

    // Check each representative in a fixed order for the requested
    // Page. Acquire locks but do not release them until done.
    for(int i=0; i<NUMPROC; i++) {
	rep = (FCMPartitionedSimple *)COGLOBAL(_repArray[i]);
        if ( rep ) {
	    rep->_lock.acquireLock();
	    rtn = (findStatus) rep->findLocal(fileOffset, len,
					      accessType, reg, a0, a1,
					      result, ts);
	    if ( rtn == HIT || rtn == BUSY)
		break;
	}
    }
    
    // If the page was not found release locks except our own 
    // and pass status a long.
    if ( rtn == MISS ) {
	for (i=NUMPROC-1;i>=0;i--) {
	    rep=(FCMPartitionedSimple *)COGLOBAL(_repArray[i]);
	    if ( rep && rep!=this ) rep->_lock.releaseLock();
	}
    } else { // HIT || BUSY: If the page was found release all locks
	     // that where acquired. 
	for (;i>=0;i--) {
	    rep=(FCMPartitionedSimple *)COGLOBAL(_repArray[i]);
	    if ( rep ) rep->_lock.releaseLock();
	}
    }

    // Unlock the representative set via the Misshandler
    myMH->unlockReps();
    return rtn;
}


int
FCMPartitionedSimple :: findLocal( addr_t fileOffset, size_t len,
				   AccessType accessType,
				   RegionRef reg, void *a0, void *a1,
				   addr_t &result, u_int &ts )
{
    Page      *page;
    trace( MM, TR_DEBUG, tr_printf("FCMPartitionedSimple::findLocal()\n") );
    tassert( _lock.isLocked(), ass_printf(
	"FCMPartitionedSimple:findLocal: _lock should be held\n"));
    
    if( page = findPage( fileOffset ) ) {
	// check if page is in the middle of doing I/O
	if( page->doingIO ) {
	    addr_t dummy;
	    wassert( _regionCallbackList.find( fileOffset, dummy ),
		     ass_printf("no others doing I/O\n") );
	    // put on callback queue
	    FCMRegionCallback *cb = new FCMRegionCallback(reg,a0,a1);
	    _regionCallbackList.add( fileOffset, (addr_t &)cb );
	    // 1 means region must wait for a callback
	    return BUSY;
	}
	tassert( page->len == len,
		ass_printf("FCMPartitionedSimple::mismatched size: %lx"
			   " != %lx\n",
			page->len, len) );
	page->used = Page::SET;  // mark the page as referenced 
	if( accessType == WRITE ) {
	    page->dirty = Page::SET;
	}
	trace( MM, TR_DEBUG,
	       tr_printf( "page at paddr %lx has been referenced!!!\n",
			  page->paddr) );
        result = page->paddr;
	ts = COGLOBAL(_timestamp);
	trace( MM, TR_DEBUG, tr_printf("FCMPartitionedSimple: found(%lx,%lx)"
				       " -> %lx\n",
			  fileOffset, len, result ) );
	return HIT;
    }

    return MISS;
}

int
FCMPartitionedSimple :: handleAndClassifyMiss( addr_t fileOffset, size_t len,
					       AccessType accessType,
					       RegionRef reg, 
					       void *a0, void *a1,
					       addr_t &result, u_int &ts, 
					       FCMRegionCallback * &cb )
{
    Page       *page;
    TornStatus  rc;
    trace( MM, TR_DEBUG,
	   tr_printf("FCMPartitionedSimple::handleAndClassifyMiss()\n") );
    //FIXME:  This is wrong I am making a 
    result = DREF(COGLOBAL(_pm))->allocPage( rc ); // allocate a physical page
    tassert( result > 0, 0 );

    trace( MM, TR_DEBUG, tr_printf("FCMSimple: adding(%lx,%lx) -> %lx\n",
			  fileOffset, len, result ) );
    page = addPage( fileOffset, result, len );

    // we assume we will be doing I/O at this point
    cb = new FCMRegionCallback(reg,a0,a1);
    _regionCallbackList.add( fileOffset, (addr_t &)cb );
    page->doingIO = Page::SET;
    if( accessType == WRITE ) {
	page->dirty = Page::SET;
    }
    _lock.releaseLock();
    rc = DREF(COGLOBAL(_cor))->read( result, fileOffset, len );
    
    if ( rc == 1 ) return HANDLED_ASYNC;
    else {
	tassert( rc == 0, ass_printf("cor->read failed: %x\n", rc ) );
	return NOT_HANDLED_SYNC;
    }
}

// cleanCallBacks:
//    This function assumes that fileOffset indicates a address 
//    for which a call back is nolonger needed and that there
//    are call backs on the callback list associated id fileOffset.
//    It is assumed that cb holds the minimum one call back on
//    the list.  If no callbacks are found for fileOffset then return
//    0.  Note this routine deallocates cb. 
int 
FCMPartitionedSimple :: cleanCallBacks( addr_t fileOffset, size_t len,
					addr_t &result, u_int &ts, 
					FCMRegionCallback *cb )		       
{
    Page       *page;
    TornStatus  rc;
    trace( MM, TR_DEBUG,
	   tr_printf("FCMPartitionedSimple::cleanCallBacks() cb=%llx\n", cb) );
    _lock.acquireLock();

    page = findPage( fileOffset );
    if( page == 0 ) {
	// Could find not any callback for the indicated fileOffset
	_lock.releaseLock();
	wassert(0,ass_printf("FCMPartitioned Simple::cleanCallBacks:no page"
			     ": %lx\n", fileOffset));
	return 0;
    }
    tassert( page->paddr == PHYS_RAW(result),
	     ass_printf("page paddr mismatch: %lx != %lx\n", page->paddr, 
			result) );
    tassert( page->doingIO, ass_printf("Not doing I/O on page\n") );
    // clear doing-IO bit
    page->doingIO = Page::CLEAR;

    FCMRegionCallback *tmpcb;

    rc = _regionCallbackList.remove( fileOffset, (addr_t &)tmpcb );
    tassert( rc, ass_printf("couldn't find self\n") );

    ts = COGLOBAL(_timestamp);

    // did we find ourselves yet?
    int foundme = (tmpcb == cb);

    // we depend on short-circuit evaluation below
    if( tmpcb!=cb || _regionCallbackList.remove(fileOffset,(addr_t&)tmpcb )) {
	// someone else must also be waiting now; must issue callbacks
	ListSimple tmpList;

	tassert( tmpcb != cb, ass_printf("What happened 1\n") );
	tmpList.add( (addr_t)tmpcb );
	
	// build up temporary list of regions to callback
	while( _regionCallbackList.remove( fileOffset, (addr_t &)tmpcb ) ) {
	    if( tmpcb == cb ) {
		foundme = 1;
	    } else {
		tmpList.add( (addr_t)tmpcb );
	    }
	}
	tassert( foundme, ass_printf("what happened 2\n") );
	tassert( !tmpList.isEmpty(), ass_printf("what happened 3\n") );

	// we release the lock and do the callbacks using our local list
	_lock.releaseLock();

	// now call back all the regions on the tmpList
	while( tmpList.removeHead( (addr_t &)tmpcb ) ) {
	    // FIXME: should use timestamp in callback
	    DREF(tmpcb->reg)->ioDone(fileOffset, result, 
				     ts, tmpcb->a0, tmpcb->a1 );
	    delete tmpcb;
	}
	// all done except for calling myself
    } else {
	tassert( foundme, ass_printf("what happened 4\n") );
	_lock.releaseLock();
    }
    delete cb;
    return 1;
}

Page *
FCMPartitionedSimple :: findPage( addr_t fileOffset )
{
    Page *p;
    
    tassert( _lock.isLocked(), ass_printf(
	"FCM lock should be held before entering this function\n" ) ); 

    if( p = _pagelist.find( fileOffset ) )
	return p;
    else 
	trace( MM, TR_DEBUG, tr_printf("PC %lx: didn't find %lx\n",
				       this, fileOffset ));
    return 0;
}

Page *
FCMPartitionedSimple :: addPage( addr_t fileOffset, addr_t paddr, addr_t len )
{
    tassert( _lock.isLocked(), ass_printf(
	"FCM lock should be held before entering this function\n" ) ); 

    Page *p = new Page;
    trace( MM, TR_DEBUG, tr_printf("PC %lx: added %lx\n", this, fileOffset ));

    p->fileOffset  = fileOffset;
    p->paddr	   = paddr;
    p->len	   = len;
    p->used	   = Page::SET;
    p->dirty	   = Page::CLEAR;
    p->doingIO	   = Page::CLEAR;
//    p->intervalNum = 0;

//    _pagelist.acquireLock();
    _pagelist.enqueue( p );
//    _pagelist.releaseLock();

    return p;
}

// This is callback from cor on read I/O completion
TornStatus
FCMPartitionedSimple :: readDone( addr_t p, addr_t fileOffset, size_t len )
{
    Page *page;
    
    trace( MM, TR_DEBUG, tr_printf(
	"FCMPartitionedSimple::readDone(%lx,%lx,%lx)\n", p, fileOffset, len ) );

    _lock.acquireLock();

    /* Basic algorithm is as follows: look up request in pending queue
     * (verify there is one).  Look up page in page cache and verify
     * it still exists, is in doing-io mode, and the page's physical
     * address matches that passed to this routine.  Next check if there
     * are any more pending requests for this page on the queue.  If not,
     * just release lock and make callback to region.  If there are others,
     * then build up a temporary list with all those waiting, then release
     * lock and do callback.  For both cases, fcm timestamp must be snarfed
     * away with the lock held.
     */

    // For now we skip optimization for single guy waiting, and always
    // treat as a list

    page = findPage( fileOffset );
    if( page == 0 ) {
	// hmmm, where did the page go
	_lock.releaseLock();
	wassert(0,ass_printf(
	    "FCMPartitionedSimple::readDone:no page: %lx\n",fileOffset));
	// not much else we can do
	return 0;
    }
    tassert( page->paddr == PHYS_RAW(p),
	     ass_printf("page paddr mismatch: %lx != %lx\n", page->paddr, p) );
    wassert( page->len == len, ass_printf("page len mismatch: %lx != %lx\n",
					  page->len, len) );
    tassert( page->doingIO, ass_printf("Not doing I/O on page\n") );
    // clear doing-IO bit
    page->doingIO = Page::CLEAR;

    ListSimple tmpList;
    FCMRegionCallback *cb;

    // build up temporary list of regions to callback
    while( _regionCallbackList.remove( fileOffset, (addr_t &)cb ) ) {
	tmpList.add( (addr_t)cb );
    }
    wassert( !tmpList.isEmpty(), ass_printf("readDone: no one waiting\n") );

    // get local copy of current timestamp value
    unsigned int ts = COGLOBAL(_timestamp);

    // we release the lock and do the callbacks using our local list
    _lock.releaseLock();

    // now call back all the regions on the tmpList
    while( tmpList.removeHead( (addr_t &)cb ) ) {
	// FIXME: should use timestamp in callback
	DREF(cb->reg)->ioDone(fileOffset, p, ts, cb->a0, cb->a1 );
	delete cb;
    }

    return 0;
}


int
FCMPartitionedSimple :: getNumPages()
{
    FCMPartitionedSimple *rep;
    int sum=0;

    // Approximates the number of real pages held globally in the
    // FCM by summing each representatives number of Real pages
    for(int i=0; i<NUMPROC; i++) {
	rep = (FCMPartitionedSimple *)COGLOBAL(_repArray[i]);
        if ( rep ) sum+=rep->_pagelist.getNumPagesReal();
    }
    return sum;
}

int
FCMPartitionedSimple :: getPageBalance()
{
    FCMPartitionedSimple *rep;
    int sum=0;
    
    // Approximates the number of real pages held globally in the
    // FCM by summing each representatives number of Real pages
    for(int i=0; i<NUMPROC; i++) {
	rep = (FCMPartitionedSimple *)COGLOBAL(_repArray[i]);
        if ( rep ) {
	    sum+=rep->_pagelist.getNumPages();
	}
    }
    return sum;
}

// This is callback from cor on write I/O completion
TornStatus
FCMPartitionedSimple :: writeDone( addr_t fileOffset, size_t len )
{
    tassert( 0, ass_printf( "not supported yet\n" ) );
    return -1;
}

TornStatus
FCMPartitionedSimple :: writeBack()
{
    tassert ( 0, ass_printf(" writeBack : is experiencing technical"
			    " difficulties!\n"));
#if 0	
    AutoLock<SimpleSpinBlockLock> al(&_lock); // locks now, unlocks on return

    trace( MM, TR_INFO, 
	   tr_printf("trying to writeback FCMPartitionedSimple %lx\n", ref() ));

    writeBackPageCache();

    trace( MM, TR_INFO, 
	   tr_printf("writeback FCMPartitionedSimple %lx done\n",ref()));
#endif
    return 0;
}
 
void
FCMPartitionedSimple :: writeBackPageCache()
{
    tassert ( 0, ass_printf(" writeBackPageCache : is experiencing technical"
			    " difficulties!\n"));
#if 0    
    Page *p;

    trace( MM, TR_INFO, tr_printf("doing writeback\n") );

    tassert( _lock.isLocked(), ass_printf(
	"FCM lock should be held before entering this function\n" ) );
//    _pagelist.acquireLock();
    p = _pagelist.getPageListHead();
    while( p ) {
	if( p->dirty ) {
	    trace(MM, TR_INFO, tr_printf("writing back page off %lx\n",
					 p->fileOffset));
	    DREF(_cor)->write( p->paddr, p->fileOffset, p->len );
	}
	p = p->next;
    }
//    _pagelist.releaseLock();
#endif
}



TornStatus
FCMPartitionedSimple :: invalidate()
{
    tassert ( 0, ass_printf(" invalidate : is experiencing technical"
			    " difficulties!\n"));
#if 0
    AutoLock<SimpleSpinBlockLock> al(&_lock); // locks now, unlocks on return

    trace( MM, TR_INFO, 
	   tr_printf("trying to invalidate FCMPartitionedSimple %lx\n", ref() ));

    deallocPageCache();

    trace( MM, TR_INFO,
	   tr_printf("invalidated FCMPartitionedSimple %lx\n",ref()));
#endif
    return 0;
}

void
FCMPartitionedSimple :: deallocPageCache()
{

    tassert( COGLOBAL(_lock).isLocked(), ass_printf(
	"FCM lock should be held before entering this function\n" ) );
    FCMPartitionedSimple *rep;
    
    for(int i=0; i<NUMPROC; i++) {
	rep = (FCMPartitionedSimple *)COGLOBAL(_repArray[i]);
        if ( rep ) {
	    if (rep->_pagelist.getNumPages() > 0 ) {
		rep->_lock.acquireLock();
		trace( MM, ANY, tr_printf("FCM %lx: deallocing all pages"
					  " on rep=%llx\n", ref(), rep ) );
		// Clean the page lists. 
		rep->_pagelist.dealloc(COGLOBAL(_pm));
		rep->_lock.releaseLock();
	    }
	    else {
		trace( MM, ANY, tr_printf( "pagelist is empty for rep=%llx\n",
					   rep) );
	    }
	}
    }
}


TornStatus
FCMPartitionedSimple :: detachCOR( CORRef c )
{
    TornStatus rc;
//    FIXME:  Need to added locking in attachCOR for this to be safe??
    AutoLock<SimpleSpinBlockLock> al(&COGLOBAL(_lock)); // locks now, 
                                                        // unlocks on return
    
    trace( MM, TR_INFO, tr_printf("Destroying fcmsimple\n"));
    
    tassert( (COGLOBAL(_cor) == c) || 
	     (c == 0 && COGLOBAL(_cor) == (CORRef)OBJTRANS_DELETED),
	     ass_printf("Detach call by someone else: %lx != %lx\n", c,
			COGLOBAL(_cor)) );

    COGLOBAL(_cor) = (CORRef)OBJTRANS_DELETED;

//    deallocPageCache();
    prepareToDie();   // dealloc page cache and detach from PM
    
    rc = DREF(GOBJ::TheObjTransRef())->dealloc( _ref );

    return rc;
}

void
FCMPartitionedSimple :: prepareToDie()
// dealloc page cache and detach from PM
{
    TornStatus rc;
    
    tassert( COGLOBAL(_lock).isLocked(), ass_printf(
	"FCM lock should be held before entering this function\n" ) );

    /* FCM is being removed becase of one of the following reasons:
       - the last region was detached and we have no COR attached,
       - COR is getting detached and there are no more regions mapping us.
       Therefore, the accions are to dealloc all pages and to detach from
       PM */
    // make sure region list is emptyx

    tassert( COGLOBAL(_regionList).isEmpty(),
	     ass_printf( "region list is not empty\n" ) );

    deallocPageCache();
    // detach from pm
    if( COGLOBAL(_pm) ) {
	rc = DREF(COGLOBAL(_pm))->detachFCM( ref(), getPageBalance() );
	tassert( rc == 0, ass_printf("detach FCM from PM failed\n") );
	COGLOBAL(_pm) = (PMRef)OBJTRANS_DELETED;
    }
}

TornStatus
FCMPartitionedSimple :: cleanup()
{
    trace( MM, TR_INFO, tr_printf("cleanup rep FCMPartitionedSimple\n"));
    tassert( _regionCallbackList.isEmpty(), ass_printf("list not empty\n") );
    delete this;
    return 0;
}


