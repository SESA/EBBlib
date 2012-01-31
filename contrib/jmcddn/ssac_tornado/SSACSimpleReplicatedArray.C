#include "SSACSimpleReplicatedArray.H"
#include "misc/GLocal.H"
#include <tornado/ListSimple.H>
#include <tornado/TAssert.H>
 
void
SSACSimpleReplicatedArray :: HashQueues :: init(const int &numentries)
{
    tassert((!entries),
	  ass_printf("\nSSACSimpleReplicatedArray::HashQueues:initq:entry!=0\n"));

    trace( MISC, TR_INFO,
	   tr_printf(
	       ">>> SSACSimpleReplicatedArray::HashQueues::init: on que=%lx\n",
	       this ));
    
    entries=new CacheEntrySimple[numentries];
}

void
SSACSimpleReplicatedArray :: HashQueues :: rollover()
{
    tassert(0,
	    ass_printf("\nSSACSimpleReplicatedArray::HashQueues:rollover() called\n"));
}

SSACSimpleReplicatedArray :: HashQueues :: HashQueues()
{
    count=0;
    entries=0;
}
  
SSACSimpleReplicatedArray :: HashQueues :: ~HashQueues()
{
    if (entries) delete[] entries;
}
  
SSACSimpleReplicatedArray :: SSACSimpleReplicatedArray( const int &numhashqs,
							const int &associativity)
{
    _associativity=associativity;
    _numhashqs = numhashqs;
    _hashqs = new HashQueues[_numhashqs];

    if (MYVP < _numhashqs % NUMPROC)
    {
	_dirlines = new  struct DirLine [_numhashqs/NUMPROC + 1];
	for(int i=0;i<_numhashqs/NUMPROC +1;i++)
	    _dirlines[i].direntries=new struct DirEntry [_associativity*NUMPROC];
    }
    else
    {
	_dirlines = new struct DirLine [_numhashqs/NUMPROC];
	for(int i=0;i<_numhashqs/NUMPROC;i++)
	    _dirlines[i].direntries=new struct DirEntry [_associativity*NUMPROC];
    }
}


