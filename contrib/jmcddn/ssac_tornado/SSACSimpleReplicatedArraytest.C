#include <unistd.h>
#include <stdlib.h>
#include <tornado/sys/WellKnown.H>
#include <tornado/sys/kernelcalls.h>
#include <tornado/TAssert.H>
#include "objtrans/ot.H"
#include "misc/ObjectRefs.H"
#include "misc/misc.h"
#include "misc/Timer.H"
#include "objtrans/GOBJ.H"
#include <StubXAppl.H>
#include "misc/exception.H"
#include "lock/Barrier.H"
#include "misc/KernelObj.H"
#include "SSACSimpleReplicatedArray.H"
 
// Test Parameters
double FRACTIONRANDOM=1.0,
       FRACTIONWRITE=0;
  
int NUMFREEENTRIES=3,
    HASHTABLESIZE=128,
    WORKERS=NUMPROC,
    ASSOCIATIVITY=4,
    NUMREQUESTS=4096,
    PERPROC=0,
    MAXRANDOM=0x7fffffff,
    READPROCESSINGSPIN=1,
    WRITEPROCESSINGSPIN=1,
//    MAXRANGE=0x1ff,
    MAXRANGE=333,
    MINRANGE=0;
   
 
// Configure Test behaviour
#undef  INTERACTIVE
#undef  PRINTCACHEATEND 
#define  PRELOADTRANSLATIONENTRYFOREACHWORKER
#undef  DELAYSECONDARYWORKERS    
#define  PRELOADCACHE 
#undef  DIRTYPRELOAD 

#define FLUSHHARDWARECACHES 
#define MEASURETIMEPERWORKER 

void
printparms()
{
    tr_printf("**** PARMS: FRACTIONRANDOM=%lf\n",FRACTIONRANDOM);
    tr_printf("**** PARMS: FRACTIONWRITE=%lf\n",FRACTIONWRITE);
    tr_printf("**** PARMS: WORKERS=%d\n",WORKERS);
    tr_printf("**** PARMS: NUMFREEENTRIES=%d\n",NUMFREEENTRIES);
    tr_printf("**** PARMS: HASHTABLESIZE=%d\n",HASHTABLESIZE);
    tr_printf("**** PARMS: ASSOCIATIVITY=%d\n",ASSOCIATIVITY);
    tr_printf("**** PARMS: NUMREQUESTS=%d\n",NUMREQUESTS);
    tr_printf("**** PARMS: PERPROC=%d\n",PERPROC);
    tr_printf("**** PARMS: READPROCESSINGSPIN=%d\n",READPROCESSINGSPIN);
    tr_printf("**** PARMS: WRITEPROCESSINGSPIN=%d\n",WRITEPROCESSINGSPIN);
    tr_printf("**** PARMS: MAXRANGE=%d\n",MAXRANGE);
    tr_printf("**** PARMS: MINRANGE=%d\n",MINRANGE);
}

void
setparm(char *token, char *value)
{
//    tr_printf("token=%s value=%s\n",token,value);

    
    if (strcmp(token,"FRACTIONRANDOM")==0)
    {
	FRACTIONRANDOM=atof(value);
	return;
    }
    if (strcmp(token,"FRACTIONWRITE")==0)
    {
	FRACTIONWRITE=atof(value);
	return;
    }
    if (strcmp(token,"NUMFREEENTRIES")==0)
    {
	NUMFREEENTRIES=atoi(value);
	return;
    }
    if (strcmp(token,"HASHTABLESIZE")==0)
    {
	HASHTABLESIZE=atoi(value);
	return;
    }
    if (strcmp(token,"ASSOCIATIVITY")==0)
    {
	ASSOCIATIVITY=atoi(value);
	return;
    }
    if (strcmp(token,"WORKERS")==0)
    {
	WORKERS=atoi(value);
	return;
    }
    if (strcmp(token,"NUMREQUESTS")==0)
    {
	NUMREQUESTS=atoi(value);
	return;
    }
    if (strcmp(token,"PERPROC")==0)
    {
	PERPROC=atoi(value);
	return;
    }
    if (strcmp(token,"READPROCESSINGSPIN")==0)
    {
	READPROCESSINGSPIN=atoi(value);
	return;
    }
    if (strcmp(token,"WRITEPROCESSINGSPIN")==0)
    {
	WRITEPROCESSINGSPIN=atoi(value);
	return;
    }
    if (strcmp(token,"MINRANGE")==0)
    {
	MINRANGE=atoi(value);
	return;
    }
    if (strcmp(token,"MAXRANGE")==0)
    {
	MAXRANGE=atoi(value);
	return;
    }
    tr_printf("**** ERROR: Unknown token=%s value=%s\n",token,value);
}

