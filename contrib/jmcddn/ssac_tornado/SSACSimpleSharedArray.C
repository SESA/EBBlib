#include <stdio.h>
#include <stdlib.h>

#include "EBBKludge.H"
#include "SSACSimpleSharedArray.H"

//#define DOTRACE 1
  
void
SSACSimpleSharedArray :: HashQueues :: init(const int &numentries)
{
    tassert((!entries),
	  ass_printf("\nSSACSimpleSharedArray::HashQueues:initq:entries!=0\n"));

#ifdef DOTRACE
    trace( MISC, TR_INFO,
	   tr_printf(
	       ">>> SSACSimpleSharedArray::HashQueues::init: on que=%p\n",
	       this ));
#endif
    
    entries=new CacheEntrySimple[numentries];
}

CacheEntrySimple *
SSACSimpleSharedArray :: HashQueues :: search(CacheObjectIdSimple &id,
						  const int &numentries)
{
    register int i=0;
    while (i<numentries)
    {
	if (id == entries[i].id)
	    return &(entries[i]);
	i++;
    }
    return (CacheEntrySimple *)0;
}

CacheEntrySimple *
SSACSimpleSharedArray :: HashQueues :: lruentry(const int &numentries)
{
    // find least recently used
    // Note this could have been combined with first search but then
    // would have add expense to a hit.  this search should benifit
    // from a warmer cache.
    register int i=0;
    register CacheEntrySimple *ep=0;
    
    while (i<numentries)
    {
	if ( !( entries[i].flags & CacheEntrySimple::BUSY ) )
	    if (!ep)
	    {
		ep=&(entries[i]);
		if (!ep->id.valid())
		    break;
	    }
	    else
		if ( entries[i].lastused < ep->lastused )
		    ep=&(entries[i]);
	i++;
    }
    return ep;
}

void
SSACSimpleSharedArray :: HashQueues :: rollover()
{
    tassert(0,
	    ass_printf("\nSSACSimpleSharedArray::HashQueues:rollover() called\n"));
}

SSACSimpleSharedArray :: HashQueues :: HashQueues()
{
    count=0;
    entries=0;
}

SSACSimpleSharedArray :: HashQueues :: ~HashQueues()
{
    if (entries) delete[] entries;
}
  
SSACSimpleSharedArray :: SSACSimpleSharedArray( const int &numhashqs,
						const int &associativity)
{
    _associativity=associativity;
    _numhashqs = numhashqs;
    _hashqs = new HashQueues[_numhashqs];
    _ref = this;
}


EBBRC 
SSACSimpleSharedArray :: get( CacheObjectId &id, CacheEntry* &ce,
			      const gettype &type )
{
    EBBRC rtn=0;
       
    CacheObjectIdSimple& theid=((CacheObjectIdSimple &)id);    
    register struct HashQueues *hashq;
    // the assignment of ce to ep is here just to avoid compliation warning
    register CacheEntrySimple *ep=(CacheEntrySimple *)ce;

    tassert(theid.valid(),
	    ass_printf("\nSSACSimpleSharedArray::get() target id=0x%x is invalid\n"
		       , theid.id()));
		       
    hashq = &(_hashqs[theid.index(_numhashqs)]);
    
    if (!hashq->entries) {
	hashq->lock.acquireLock();
	hashq->init(_associativity);
	hashq->lock.releaseLock();
    }

 again:    
    hashq->lock.acquireLock();

    ep=hashq->search(theid,_associativity);

    if (ep)
    {
#ifdef DOTRACE
	trace( MISC, TR_INFO,
	       tr_printf(
	   ">>> SSACSimpleSharedArray::get: Hit: id=%d index=%d ep=%p:\n",
	   theid.id(), theid.index(_numhashqs), ep );
	       ep->print()
	       );
#endif
	if ( ep->flags & CacheEntrySimple::BUSY )
	{
	    hashq->lock.releaseLock();
	    ep->sleep();
	    goto again;
	}

	hashq->count++;
	if (!hashq->count) hashq->rollover();
	ep->lastused=hashq->count;
	if ( type == SSAC::GETFORWRITE )
	    ep->flags |= CacheEntrySimple::BUSY;
	hashq->lock.releaseLock();
	ce=ep;
	SET_CLSCD(rtn,1);
	return rtn;
    } 
    else
    {
	// miss
#ifdef DOTRACE
	trace( MISC, TR_INFO,
	       tr_printf(
		   ">>> SSACSimpleSharedArray::get: Miss: id=%d index=%d\n",
		   theid.id(),theid.index(_numhashqs)));
#endif
	ep=hashq->lruentry(_associativity);
	
	if (ep)
	{ 
	    if (ep->flags & CacheEntrySimple::DIRTY)
	    {
		
		//clean the entry
#ifdef DOTRACE
		trace( MISC, TR_INFO,
		       tr_printf(
		   ">>> SSACSimpleSharedArray::get:Miss: Cleaning entry:\n"
			   )
			//,ep->print()
			);
#endif
		ep->id.save(ep->data);
		ep->flags &= ~CacheEntrySimple::DIRTY;
	    }
	    ep->id=theid;
	    ep->data=ep->id.load();
	}
	else  
	{
	    // no free elements take the simple way out for the moment
	    // just pass back an error code
#ifdef DOTRACE
	    trace( MISC, TR_INFO,
		   tr_printf(
		    ">>> SSACSimpleSharedArray::get: Miss:*NOFREE ENTRIES*\n"
		       ));
#endif
	    SET_CLSCD(rtn,0);
	    hashq->lock.releaseLock();
	    return rtn;
	}
	hashq->count++;
	if (!hashq->count) hashq->rollover();
	ep->lastused=hashq->count;
	if ( type == SSAC::GETFORWRITE )
	    ep->flags |= CacheEntrySimple::BUSY;
	hashq->lock.releaseLock();
	ce=ep;
	SET_CLSCD(rtn,1);
	return rtn;
    }
}