CacheEntrySimple *
SSACSimpleReplicatedArray :: HashQueues :: search(CacheObjectIdSimple &id,
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
SSACSimpleReplicatedArray :: HashQueues :: lruentry(const int &numentries)
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

struct SSACSimpleReplicatedArray::DirEntry *
SSACSimpleReplicatedArray :: DirLine :: lookupid(CacheObjectIdSimple &id,
						 const int &associativity)
{
    register int i=0;
    register int numdirentries=associativity*NUMPROC;
    while (i<numdirentries)
    {
	if (id == direntries[i].id)
	{
	    return &(direntries[i]);
	}
	i++;
    }
    return 0;
}

void
SSACSimpleReplicatedArray :: DirLine :: adddirentry(CacheObjectIdSimple &id,
						    const int &vp,
						    const int &associativity)
{
    struct SSACSimpleReplicatedArray::DirEntry *dep=0;
    register int i=0;
    register int numdirentries=associativity*NUMPROC;
    while (i<numdirentries)
    {
	if (!direntries[i].id.valid())
	{
	    dep=&(direntries[i]);
	    break;
	}
	i++;
    } 
    tassert ( dep,
	      ass_printf("\n can't find a free directory entry\n"));
    dep->id=id;
    dep->clearall();
    dep->addvp(vp);

    return;
}

struct CacheEntrySimple *
SSACSimpleReplicatedArray :: getremotecopyfromdir(struct DirEntry *direntry)
{
    register reg_t mask=direntry->vpmask;
    register SSACSimpleReplicatedArray *rep=0;
    register struct HashQueues *hashq=0;
    register CacheEntrySimple *ep=0;
    register int index=direntry->id.index(_numhashqs);

    for (int vp=0; mask!=0;mask>>=1,vp++)
	if (mask & 0x1)
	{
	    tassert(vp!=MYVP, ass_printf("\n Opps trying to get remote copy from same processor\n" ));
	    // found a copy on vp
	    rep = (SSACSimpleReplicatedArray *)
		((SSACSimpleReplicatedMH *)MYMHO)->findRepOn(vp);
	    tassert(rep, ass_printf(
		"\n DirEntry id=%d vpmask=0x%llx can't find rep on vp=%d\n",
		direntry->id.id(),direntry->vpmask,vp));
	    hashq=&(rep->_hashqs[index]);
	    hashq->lock.acquireLock();  // FIXME: don't think this is neccessary
	    ep=hashq->search(direntry->id,_associativity);
	    hashq->lock.releaseLock(); // FIXME: don't think this is neccessary
	    tassert(ep, ass_printf(
		"\n DirEntry id=%d vpmask=0x%llx can't find Entry on vp=%d\n",
		direntry->id.id(),direntry->vpmask,vp));
	    return ep;
	}
    return 0;
}

void
SSACSimpleReplicatedArray :: DirEntry :: spin()
{
    register int i=0;
    while(i<BACKOFF) i++;
}

void
SSACSimpleReplicatedArray :: setbusyonallcopiesindir(struct DirEntry *direntry,
						  CacheEntrySimple *local)
{
// This function assumes that the invoking process
// has the direntry lock and the lock for the entry on this processor
// if local is not null
    register repmask=0;
    register SSACSimpleReplicatedArray *rep=0;
    struct HashQueues *hashq=0;
    register CacheEntrySimple *ep=0;
    register int index;
    ListSimple ques;
    void *cur=0;

    tassert(~direntry->busy(), ass_printf("\nEntry already busy\n"));
	    
    if (local)
	repmask=direntry->vpmask & ~(1<<MYVP);
    else
	repmask=direntry->vpmask;

    index=direntry->id.index(_numhashqs);
    for ( register int vp=0; repmask!=0; repmask>>=1,vp++ )
	if (repmask & 0x1)
	{
	    // found a copy on vp
	    rep=(SSACSimpleReplicatedArray *)
		((SSACSimpleReplicatedMH *)MYMHO)->findRepOn(vp);
	    tassert(rep, ass_printf(
		"\n DirEntry id=%d vpmask=0x%llx can't find rep on vp=%d\n",
		direntry->id.id(),direntry->vpmask,vp));
	    hashq=&(rep->_hashqs[index]);
	    hashq->lock.acquireLock(); 
	    ques.add((addr_t)hashq);
	}
    
    for ( cur=ques.next((void *)0,(addr_t &)hashq);
	  cur; cur=ques.next(cur,(addr_t &)hashq) )
    {
	ep=hashq->search(direntry->id,_associativity);
	tassert(ep, ass_printf(
	    "\n DirEntry id=%d vpmask=0x%llx can't find Entry on vp=%d\n",
	    direntry->id.id(),direntry->vpmask,vp));
	ep->flags |= CacheEntrySimple::BUSY;
    }
    
    while (ques.removeHead((addr_t &)hashq))
    {
	hashq->lock.releaseLock();
    }
    
    if (local) local->flags |= CacheEntrySimple::BUSY;
    direntry->setbusy();
    return;
}

void
SSACSimpleReplicatedArray :: updateandclearbusyonallcopiesindir(struct DirEntry *direntry,
								CacheEntrySimple *local)
{
// This function assumes that the invoking process
// has the direntry lock and the lock for the entry on this processor
// if local is not null
    register repmask=0;
    register SSACSimpleReplicatedArray *rep=0;
    struct HashQueues *hashq=0;
    register CacheEntrySimple *ep=0;
    register int index;
    ListSimple ques;
    void *cur=0;

    tassert(~direntry->busy(), ass_printf("\nEntry already busy\n"));
    tassert(local, ass_printf("\nNo Local copy specified\n"));	    

    repmask=direntry->vpmask & ~(1<<MYVP);

    index=direntry->id.index(_numhashqs);
    for ( register int vp=0; repmask!=0; repmask>>=1,vp++ )
	if (repmask & 0x1)
	{
	    // found a copy on vp
	    rep=(SSACSimpleReplicatedArray *)
		((SSACSimpleReplicatedMH *)MYMHO)->findRepOn(vp);
	    tassert(rep, ass_printf(
		"\n DirEntry id=%d vpmask=0x%llx can't find rep on vp=%d\n",
		direntry->id.id(),direntry->vpmask,vp));
	    hashq=&(rep->_hashqs[index]);
	    hashq->lock.acquireLock(); 
	    ques.add((addr_t)hashq);
	}
    
    for ( cur=ques.next((void *)0,(addr_t &)hashq);
	  cur; cur=ques.next(cur,(addr_t &)hashq) )
    {
	ep=hashq->search(direntry->id,_associativity);
	tassert(ep, ass_printf(
	    "\n DirEntry id=%d vpmask=0x%llx can't find Entry on vp=%d\n",
	    direntry->id.id(),direntry->vpmask,vp));
	ep->data = local->data;
	ep->flags &= ~CacheEntrySimple::BUSY;
    }
    
    while (ques.removeHead((addr_t &)hashq))
    {
	hashq->lock.releaseLock();
    }
    
    local->flags &= ~CacheEntrySimple::BUSY;
    direntry->clearbusy();
    return;
}

struct SSACSimpleReplicatedArray::DirLine *
SSACSimpleReplicatedArray :: finddirline(CacheObjectIdSimple &id)
{
   int vp=id.index(_numhashqs)%NUMPROC;
   ClusteredObject *rep=((SSACSimpleReplicatedMH *)MYMHO)->findRepOn(vp);
   if (!rep)
       rep=((SSACSimpleReplicatedMH *)MYMHO)->createRepOn(vp);
   return &(((SSACSimpleReplicatedArray *)rep)->
	    _dirlines[(id.index(_numhashqs)-vp)/NUMPROC]);
}


int
SSACSimpleReplicatedArray :: removefromdir(CacheObjectIdSimple &id,
					   const int &vp, int dirty)
{
    register int clean=0;
    struct DirEntry *dep=(finddirline(id))->lookupid(id,_associativity);
    
    tassert(dep, ass_printf(
	"\nTrying to remove id,vp from the directory but and entry can't be found\n"
	));
    if (dirty) dep->setmodified();
    dep->removevp(vp);
    if (!dep->vpmask)
    {
	dep->id.invalidate();
	if (dep->modified()) clean=1;
	dep->clearall();
    }
    return clean;
}
   

TornStatus 
SSACSimpleReplicatedArray :: get( CacheObjectId &id, CacheEntry* &ce,
			      const gettype &type )
{
    TornStatus rtn=0;
       
    CacheObjectIdSimple& theid=((CacheObjectIdSimple &)id);    
    register struct HashQueues *hashq=0;
    // the assignment of ce to ep is here just to avoid compliation warning
    register CacheEntrySimple *ep=(CacheEntrySimple *)ce;
    register struct DirLine *dirline=0;
    register struct DirEntry *direntry=0;
 	
    tassert(theid.valid(),ass_printf("\nSSACSimpleReplicatedArray::get() target id=0x%x is invalid\n",
				     theid.id()));
		       
    hashq = &(_hashqs[theid.index(_numhashqs)]);
    if (!hashq->entries) {
	hashq->lock.acquireLock();
	hashq->init(_associativity);
	hashq->lock.releaseLock();
    }
    if ( type == SSAC::GETFORWRITE )
	dirline=finddirline(theid);
 again1:
    if ( type == SSAC::GETFORWRITE )
	dirline->lock.acquireLock();
    hashq->lock.acquireLock();
    ep=hashq->search(theid,_associativity);
    if ( ep )
    {
	// hit
	trace( MISC, TR_INFO,tr_printf(">>> SSACSimpleReplicatedArray::get: Hit: id=%d index=%d ep=%llx:\n",
				       theid.id(), theid.index(_numhashqs),ep );ep->print());
	if ( ep->flags & CacheEntrySimple::BUSY )
	{
	    if ( type == SSAC::GETFORWRITE )
		dirline->lock.releaseLock();
	    hashq->lock.releaseLock();
	    ep->sleep();
	    goto again1;
	}
	if ( type == SSAC::GETFORWRITE )
	{
	    direntry=dirline->lookupid(theid,_associativity);
	    setbusyonallcopiesindir(direntry,ep);
	}
	hashq->count++;
	if (!hashq->count) hashq->rollover();
	ep->lastused=hashq->count;
	hashq->lock.releaseLock();
	if ( type == SSAC::GETFORWRITE )
	    dirline->lock.releaseLock();
	ce=ep;
	SET_CLSCD(rtn,1);
	return rtn;
    }
    else
    { // Missed Locally
	trace( MISC, TR_INFO,tr_printf(">>> SSACSimpleReplicatedArray::get: Missed Locally: id=%d index=%d\n",
				       theid.id(),theid.index(_numhashqs)));
	if ( type == SSAC::GETFORREAD )
	{
	    hashq->lock.releaseLock();	
	    dirline=finddirline(theid);
	again2:	
	    dirline->lock.acquireLock();
	    hashq->lock.acquireLock();
	    ep=hashq->search(theid,_associativity);
	    if (ep)
	    {
		// found a matching entry second time around
		if (ep->flags & CacheEntrySimple::BUSY )
		{
		    hashq->lock.releaseLock();
		    dirline->lock.releaseLock();
		    ep->sleep();
		    goto again2;
		}
		// if an entry was and it is not busy then skip rest of miss processing
	    }
	}
 	if (!ep) // Still a Local Miss?
	{ // check for global miss;
	    direntry=dirline->lookupid(theid,_associativity);
	    if (direntry)
		if (direntry->busy())
		{
		    hashq->lock.releaseLock();
		    dirline->lock.releaseLock();
		    direntry->spin();
		    goto again2;	
		}
	    // get a free entry 	
	    ep=hashq->lruentry(_associativity);

	    if (ep) 
	    {
		if (ep->id.valid())
		{
		    if (removefromdir(ep->id, MYVP,
				      ep->flags & CacheEntrySimple::DIRTY))
		    {
			//clean the entry
			trace( MISC, TR_INFO,tr_printf(">>> SSACSimpleReplicatedArray::get:Miss: Cleaning entry:\n");
			       ep->print());
			ep->id.save(ep->data);
		    }
		}
		ep->flags = CacheEntrySimple::ZERO;
		ep->id=theid;
		if (direntry)
		{
		    CacheEntrySimple *copy=getremotecopyfromdir(direntry);
		    ep->data=copy->data;
		    direntry->addvp(MYVP);
		}
		else
		{
		    ep->data=ep->id.load();
		    dirline->adddirentry(theid,MYVP,_associativity);
		}
	    }
	    else  
	    {
		// no free elements take the simple way out for the moment
		// just pass back an error code
		trace( MISC, TR_INFO,tr_printf(">>> SSACSimpleReplicatedArray::get: Miss:*NOFREE ENTRIES*\n"));
		hashq->lock.releaseLock();
		dirline->lock.releaseLock();
		SET_CLSCD(rtn,0);
		return rtn;
	    }
	}
	if ( type == SSAC::GETFORWRITE )
	{
	    if (!direntry) direntry=dirline->lookupid(theid,_associativity);
	    setbusyonallcopiesindir(direntry,ep);
	}
	hashq->count++;
	if (!hashq->count) hashq->rollover();
	ep->lastused=hashq->count;
	hashq->lock.releaseLock();
	dirline->lock.releaseLock();
	ce=ep;
	SET_CLSCD(rtn,1);
	return rtn;
    }
}

TornStatus
SSACSimpleReplicatedArray :: putback( CacheEntry* &ce, const putflag &flag )
{

    register CacheEntrySimple *entry=(CacheEntrySimple *)ce;
    register struct HashQueues *hashq=&(_hashqs[entry->id.index(_numhashqs)]);
    register struct DirLine *dirline=0;
    register struct DirEntry *direntry=0;

    if (!entry) return 0;
    
    tassert((hashq->entries),
	 ass_printf("\n\nSSACSimpleReplicatedArray::putback: bad queue entry:\n");
	    entry->print());
    
    dirline=finddirline(entry->id);
    dirline->lock.acquireLock();
    hashq->lock.acquireLock();

    if (flag == KEEP)
    {
	trace( MISC, TR_INFO,
	       tr_printf(">>> SSACSimpleReplicatedArray::putback: KEEP entry:");
	       entry->print()
	    );
    	entry->lastused=hashq->count;
    }
    else
    {
	trace( MISC, TR_INFO,
	     tr_printf(">>> SSACSimpleReplicatedArray::putback: DISCARD entry:");
	       entry->print()
	    );
	entry->lastused=0;
    }
    
    direntry=dirline->lookupid(entry->id,_associativity);
    updateandclearbusyonallcopiesindir(direntry,entry);
    hashq->lock.releaseLock();
    dirline->lock.releaseLock();
    entry->wakeup();
    return 0;
}

TornStatus
SSACSimpleReplicatedArray :: flush()
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
		trace( MISC, TR_INFO,
		       tr_printf(">>> SSACSimpleReplicatedArray::flush: entry:");
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
	}
	hashq->lock.releaseLock();
    }
    return 0;
}

