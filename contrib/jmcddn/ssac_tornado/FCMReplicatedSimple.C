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
 * $Id: FCMReplicatedSimple.C,v 1.3 1998/12/09 05:26:06 jonathan Exp $
 *****************************************************************************/

/*****************************************************************************
 * Module Description: Very simple first cut at an FCM
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
#include "FCMReplicatedSimple.H"


TornStatus
FCMReplicatedSimple :: find( addr_t fileOffset, size_t len,
			     AccessType accessType,
			     RegionRef reg, void *a0, void *a1,
			     addr_t &result, u_int &ts )
{
    TornStatus rc;
    addr_t     p;
    Page      *page;

retry:
    
    _lock.acquireLock();
    
    if( page = findPage( fileOffset ) ) {

	// check if page is in the middle of doing I/O
	if( page->doingIO ) {
	    addr_t dummy;
	    wassert( _regionCallbackList.find( fileOffset, dummy ),
		     ass_printf("no others doing I/O\n") );
	    // put on callback queue
	    FCMRegionCallback *cb = new FCMRegionCallback(reg,a0,a1);
	    _regionCallbackList.add( fileOffset, (addr_t &)cb );
	    _lock.releaseLock();
	    // 1 means region must wait for a callback
	    return 1;
	}
	tassert( page->len == len,
		ass_printf("FCMReplicatedSimple::mismatched size: %lx"
			   " != %lx\n",
			page->len, len) );
	page->used = Page::SET;  // mark the page as referenced 
	if( accessType == WRITE ) {
	    page->dirty = Page::SET;
	}
	trace( MM, TR_DEBUG,
	       tr_printf( "page at paddr %lx has been referenced!!!\n",
			  page->paddr) );
	p = page->paddr;
	ts = _timestamp;
	_lock.releaseLock();
	result = p;
	trace( MM, TR_DEBUG, tr_printf("FCMReplicatedSimple: found(%lx,%lx)"
				       " -> %lx\n",
			  fileOffset, len, p ) );
	return 0;
    }

    /* allocation of memory is no longer through DramManager, but rather
       through Page Manager (PM) */
//    p = DREF(GOBJ::TheDramManagerRef())->allocMemory( len, rc );
    p = DREF(_pm)->allocPage( rc );  // allocate a physical page
    tassert( p > 0, 0 );
    
    trace( MM, TR_DEBUG, tr_printf("FCMReplicatedSimple: adding(%lx,%lx)"
				   " -> %lx\n",
			  fileOffset, len, p ) );
    page = addPage( fileOffset, p, len );

    // we assume we will be doing I/O at this point
    FCMRegionCallback *cb = new FCMRegionCallback(reg,a0,a1);
    _regionCallbackList.add( fileOffset, (addr_t &)cb );
    page->doingIO = Page::SET;
    if( accessType == WRITE ) {
	page->dirty = Page::SET;
    }

    _lock.releaseLock();
    rc = DREF(_cor)->read( p, fileOffset, len );

    // if rc == 1, this means we must wait for a callback from the cor
    if( rc == 1 ) {
	return 1;
    }
    tassert( rc == 0, ass_printf("cor->read failed: %x\n", rc ) );

    // It seems we didn't need to do I/O or at least it wasn't asynchronous

    // at this point, the whole world could have changed; we treat as
    // if it were a callback from a cor on I/O completion and recheck
    // everything
    // check readDone call for proper solution (should encapsulate)
    
    _lock.acquireLock();

    page = findPage( fileOffset );
    if( page == 0 ) {
	// hmmm, where did the page go
	_lock.releaseLock();
	wassert(0,ass_printf("FCMReplicatedSimple::readDone:no page: %lx\n",
			     fileOffset));
	// retry
	goto retry;
    }
    tassert( page->paddr == PHYS_RAW(p),
	     ass_printf("page paddr mismatch: %lx != %lx\n", page->paddr, p) );
    tassert( page->doingIO, ass_printf("Not doing I/O on page\n") );
    // clear doing-IO bit
    page->doingIO = Page::CLEAR;

    FCMRegionCallback *tmpcb;

    rc = _regionCallbackList.remove( fileOffset, (addr_t &)tmpcb );
    tassert( rc, ass_printf("couldn't find self\n") );

    ts = _timestamp;

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
	    DREF(tmpcb->reg)->ioDone(fileOffset, p, ts, tmpcb->a0, tmpcb->a1 );
	    delete tmpcb;
	}
	// all done except for calling myself
    } else {
	tassert( foundme, ass_printf("what happened 4\n") );
	_lock.releaseLock();
    }
	
    result = p;

    delete cb;

    return 0;
}