EBBRC
SSACSimpleSharedArray :: putback( CacheEntry* &ce, const putflag &flag )
{

    register CacheEntrySimple *entry=(CacheEntrySimple *)ce;
    register struct HashQueues *hashq=&(_hashqs[entry->id.index(_numhashqs)]);

    if (!entry) return 0;
    
    tassert((hashq->entries),
	    ass_printf("\n\nSSACSimpleSharedArray::putback: bad queue entry:\n");
	    entry->print()
	    );
    hashq->lock.acquireLock();
    if (flag == KEEP)
    {
#ifdef DOTRACE
	trace( MISC, TR_INFO,
	       tr_printf(">>> SSACSimpleSharedArray::putback: KEEP entry:\n");
	       entry->print()
	    );
#endif
    	entry->lastused=hashq->count;
    }
    else
    {
#ifdef DOTRACE
	trace( MISC, TR_INFO,
	       tr_printf(">>> SSACSimpleSharedArray::putback: DISCARD entry:\n");
	       entry->print()
	    );
#endif
	entry->lastused=0;
    }
    entry->flags &= ~CacheEntrySimple::BUSY;
    hashq->lock.releaseLock();
    entry->wakeup();
    return 0;
}

EBBRC
SSACSimpleSharedArray :: flush()
{
    register struct HashQueues *hashq;
    register CacheEntrySimple *ce;
    register int i;
    
    for (i=0; i<_numhashqs; i++)
    {
	hashq = &(_hashqs[i]);
	if (!hashq->entries) continue;
    again:	
	hashq->lock.acquireLock();
	for ( register int j=0; j<_associativity; j++ )
	{
	    ce=&(hashq->entries[j]);
	    if ( !(ce->flags & CacheEntrySimple::BUSY)
               && (ce->id.valid())	    
		 && ( ce->flags & CacheEntrySimple::DIRTY ))
	    {
#ifdef DOTRACE
		trace( MISC, TR_INFO,
		       tr_printf(">>> SSACSimpleSharedArray::flush: entry:")//,ce->print()
		       );
#endif
		ce->flags |= CacheEntrySimple::BUSY;
		hashq->lock.releaseLock();
		ce->id.save(ce->data);
		hashq->lock.acquireLock();
		ce->flags &= ~CacheEntrySimple::DIRTY &
		             ~CacheEntrySimple::BUSY;
		hashq->lock.releaseLock();
		ce->wakeup();
		goto again;
	    }
	}
	hashq->lock.releaseLock();
    }
    return 0;
}

SSACSimpleSharedArray :: ~SSACSimpleSharedArray()
{
#ifdef DOTRACE
    trace( MISC, TR_INFO,
	  tr_printf("**** ~SSACSimpleSharedArray ref=%p\n",_ref));
#endif
    delete[] _hashqs; 
}

EBBRC
SSACSimpleSharedArray :: snapshot()
{
    register struct HashQueues *hashq;
    register int i,j,flags;
    
    for (i=0; i<_numhashqs; i++)
    {
	hashq = &(_hashqs[i]);
	hashq->lock.acquireLock();
	tr_printf("_hashq[%d].count=%ld\n_hashqs[%d].entries: ",i,hashq->count,i);
	if (hashq->entries)
	    for ( j=0; j<_associativity; j++)
	    {
		tr_printf("%d:%d:%ld:%p:",j,
			  hashq->entries[j].id.id(),
			  hashq->entries[j].lastused,
			  hashq->entries[j].data);
		flags=(hashq->entries[j]).flags;
		if (flags & CacheEntrySimple::BUSY)
		    printf("B|");
		else printf("F|");
		if (flags & CacheEntrySimple::DIRTY)
		    printf("D ");
		else printf("C ");
	    }
	else
	    tr_printf("NULL");
	tr_printf("\n");
	hashq->lock.releaseLock();
    }
    return 0;
}
  
 
 