void
setparms()
{
    register char *data,*token,*value,*src,*dst,*end;

    data=new char[KERNEL_PARMS_ARGS_NUMOF_BYTES];

    WORKERS=NUMPROC;
    
    token=value=dst=data;
    src=(char *)KParms.args;
    end=src+KERNEL_PARMS_ARGS_NUMOF_BYTES;
    
    while (src<end)
    {
	while (src<end && *src!='=') {
	    *dst=*src;
	    src++;
	    dst++;
	}
	if (src==end) break;
	*dst=(char)0;
	dst++;
	value=dst;
        src++;

	while (src<end && *src!='\n') {
	    *dst=*src;
	    src++;
	    dst++;
	}
	if (src==end) break;
	*dst=(char)0;
	setparm(token,value);
	src++;
	dst++;
	token=dst;
    }
    delete[] data;
}

#ifdef INTERACTIVE
enum {MAXINSTRING=80};
SpinBarrier *mybar;

void
perprocinteractivetest(SSACRef ssac)
{
    struct inusenode {
	CacheEntrySimple *entry;
	struct inusenode *next;
    } *inuse=0, *lastinuse=0;
    
    CacheEntrySimple *entry=0;
    CacheObjectIdSimple id;
    long orgTraceMask=TraceMask, orgTraceLevel=TraceLevel;
    char dirty=0,trace=0,write=0;
    register TornStatus status=0;
    register int an;
    char instring[MAXINSTRING];
    char *c=instring;
    *c=0;
    IncActiveCalls();
    
        // Preload the cache so that each bucket has only NUMFREEENTRIES
#ifdef PRELOADCACHE
    for (register int i=0;
	 i<( HASHTABLESIZE * ( ASSOCIATIVITY - NUMFREEENTRIES ) );
	 i++)
    {
	id=i;
	status=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
			       SSAC::GETFORREAD);
	tassert((TCLSCD(status)),
		ass_printf("\n**** ERROR: got error from get during preloading of cache\n"));
#ifdef DIRTYPRELOAD
	entry->dirty(); 
#endif
    }
