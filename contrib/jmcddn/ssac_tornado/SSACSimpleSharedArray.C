#include "SSACSimpleSharedArray.H"
#include <tornado/TAssert.H>
  
void
SSACSimpleSharedArray :: HashQueues :: init(const int &numentries)
{
    tassert((!entries),
	  ass_printf("\nSSACSimpleSharedArray::HashQueues:initq:entries!=0\n"));

    trace( MISC, TR_INFO,
	   tr_printf(
	       ">>> SSACSimpleSharedArray::HashQueues::init: on que=%lx\n",
	       this ));
    
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
#ifndef NOLOCKSNOFLAGSNOCOUNTERS	    
	if ( !( entries[i].flags & CacheEntrySimple::BUSY ) )
#endif
	    if (!ep)
	    {
		ep=&(entries[i]);
		if (!ep->id.valid())
		    break;
	    }
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	    else
		if ( entries[i].lastused < ep->lastused )
		    ep=&(entries[i]);
#endif
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
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
    count=0;
#endif
    entries=0;
}

SSACSimpleSharedArray :: HashQueues :: ~HashQueues()
{
    if (entries) delete[] entries;
}
  
SSACSimpleSharedArray :: SSACSimpleSharedArray( const int &numhashqs,
						const int &associativity)
    : _mh(this)
{
    _associativity=associativity;
    _numhashqs = numhashqs;
    _hashqs = new HashQueues[_numhashqs];
}


TornStatus 
SSACSimpleSharedArray :: get( CacheObjectId &id, CacheEntry* &ce,
			      const gettype &type )
{
    TornStatus rtn=0;
       
    CacheObjectIdSimple& theid=((CacheObjectIdSimple &)id);    
    register struct HashQueues *hashq;
    // the assignment of ce to ep is here just to avoid compliation warning
    register CacheEntrySimple *ep=(CacheEntrySimple *)ce;

    tassert(theid.valid(),
	    ass_printf("\nSSACSimpleSharedArray::get() target id=0x%x is invalid\n"
		       , theid.id()));
		       
    hashq = &(_hashqs[theid.index(_numhashqs)]);
    
    if (!hashq->entries) {
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	hashq->lock.acquireLock();
#endif
	hashq->init(_associativity);
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	hashq->lock.releaseLock();
#endif
    }

#ifndef NOLOCKSNOFLAGSNOCOUNTERS    
 again:    
    hashq->lock.acquireLock();
#endif    

    ep=hashq->search(theid,_associativity);

    if (ep)
    {
	// hit
	trace( MISC, TR_INFO,
	       tr_printf(
	   ">>> SSACSimpleSharedArray::get: Hit: id=%d index=%d ep=%llx:\n",
		   theid.id(), theid.index(_numhashqs), ep );
	       ep->print()
	    );
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	if ( ep->flags & CacheEntrySimple::BUSY )
	{
	    hashq->lock.releaseLock();
	    ep->sleep();
	    goto again;
	}
#endif
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	hashq->count++;
	if (!hashq->count) hashq->rollover();
	ep->lastused=hashq->count;
	if ( type == SSAC::GETFORWRITE )
	    ep->flags |= CacheEntrySimple::BUSY;
	hashq->lock.releaseLock();
#endif  
	ce=ep;
	SET_CLSCD(rtn,1);
	return rtn;
    } 
    else
    {
	// miss
	trace( MISC, TR_INFO,
	       tr_printf(
		   ">>> SSACSimpleSharedArray::get: Miss: id=%d index=%d\n",
		   theid.id(),theid.index(_numhashqs)));

	ep=hashq->lruentry(_associativity);
	
	if (ep)
	{ 
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	    if (ep->flags & CacheEntrySimple::DIRTY)
#endif
	    {
		//clean the entry
		trace( MISC, TR_INFO,
		       tr_printf(
		   ">>> SSACSimpleSharedArray::get:Miss: Cleaning entry:\n"
			   );
		           ep->print());
		ep->id.save(ep->data);
#ifndef NOLOCKSNOFLAGSNOCOUNTERS 
		ep->flags &= ~CacheEntrySimple::DIRTY;
#endif 
	    }
	    ep->id=theid;
	    ep->data=ep->id.load();
	}
	else  
	{
	    // no free elements take the simple way out for the moment
	    // just pass back an error code
	    trace( MISC, TR_INFO,
		   tr_printf(
		    ">>> SSACSimpleSharedArray::get: Miss:*NOFREE ENTRIES*\n"
		       ));
	    SET_CLSCD(rtn,0);
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	    hashq->lock.releaseLock();
#endif
	    return rtn;
	}
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	hashq->count++;
	if (!hashq->count) hashq->rollover();
	ep->lastused=hashq->count;
	if ( type == SSAC::GETFORWRITE )
	    ep->flags |= CacheEntrySimple::BUSY;
	hashq->lock.releaseLock();
#endif 
	ce=ep;
	SET_CLSCD(rtn,1);
	return rtn;
    }
}