Page *
FCMReplicatedSimple :: findPage( addr_t fileOffset )
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
FCMReplicatedSimple :: addPage( addr_t fileOffset, addr_t paddr, addr_t len )
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
FCMReplicatedSimple :: readDone( addr_t p, addr_t fileOffset, size_t len )
{
    Page *page;
    
    trace( MM, TR_DEBUG, tr_printf(
	"FCMReplicatedSimple::readDone(%lx,%lx,%lx)\n", p, fileOffset, len ) );

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
	    "FCMReplicatedSimple::readDone:no page: %lx\n",fileOffset));
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
    unsigned int ts = _timestamp;

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


// This is callback from cor on write I/O completion
TornStatus
FCMReplicatedSimple :: writeDone( addr_t fileOffset, size_t len )
{
    tassert( 0, ass_printf( "not supported yet\n" ) );
    return -1;
}

TornStatus
FCMReplicatedSimple :: writeBack()
{
    AutoLock<SimpleSpinBlockLock> al(&_lock); // locks now, unlocks on return

    trace( MM, TR_INFO, 
	   tr_printf("trying to writeback FCMReplicatedSimple %lx\n", ref() ));

    writeBackPageCache();

    trace( MM, TR_INFO, 
	   tr_printf("writeback FCMReplicatedSimple %lx done\n",ref()));

    return 0;
}

void
FCMReplicatedSimple :: writeBackPageCache()
{
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
}


TornStatus
FCMReplicatedSimple :: invalidate()
{
    AutoLock<SimpleSpinBlockLock> al(&_lock); // locks now, unlocks on return

    trace( MM, TR_INFO, 
	   tr_printf("trying to invalidate FCMReplicatedSimple %lx\n", ref() ));

    deallocPageCache();

    trace( MM, TR_INFO,
	   tr_printf("invalidated FCMReplicatedSimple %lx\n",ref()));

    return 0;
}

void
FCMReplicatedSimple :: deallocPageCache()
{
    tassert( _lock.isLocked(), ass_printf(
	"FCM lock should be held before entering this function\n" ) );

    if( _pagelist.getNumPages() == 0 ) {
	trace( MM, ANY, tr_printf( "pagelist is empty\n" ) );
	return;
    }
    
    trace( MM, ANY, tr_printf("FCM %lx: deallocing all pages\n", ref()) );
    // Clean the page lists. 
    _pagelist.dealloc( _pm );  // pm is our page manager
}


TornStatus
FCMReplicatedSimple :: detachCOR( CORRef c )
{
    TornStatus rc;
    AutoLock<SimpleSpinBlockLock> al(&_lock); // locks now, unlocks on return
    
    trace( MM, TR_INFO, tr_printf("Destroying fcmsimple\n"));
    
    tassert( (_cor == c) || (c == 0 && _cor == (CORRef)OBJTRANS_DELETED),
	     ass_printf("Detach call by someone else: %lx != %lx\n", c, _cor) );

    _cor = (CORRef)OBJTRANS_DELETED;

//    deallocPageCache();
    prepareToDie();   // dealloc page cache and detach from PM
    
    rc = DREF(GOBJ::TheObjTransRef())->dealloc( _ref );

    return rc;
}

void
FCMReplicatedSimple :: prepareToDie()
// dealloc page cache and detach from PM
{
    TornStatus rc;
    
    tassert( _lock.isLocked(), ass_printf(
	"FCM lock should be held before entering this function\n" ) );

    /* FCM is being removed becase of one of the following reasons:
       - the last region was detached and we have no COR attached,
       - COR is getting detached and there are no more regions mapping us.
       Therefore, the accions are to dealloc all pages and to detach from
       PM */
    // make sure region list is emptyx
    tassert( _regionList.isEmpty(),
	     ass_printf( "region list is not empty\n" ) );

    deallocPageCache();
    // detach from pm
    if( _pm ) {
	rc = DREF(_pm)->detachFCM( ref(), getPageBalance() );
	tassert( rc == 0, ass_printf("detach FCM from PM failed\n") );
	_pm = (PMRef)OBJTRANS_DELETED;
    }
}

TornStatus
FCMReplicatedSimple :: cleanup()
{
    trace( MM, TR_INFO, tr_printf("cleanup fcmsimple\n"));
    tassert( _regionCallbackList.isEmpty(), ass_printf("list not empty\n") );
    delete this;
    return 0;
}