#endif
    
    while (1) {
	tr_printf(
	    "(e)xit (t)trace (s)napshot (i)nuse (p)utback (w)rite (d)irty [0-9,\\n] (%s):",
	    instring);
	while( (an=PeekChar()) == 0);
	tr_printf("\n");
	if ( an == 'e' ) break;
	if ( an == 't' )
	{
	    trace = ~trace;
	    if (trace)
	    {
		TraceMask |= LIBS | MISC;
		if (TraceLevel < TR_INFO) TraceLevel=TR_INFO;
		tr_printf("Tracing: ON\n");
	    }
	    else
	    {
		TraceMask = orgTraceMask;
		TraceLevel = orgTraceLevel;
		tr_printf("Tracing: OFF: set back to original level and mask\n");
	    }
	    continue;
	}
	if ( an == 's' ) 
	{
	    DREF(ssac)->snapshot();
	    continue;
	}
	if ( an == 'd' )
	{
	    dirty = ~dirty;
	    if (dirty) tr_printf("dirty: ON: Will dirty entries on putback\n");
	    else tr_printf("dirty: OFF: will NOT dirty entries on putback\n");
	    continue;
	}
	if ( an == 'w' )
	{
	    write = ~write;
	    if (write) tr_printf("write: ON: get entries for write\n");
	    else tr_printf("write: OFF: will get entries for read\n");
	    continue;
	}
	if ( an == 'i' )
	{
	    if (inuse)
		for (register inusenode *tmp=inuse;
		     tmp!=0; tmp=tmp->next)
		    tr_printf("\tinuse: id=%d data=%lx\n",
			      tmp->entry->id.id(),tmp->entry->data);
	    else
		tr_printf("\tinuse list is empty\n");
	    continue;
	}
	if ( an == 'p' )
	{
	    if (inuse)
	    {
		lastinuse=inuse->next;
		while(inuse!=0)
		{
		    tr_printf("\tputting back id=%d data=%lx\n",
			      inuse->entry->id.id(),inuse->entry->data);
		    if (dirty) inuse->entry->dirty();
		    DREF(ssac)->putback((CacheEntry * &)inuse->entry,SSAC::KEEP);
		    delete inuse;
		    inuse=lastinuse;
		    if (lastinuse) lastinuse=lastinuse->next;
		}
		inuse=0; lastinuse=0;
	    }
	    else 
		tr_printf("\tinuse list is empty\n");
	    continue;
	}
	if ( an == '\n' )
	{
	    if ( c != instring )
	    {
		register int tmp=0,fac=1;
		for(c--;c>=instring;c--,fac*=10)
		    tmp+=fac*(*c - '0');		
		c=instring; *c=0;
		id=tmp;
		tr_printf("id.id()=%d\n",id.id());

		if ( write )
		    status=DREF(ssac)->get((CacheObjectId &)id,
					   (CacheEntry * &)entry,
					   SSAC::GETFORWRITE);
		else
		    status=DREF(ssac)->get((CacheObjectId &)id,
					   (CacheEntry * &)entry,
					   SSAC::GETFORREAD);
		    
		if ( TCLSCD(status) )
		{
		    tr_printf("entry->id.id()=%d:\n",entry->id.id());
		    entry->print();
		    if (write)
		    {
			register inusenode *tmp=new inusenode;
			tmp->entry=entry;
			tmp->next=0;
			if (inuse) 
			    lastinuse->next=tmp;
			else 
			    inuse=tmp;
			lastinuse=tmp;
		    }
		}
		else
		    tr_printf("\t** get failed **\n");
	    }
	    continue;
	}
	*c=an;
	c++;
	tassert( (c <= instring+MAXINSTRING),
		 tr_printf("OVERFLOWED MAXINSTRING\n") );
	*c=0;
    }
    DecActiveCalls();
    mybar->enter();
    return;
}
#else

struct request {
    int id;
    enum typeofreqeust {READ,WRITE} type;
};

SpinBarrier *endworkerbar;
SpinBarrier *startworkerbar;

int
dosomethingwithentry(SSACRef ssac, CacheEntrySimple *entry,int action)
{ 
//    tr_printf("entry->id.id()=%d:\n",entry->id.id());
//    entry->print();
    register j;
    if (action==request::WRITE)
    {
	if (entry->data == (void *)long(entry->id.id()))
	{
	    entry->data=0;
	}
	for (register i=0;i<WRITEPROCESSINGSPIN;i++)
	{
	    entry->data=(void *)(long(entry->data) + 1);
	}
	j=long(entry->data);
	entry->dirty();
	DREF(ssac)->putback((CacheEntry * &)entry,SSAC::KEEP);
    }
    else
    {
	for (register i=0;i<READPROCESSINGSPIN;i++)
	{
	    j+=long(entry->data);
	}
    }
    SpinDelayNS((unsigned long)5000UL);
    return j;
}