TornStatus
SSACSimpleSharedArray :: putback( CacheEntry* &ce, const putflag &flag )
{

    register CacheEntrySimple *entry=(CacheEntrySimple *)ce;
    register struct HashQueues *hashq=&(_hashqs[entry->id.index(_numhashqs)]);

    if (!entry) return 0;
    
    tassert((hashq->entries),
	 ass_printf("\n\nSSACSimpleSharedArray::putback: bad queue entry:\n");
	    entry->print());
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
    hashq->lock.acquireLock();
#endif
    if (flag == KEEP)
    {
	trace( MISC, TR_INFO,
	       tr_printf(">>> SSACSimpleSharedArray::putback: KEEP entry:");
	       entry->print()
	    );
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
    	entry->lastused=hashq->count;
#endif
    }
    else
    {
	trace( MISC, TR_INFO,
	     tr_printf(">>> SSACSimpleSharedArray::putback: DISCARD entry:");
	       entry->print()
	    );
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	entry->lastused=0;
#endif
    }
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
    entry->flags &= ~CacheEntrySimple::BUSY;
    hashq->lock.releaseLock();
#endif
    entry->wakeup();
    return 0;
}

TornStatus
SSACSimpleSharedArray :: flush()
{
    register struct HashQueues *hashq;
    register CacheEntrySimple *ce;
    register int i;
    
    for (i=0; i<_numhashqs; i++)
    {
	hashq = &(_hashqs[i]);
	if (!hashq->entries) continue;
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
    again:	
	hashq->lock.acquireLock();
#endif
	for ( register int j=0; j<_associativity; j++ )
	{
	    ce=&(hashq->entries[j]);
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	    if ( !(ce->flags & CacheEntrySimple::BUSY)
               && (ce->id.valid())	    
		 && ( ce->flags & CacheEntrySimple::DIRTY ))
	    {
		trace( MISC, TR_INFO,
		       tr_printf(">>> SSACSimpleSharedArray::flush: entry:");
		       ce->print()
		       );
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
#endif
	}
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	hashq->lock.releaseLock();
#endif
    }
    return 0;
}

SSACSimpleSharedArray :: ~SSACSimpleSharedArray()
{
    trace( MISC, TR_INFO,
	  tr_printf("**** ~SSACSimpleSharedArray ref=%lx\n",_ref));
    delete[] _hashqs; 
}

TornStatus
SSACSimpleSharedArray :: snapshot()
{
    register struct HashQueues *hashq;
    register int i,j,flags;
    
    for (i=0; i<_numhashqs; i++)
    {
	hashq = &(_hashqs[i]);
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	hashq->lock.acquireLock();
	tr_printf("_hashq[%d].count=%U\n_hashqs[%d].entries: ",i,hashq->count,i);
#endif
	if (hashq->entries)
	    for ( j=0; j<_associativity; j++)
	    {
		tr_printf("%d:%d:%U:%lx:",j,
			  hashq->entries[j].id.id(),
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
			  hashq->entries[j].lastused,
#endif
			  hashq->entries[j].data);
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
		flags=(hashq->entries[j]).flags;
		if (flags & CacheEntrySimple::BUSY)
		    printf("B|");
		else printf("F|");
		if (flags & CacheEntrySimple::DIRTY)
		    printf("D ");
		else printf("C ");
#endif
	    }
	else
	    tr_printf("NULL");
	tr_printf("\n");
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
	hashq->lock.releaseLock();
#endif
    }
    return 0;
}
  
 
 