SSACSimpleReplicatedArray :: ~SSACSimpleReplicatedArray()
{
    trace( MISC, TR_INFO,
	  tr_printf("**** ~SSACSimpleReplicatedArray ref=%lx\n",_ref));
    if (_hashqs) delete[] _hashqs;
    if (_dirlines) delete[] _dirlines;
}
 
TornStatus
SSACSimpleReplicatedArray :: snapshot()
{
    register struct HashQueues *hashq;
    register int i, j, flags;
    
    for (i=0; i<_numhashqs; i++)
    {
	hashq = &(_hashqs[i]);
	hashq->lock.acquireLock();
	tr_printf("_hashq[%d].count=%U\n_hashqs[%d].entry: ",i,hashq->count,i);
	if (hashq->entries)
	    for ( j=0; j<_associativity; j++)
	    {
		tr_printf("%d:%d:%U:%lx:",j,
			  hashq->entries[j].id.id(),
			  hashq->entries[j].lastused,
			  hashq->entries[j].data);
		flags=(hashq->entries[j]).flags;
		if (flags & CacheEntrySimple::BUSY)
		    tr_printf("B|");
		else printf("F|");
		if (flags & CacheEntrySimple::DIRTY)
		    tr_printf("D ");
		else tr_printf("C ");
	    }
	else
	    tr_printf("NULL");
	tr_printf("\n");
	hashq->lock.releaseLock();
    }
    if (MYVP < _numhashqs % NUMPROC)
    {
	for(i=0;i<_numhashqs/NUMPROC +1;i++)
	{
	    tr_printf("_dirlines[%d]: ",i);
	    for (j=0; j<_associativity*NUMPROC; j++)
	    {
		tr_printf("%d:0x%llx:",_dirlines[i].direntries[j].id.id(),
			  _dirlines[i].direntries[j].vpmask);
		if (_dirlines[i].direntries[j].flags &
		    SSACSimpleReplicatedArray::DirEntry::BUSY)
		    tr_printf("B|");
		else
		    tr_printf("F|");
		if (_dirlines[i].direntries[j].flags &
		    SSACSimpleReplicatedArray::DirEntry::MODIFIED)
		    tr_printf("M ");
		else
		    tr_printf("C ");
	    }
	    tr_printf("\n");
	}
    }
    else
    {
	for(i=0;i<_numhashqs/NUMPROC;i++)
	{
	    tr_printf("_dirlines[%d]: ",i);
	    for (j=0; j<_associativity*NUMPROC; j++)
	    {
		tr_printf("%d:0x%llx:",_dirlines[i].direntries[j].id.id(),
			  _dirlines[i].direntries[j].vpmask);
		if (_dirlines[i].direntries[j].flags &
		    SSACSimpleReplicatedArray::DirEntry::BUSY)
		    tr_printf("B|");
		else
		    tr_printf("F|");
		if (_dirlines[i].direntries[j].flags &
		    SSACSimpleReplicatedArray::DirEntry::MODIFIED)
		    tr_printf("M ");
		else
		    tr_printf("C ");
	    }
	    tr_printf("\n");
	}
    }
    return 0;
}
 
 
 