void
worker(SSACRef ssac, request *requests, int numids)
{
    register CacheEntrySimple *entry=0;
    register TornStatus status;
    CacheObjectIdSimple id;

    for (register int i=0;i<numids;i++)
    {
	id=requests[i].id;
	tr_printf("i=%d id=%d\n",i,id.id());
    again:
	if  (requests[i].type==request::WRITE)
	{
	    status=DREF(ssac)->get((CacheObjectId &)id,
				   (CacheEntry * &)entry,
				   SSAC::GETFORWRITE);
	}
	else
	{
	    status=DREF(ssac)->get((CacheObjectId &)id,
				   (CacheEntry * &)entry,
 				   SSAC::GETFORREAD);
	}
	if (TCLSCD(status))
	{
	    dosomethingwithentry(ssac, entry, requests[i].type);
	}
	else
	{
	    for (register int j=0;j<1000;j++);
	    goto again;
	}
    }
}
  
 
void
startworker(SSACRef ssac, request *requests, int numids)
{
    //setup
    // create array of ids in processor local memory
    register int i;
#ifdef MEASURETIMEPERWORKER
    unsigned long time=0;
#endif    
    request *localreqs=new request[numids];

//    TraceMask |= LIBS | MISC;
//    if (TraceLevel < TR_INFO) TraceLevel=TR_INFO;

    // copy ids from global array into local memory
    for (i=0;i<numids;i++)
    {
	localreqs[i]=requests[i];
    }
    
#ifdef PRELOADTRANSLATIONENTRYFOREACHWORKER
    // We do an inital access to the object to force
    // the translation entry for the co on the this processor
    // to be loaded.
    if (MYVP==0) {
//	tr_printf("**** DEBUG: Worker_%d_%#llx: Preload Translation Entry\n",MYVP,GLocal->activePDA);
	CacheObjectIdSimple id(0);
	CacheEntrySimple *entry=0;

	TornStatus status=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
			       SSAC::GETFORREAD);
	tassert((TCLSCD(status)),
		ass_printf("\n**** ERROR: got error from get during initial access\n"));
    }
    startworkerbar->enter();
    if (MYVP!=0) {
//	tr_printf("**** DEBUG: Worker_%d_%#llx: Preload Translation Entry\n",MYVP,GLocal->activePDA);
	CacheObjectIdSimple id(0);
	CacheEntrySimple *entry=0;

	TornStatus status=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
			       SSAC::GETFORREAD);
	tassert((TCLSCD(status)),
		ass_printf("\n**** ERROR: got error from get during initial access\n"));
    }
//    tr_printf("**** DEBUG: Worker_%d_%#llx: Translation Entry Preloaded (Rep Created)\n",MYVP,GLocal->activePDA);
    startworkerbar->enter();
#endif
        // Preload the cache so that each bucket has only NUMFREEENTRIES
#ifdef PRELOADCACHE
    register TornStatus status;
//    tr_printf("**** DEBUG: Worker_%d_%#llx: initialize entrys\n",MYVP,GLocal->activePDA);
    for (i=0;
	 i<( HASHTABLESIZE * ( ASSOCIATIVITY - NUMFREEENTRIES ) );
	 i++)
    {
	CacheObjectIdSimple id(0);
	CacheEntrySimple *entry=0;	
	id=i;
	status=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
			       SSAC::GETFORREAD);
	tassert((TCLSCD(status)),
		ass_printf("\n**** ERROR: got error from get during preloading of cache\n"));
#ifdef DIRTYPRELOAD
	entry->dirty(); 
#endif
    }
//    tr_printf("**** DEBUG: Worker_%d_%#llx: after initialization\n",MYVP,GLocal->activePDA);
#endif
    //starting gate (wait for all to be ready)
#ifdef FLUSHHARDWARECACHES
    FlushAllPCache();
    FlushAllSCache();
#endif
    startworkerbar->enter();
#ifdef DELAYSECONDARYWORKERS    
    if (MYVP != 0) {
	register int i=0;
	while (i<50000) i++;
    }
#endif    
#ifdef MEASURETIMEPERWORKER
    time=GetTicks();
#endif
    //run test
//    tr_printf("**** DEBUG: Worker_%d_%#llx: ABOUT TO RUN\n",MYVP,GLocal->activePDA);
    worker(ssac,localreqs,numids);
    
#ifdef MEASURETIMEPERWORKER
    time=GetTicks()-time;
    tr_printf("**** STATS: Worker_%d_%#llx: Total StarttoEnd=%lld\n",MYVP,
	      GLocal->activePDA,2*time);
#endif 
    delete[] localreqs;
    
    //all done (wait for all to finish)
    endworkerbar->enter();
}

int
mkid()
{
    double factor=(double)random()/(double)MAXRANDOM;
    int rtn= ( factor*(double)(MAXRANGE-MINRANGE)) + MINRANGE;
    return rtn;
}
    
int
pickid(const int &hotspot)
{
    if (FRACTIONRANDOM == 1) return mkid();
    if (FRACTIONRANDOM == 0) return hotspot;
    if ( ((double)random()/(double)MAXRANDOM) <= FRACTIONRANDOM )
    {
	return mkid();
    }
    else 
    {
	return hotspot;
    }
}

request::typeofreqeust
picktype()
{
    if (FRACTIONWRITE == 1) return request::WRITE;
    if (FRACTIONWRITE == 0) return request::READ;
    if ( ((double)random()/(double)MAXRANDOM) <= FRACTIONWRITE )
    {
	return request::WRITE;
    }
    else
    {
	return request::READ;
    }
}

#endif


void
driver()
{
    SSACRef ssac = SSACSimpleReplicatedArray::create(HASHTABLESIZE);
    StubXAppl appl(myAppl->getOH());
    CacheObjectIdSimple id;
    register int i,vp;
    
    IncActiveCalls();
     
#ifdef INTERACTIVE
    mybar=new SpinBarrier(NUMPROC); 
    GlobalUpdate((void **)&mybar);

    // start a perprocessor test on all processors
    for (vp=0;vp<NUMPROC;vp++) {
	// run tests asynchronously all processors skipping this processor
	if (vp!=MYVP) {
	    appl.setVP(vp);
	    appl.createProcess1( (tstatusfunc) perprocinteractivetest,
				 (reg_t) ssac);
	}
    }
    // run test on this processor and wait for it to complete before going on.
    // since all tests use a barrier this test will only return when all tests 
    // on all processors have completed
    appl.setVP(MYVP);
    appl.createProcess1AndWait( (tstatusfunc) perprocinteractivetest, (reg_t) ssac);
    delete mybar;
#else
    // note startworkerbar is never deleted explicity
    tassert(WORKERS<=NUMPROC, ass_printf("**** ERROR:  More workers than number of processors\n"));
    startworkerbar=new SpinBarrier(WORKERS);
    GlobalUpdate((void **)&startworkerbar);
    endworkerbar=new SpinBarrier(WORKERS);
    GlobalUpdate((void **)&endworkerbar);
    int wcount=0, hcount=0;
    
    // create random ids for each processor
    // again note ids is never deleted explicity
    // pick a random hotspot;
    int hotspot=mkid();
    int numreqsperproc=NUMREQUESTS/WORKERS;
    
    if (PERPROC)
	numreqsperproc=NUMREQUESTS;
    
    request *requests=new request[numreqsperproc*WORKERS];

    for (i=0;
	 i<numreqsperproc*WORKERS;
	 i++)
    {
	requests[i].id=pickid(hotspot);
	requests[i].type=picktype();
//	tr_printf("*** request[%d].id=%d\n",i,requests[i].id);
	if ( requests[i].id == hotspot )
	    hcount++;
	if (requests[i].type == request::WRITE)
	    wcount++;
    }
    tr_printf("**** DEBUG: number of requests per processor %d\n", numreqsperproc);
    tr_printf("**** DEBUG: number of actual write requests are %d\n",
	      wcount);
    tr_printf("**** DEBUG: hotspot=%d(0x%lx) number of actual hot requests are %d\n",
	      hotspot,hotspot,hcount);
    // start a perprocessor test on all processors
    for (vp=0;vp<WORKERS;vp++) {
	// run tests asynchronously all processors except one
	if (vp!=MYVP)
	{
	    appl.setVP(vp);
	    appl.createProcess( (tstatusfunc) startworker, (reg_t) ssac,
				(reg_t)&requests[vp*numreqsperproc],
				(reg_t)numreqsperproc,
				(reg_t)0, (reg_t)0, (reg_t)0 );    
	}
    }
    appl.setVP(MYVP);
    appl.createProcessAndWait( (tstatusfunc) startworker, (reg_t) ssac,
				(reg_t)&requests[MYVP*numreqsperproc],
			       (reg_t)numreqsperproc,
			       (reg_t)0, (reg_t)0, (reg_t)0 );
    delete[] requests;
    delete startworkerbar;
    delete endworkerbar;
#endif
#ifdef PRINTCACHEATEND
    tr_printf("** ALL WORKERS DONE CACHE SNAPSHOT:\n");
    DREF(ssac)->snapshot();
#endif
    DREF(ssac)->destroy();
    DecActiveCalls();
}
 
void
test()
{
    setparms();
    printparms();
#ifdef INTERACTIVE
    int an;
    while (1)
    {
	tr_printf("Press  key to start the test (e to exit)");
	while( (an= PeekChar()) == 0);
	tr_printf("\n");
	if ( an == 'e' ) break; 
	driver();
    }
#else
    tr_printf("Start of test....\n");
    driver();
    tr_printf("End of test.\n");
#endif
}


