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
#include "SSACSimpleSharedArray.H"
#include "SSACSimpleReplicatedArray.H"
#include "SSACSimplePartitionedArray.H"

// Test Parameters
double FRACTIONHOT=0.10,
       FRACTIONWRITE=0.20,
       FRACTIONLOCAL=0.80,
       FRACTIONLOCALMISS=0.10,
       FRACTIONREMOTEMISS=0.10;


int NUMFREEENTRIES=3,
    HASHTABLESIZE=128,
    WORKERS=NUMPROC,
    NUMQSPERREP=HASHTABLESIZE/WORKERS,
    ASSOCIATIVITY=4,
    NUMREQUESTS=4096,
    NUMREQSPERWORKER=NUMREQUESTS/WORKERS,
    MAXRANDOM=0x7fffffff,
    READPROCESSINGSPIN=1,
    WRITEPROCESSINGSPIN=1,
    MAXRANGE=HASHTABLESIZE,
    MINRANGE=0,
    FORCEREMOTE=1,
    INITLOCALONLY=1,
    TEST=0;

unsigned long PERREQDELAY=25000UL;

// Configure Test behaviour
#define PRINTCACHEATEND 
#define PRELOADTRANSLATIONENTRYFOREACHWORKER
#define PRELOADCACHE 
#undef  DIRTYPRELOAD 
#define FLUSHHARDWARECACHES 
#define MEASURETIMEPERWORKER 
#define CHECKFORWRITEHITS

void
printparms()
{
    tr_printf("**** PARMS: FRACTIONHOT=%lf\n",FRACTIONHOT);
    tr_printf("**** PARMS: FRACTIONWRITE=%lf\n",FRACTIONWRITE);
    tr_printf("**** PARMS: FRACTIONLOCAL=%lf\n",FRACTIONLOCAL);
    tr_printf("**** PARMS: FRACTIONLOCALMISS=%lf\n",FRACTIONLOCALMISS);
    tr_printf("**** PARMS: FRACTIONREMOTEMISS=%lf\n",FRACTIONREMOTEMISS);
    tr_printf("**** PARMS: WORKERS=%d\n",WORKERS);
    tr_printf("**** PARMS: NUMFREEENTRIES=%d\n",NUMFREEENTRIES);
    tr_printf("**** PARMS: HASHTABLESIZE=%d\n",HASHTABLESIZE);
    tr_printf("**** PARMS: NUMQSPERREP=%d\n",NUMQSPERREP);
    tr_printf("**** PARMS: ASSOCIATIVITY=%d\n",ASSOCIATIVITY);
    tr_printf("**** PARMS: NUMREQUESTS=%d\n",NUMREQUESTS);
    tr_printf("**** PARMS: NUMREQSPERWORKER=%d\n",NUMREQSPERWORKER);
    tr_printf("**** PARMS: READPROCESSINGSPIN=%d\n",READPROCESSINGSPIN);
    tr_printf("**** PARMS: WRITEPROCESSINGSPIN=%d\n",WRITEPROCESSINGSPIN);
    tr_printf("**** PARMS: MAXRANGE=%d\n",MAXRANGE);
    tr_printf("**** PARMS: MINRANGE=%d\n",MINRANGE);
    tr_printf("**** PARMS: PERREQDELAY=%d\n",PERREQDELAY);
    tr_printf("**** PARMS: FORCEREMOTE=%d\n",FORCEREMOTE);
    tr_printf("**** PARMS: INITLOCALONLY=%d\n",INITLOCALONLY);
    tr_printf("**** PARMS: TEST=%d\n",TEST);
}

void
setparm(char *token, char *value)
{
//    tr_printf("token=%s value=%s\n",token,value);

    
    if (strcmp(token,"FRACTIONHOT")==0)
    {
	FRACTIONHOT=atof(value);
	GlobalUpdate((void **)&FRACTIONHOT);
	return;
    }
    if (strcmp(token,"FRACTIONWRITE")==0)
    {
	FRACTIONWRITE=atof(value);
	GlobalUpdate((void **)&FRACTIONWRITE);
	return;
    }
    if (strcmp(token,"FRACTIONLOCAL")==0)
    {
	FRACTIONLOCAL=atof(value);
	GlobalUpdate((void **)&FRACTIONLOCAL);
	return;
    }
    if (strcmp(token,"FRACTIONLOCALMISS")==0)
    {
	FRACTIONLOCALMISS=atof(value);
	GlobalUpdate((void **)&FRACTIONLOCALMISS);
	return;
    }
    if (strcmp(token,"FRACTIONREMOTEMISS")==0)
    {
	FRACTIONREMOTEMISS=atof(value);
	GlobalUpdate((void **)&FRACTIONREMOTEMISS);
	return;
    }
    if (strcmp(token,"NUMFREEENTRIES")==0)
    {
	NUMFREEENTRIES=atoi(value);
	GlobalUpdate((void **)&NUMFREEENTRIES);
	return;
    }
    if (strcmp(token,"HASHTABLESIZE")==0)
    {
	HASHTABLESIZE=atoi(value);
	GlobalUpdate((void **)&HASHTABLESIZE);
	return;
    }
    if (strcmp(token,"ASSOCIATIVITY")==0)
    {
	ASSOCIATIVITY=atoi(value);
	GlobalUpdate((void **)&ASSOCIATIVITY);
	return;
    }
    if (strcmp(token,"WORKERS")==0)
    {
	WORKERS=atoi(value);
	GlobalUpdate((void **)&WORKERS);
	return;
    }
    if (strcmp(token,"NUMREQUESTS")==0)
    {
	NUMREQUESTS=atoi(value);
	GlobalUpdate((void **)&NUMREQUESTS);
	return;
    }
    if (strcmp(token,"READPROCESSINGSPIN")==0)
    {
	READPROCESSINGSPIN=atoi(value);
	GlobalUpdate((void **)&READPROCESSINGSPIN);
	return;
    }
    if (strcmp(token,"WRITEPROCESSINGSPIN")==0)
    {
	WRITEPROCESSINGSPIN=atoi(value);
	GlobalUpdate((void **)&WRITEPROCESSINGSPIN);
	return;
    }
    if (strcmp(token,"MINRANGE")==0)
    {
	MINRANGE=atoi(value);
	GlobalUpdate((void **)&MINRANGE);
	return;
    }
    if (strcmp(token,"MAXRANGE")==0)
    {
	MAXRANGE=atoi(value);
	GlobalUpdate((void **)&MAXRANGE);
	return;
    }
    if (strcmp(token,"PERREQDELAY")==0)
    {
	PERREQDELAY=atoi(value);
	GlobalUpdate((void **)&PERREQDELAY);
	return;
    }
    if (strcmp(token,"FORCEREMOTE")==0)
    {
	FORCEREMOTE=atoi(value);
	GlobalUpdate((void **)&FORCEREMOTE);
	return;
    }
    if (strcmp(token,"INITLOCALONLY")==0)
    {
	INITLOCALONLY=atoi(value);
	GlobalUpdate((void **)&INITLOCALONLY);
	return;
    }
    if (strcmp(token,"TEST")==0)
    {
	TEST=atoi(value);
	GlobalUpdate((void **)&TEST);
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
    GlobalUpdate((void **)&WORKERS);

    
    GlobalUpdate((void **)&FRACTIONHOT);
    GlobalUpdate((void **)&FRACTIONWRITE);
    GlobalUpdate((void **)&FRACTIONLOCAL);
    GlobalUpdate((void **)&FRACTIONLOCALMISS);
    GlobalUpdate((void **)&FRACTIONREMOTEMISS);
    GlobalUpdate((void **)&NUMFREEENTRIES);
    GlobalUpdate((void **)&ASSOCIATIVITY);
    GlobalUpdate((void **)&NUMREQUESTS);
    GlobalUpdate((void **)&MAXRANDOM);
    GlobalUpdate((void **)&READPROCESSINGSPIN);
    GlobalUpdate((void **)&WRITEPROCESSINGSPIN);
    GlobalUpdate((void **)&PERREQDELAY);
    GlobalUpdate((void **)&FORCEREMOTE);
    GlobalUpdate((void **)&INITLOCALONLY);
    GlobalUpdate((void **)&TEST);

    GlobalUpdate((void **)&MINRANGE);
  
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
    
    MAXRANGE=HASHTABLESIZE-1;
    GlobalUpdate((void **)&MAXRANGE);

    NUMQSPERREP=HASHTABLESIZE/WORKERS;
    GlobalUpdate((void **)&NUMQSPERREP);
    NUMREQSPERWORKER=NUMREQUESTS/WORKERS;
    GlobalUpdate((void **)&NUMREQSPERWORKER);
}


struct request {
    int id;
    enum typeofreqeust {READ,WRITE} type;
};

SpinBarrier *endworkerbar;
SpinBarrier *startworkerbar;
volatile int *inittoken;

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
	    entry->data=(void *)(long(entry->data) + 1);
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
    SpinDelayNS((unsigned long)PERREQDELAY);
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
//	tr_printf("i=%d id=%d\n",i,id.id());
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
perworkerssacinit(SSACRef ssac)
{
    CacheObjectIdSimple id(0);
    CacheEntrySimple *entry=0;
    TornStatus status;
    int startid,endoffset;

    if (INITLOCALONLY){
	startid=(int)MYVP*NUMQSPERREP;
        endoffset=NUMQSPERREP;
    } else {
	startid=0;
	endoffset=HASHTABLESIZE;
    }
    
#ifdef PRELOADTRANSLATIONENTRYFOREACHWORKER
    // We do an inital access to the object to force
    // the translation entry for the co on the this processor
    // to be loaded.
    while (*inittoken!=MYVP);
    id=startid;
    status=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
			   SSAC::GETFORREAD);
    tassert((TCLSCD(status)),
	    ass_printf("\n**** ERROR: got error from get during initial access\n"));
    if (MYVP==(NUMPROC-1)) *inittoken=0;
    else *inittoken=MYVP+1;
#endif

#ifdef PRELOADCACHE
    // Preload the cache so that each bucket has only NUMFREEENTRIES
    while (*inittoken!=MYVP);
    for (int j=0; j<(ASSOCIATIVITY-NUMFREEENTRIES); j++)
	for (int i = ( startid + (HASHTABLESIZE * j));
	     i < ((startid + (HASHTABLESIZE * j)) + endoffset);
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
    *inittoken=MYVP+1;
#endif
    
}

void
startworker(SSACRef ssac, request *requests, int numids)
{
    //setup
    // create array of ids in processor local memory
#ifdef MEASURETIMEPERWORKER
    unsigned long time=0;
#endif    
    request *localreqs=new request[numids];

//    printparms();
    
    // copy ids from global array into local memory
    for (register int i=0;i<numids;i++)
    {
	localreqs[i]=requests[i];
    }
    perworkerssacinit(ssac);
    //starting gate (wait for all to be ready)

    startworkerbar->enter();

//    DREF(ssac)->snapshot();
    tr_printf("**** DEBUG: Worker_%d_%#llx: At the Starting Gate %llx\n",
	      MYVP,GLocal->activePDA,startworkerbar);
#ifdef FLUSHHARDWARECACHES
    FlushAllPCache();
    FlushAllSCache();
#endif
    
    startworkerbar->enter();
#ifdef MEASURETIMEPERWORKER
    time=GetTicks();
#endif
    //run test
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

int get_random_index(int pts)
{
  double delta = 0.4999;
  int less_pt;
  int val;

  less_pt = pts - 1;
  val = (int)-delta+(less_pt+2*delta)*((double)random()/(double)MAXRANDOM);
  return val;
}


int
mkid(const int &min,const int &max)
{
    
    int rtn=get_random_index(max-min+1);
    rtn+=min;
    return rtn;
}

void
mkmiss(int &id)
{
    double ranfactor=(double)random()/(double)MAXRANDOM;
    int mult= ( ranfactor*(double)(1000-1)) + 1;
    id+=mult*HASHTABLESIZE;
}
 
int
islocal(const int &id, const int &vp)
{
    int tvp=id/NUMQSPERREP;
//    tr_printf(">>>> id=%d NUMQSPERREP=%d tvp=%d tvp%%NUMQSPERREP=%d\n",id,NUMQSPERREP,tvp,tvp%NUMQSPERREP);
    return ( (vp == (tvp%WORKERS)) );
}

int
ismiss(const int &id,const int &vp)
{
    return (!(id >=0 && id <= MAXRANGE));
}

int
remotehitid(const int &vp)
{
    int rtn=mkid(0,MAXRANGE);
    if (FORCEREMOTE)
	while (islocal(rtn,vp)) rtn=mkid(0,MAXRANGE);
    return rtn;
}

int
pickhotspot()
{
  return mkid(0,MAXRANGE);
}

int
remotemissid(const int &vp)
{
    int rtn=remotehitid(vp);
    mkmiss(rtn);
    return rtn;
}

int
localhitid(const int &vp)
{
    int start=vp*NUMQSPERREP;
    return mkid(start,start+NUMQSPERREP-1);
}

int
localmissid(const int &vp)
{
    int rtn=localhitid(vp);
    mkmiss(rtn);
    return rtn;
}

int
randomid(const int &vp)
{
    int rtn=0;
    if ( (WORKERS>1) && (((double)random()/(double)MAXRANDOM) > FRACTIONLOCAL) )
    {
	if ( ((double)random()/(double)MAXRANDOM) > FRACTIONREMOTEMISS )
	    rtn=remotehitid(vp);
	else
	    rtn=remotemissid(vp);
    }
    else
    {
	if ( ((double)random()/(double)MAXRANDOM) > FRACTIONLOCALMISS )
	    rtn=localhitid(vp);
	else
	    rtn=localmissid(vp);
    }
    return rtn;
}

int
pickid(const int &hotspot, const int &vp)
{
    if (FRACTIONHOT == 1) return hotspot;
    if (FRACTIONHOT == 0) return randomid(vp);
    if ( ((double)random()/(double)MAXRANDOM) > FRACTIONHOT )
    {
	return randomid(vp);
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

SSACRef
makessac()
{
    SSACRef ssac=0;
    switch (TEST) {
    case 0:
	ssac=SSACSimpleSharedArray::create(HASHTABLESIZE);
	tr_printf("**** DEBUG: SSAC type: SSACSimpleSharedArray\n");
	break;
    case 1:
	ssac=SSACSimpleReplicatedArray::create(HASHTABLESIZE);
	tr_printf("**** DEBUG: SSAC type: SSACSimpleReplicatedArray\n");
	break;
    case 2:
	ssac=SSACSimplePartitionedArray::create(HASHTABLESIZE);
	tr_printf("**** DEBUG: SSAC type: SSACSimplePartitionedArray\n");
	break;
    default:
	tr_printf("**** ERROR makessac: TEST %d not define\n",TEST);
	tassert(0, ass_printf("\nOOPS TEST NOT DEFINE\n"));
    }
    return ssac;
}

request *
initrequests()
{
    int i,j,index;
    int wcount=0, hcount=0,lcount=0,lmisscount=0,rmisscount=0,lwcount=0,rwcount=0;
    // create random ids for each processor
    // again note ids is never deleted explicity
    // pick a random hotspot;
    int hotspot=pickhotspot();

    request *requests=new request[NUMREQSPERWORKER*WORKERS];
    
    for (i=0; i<WORKERS; i++)
    {
	for (j=0; j<NUMREQSPERWORKER; j++)
	{
	    index=i*NUMREQSPERWORKER+j;
	    requests[index].type=picktype();
	    requests[index].id=pickid(hotspot,i);
//	    tr_printf("*** request[%d].id=%d\n",index,requests[index].id);
	    if ( requests[index].id == hotspot )
		hcount++;
	    if (islocal(requests[index].id,i) )
	    {
		lcount++;
		if ( ismiss(requests[index].id,i) )
		    lmisscount++;
		if ( requests[index].type == request::WRITE )
		    lwcount++;
	    }
	    else
	    {
		if ( ismiss(requests[index].id,i) )
		    rmisscount++;
		if ( requests[index].type == request::WRITE )
		    rwcount++;		    
	    }
	    if (requests[index].type == request::WRITE)
		wcount++;
	}
    }
    tr_printf("**** DEBUG: number of requests per processor %d\n", NUMREQSPERWORKER);
    tr_printf("**** DEBUG: number of actual write requests are %d\n",
	      wcount);
    tr_printf("**** DEBUG: hotspot=%d(0x%lx) number of actual hot requests are %d\n",
	      hotspot,hotspot,hcount);
    tr_printf("**** DEBUG: number of actual local requests are %d\n",
	      lcount);
    tr_printf("**** DEBUG: number of actual local write requests are %d\n",
	      lwcount);
    tr_printf("**** DEBUG: number of actual local miss requests are %d\n",
	      lmisscount);
    tr_printf("**** DEBUG: number of actual remote write requests are %d\n",
	      rwcount);
    tr_printf("**** DEBUG: number of actual remote miss requests are %d\n",
	      rmisscount);
    return requests;
}

void
driver()
{
    SSACRef ssac = makessac();
    StubXAppl appl(myAppl->getOH());
    CacheObjectIdSimple id;
    register int vp;
    
    IncActiveCalls();
     
    // note startworkerbar is never deleted explicity
    tassert(WORKERS<=NUMPROC, ass_printf("**** ERROR:  More workers than number of processors\n"));

    startworkerbar=new SpinBarrier(WORKERS);
    GlobalUpdate((void **)&startworkerbar);
    endworkerbar=new SpinBarrier(WORKERS);
    GlobalUpdate((void **)&endworkerbar);
    inittoken=new int;
    *inittoken=0;
    GlobalUpdate((void **)&inittoken);
    
    
    request *requests=initrequests();
    
    // start a perprocessor test on all processors
    for (vp=0;vp<WORKERS;vp++) {
	// run tests asynchronously all processors distributing request appropriately
	if (vp!=MYVP)
	{
	    appl.setVP(vp);
	    appl.createProcess( (tstatusfunc) startworker, (reg_t) ssac,
				(reg_t)&requests[vp*NUMREQSPERWORKER],
				(reg_t)NUMREQSPERWORKER,
				(reg_t)0, (reg_t)0, (reg_t)0 );    
	}
    }
    appl.setVP(MYVP);
    appl.createProcessAndWait( (tstatusfunc) startworker, (reg_t) ssac,
				(reg_t)&requests[MYVP*NUMREQSPERWORKER],
			       (reg_t)NUMREQSPERWORKER,
			       (reg_t)0, (reg_t)0, (reg_t)0 );
    delete[] requests;
    delete startworkerbar;
    delete endworkerbar;
    delete inittoken;
#ifdef CHECKFORWRITEHITS
    {
	CacheObjectIdSimple id(0);
	CacheEntrySimple *entry=0;
	TornStatus status;
	int twrites=0;
	
	for (int i=0; i<HASHTABLESIZE; i++)
	{
	    id=i;
	    status=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
				   SSAC::GETFORREAD);
	    tassert((TCLSCD(status)),
		    ass_printf("\n**** ERROR: got error from get during preloading of cache\n"));
	    if (entry->data != (void *)long(i)) {
		// tr_printf("**** DEBUG i=%d entry->data=%llx (%d)\n",i,entry->data,long(entry->data));
		twrites+=((long(entry->data) - i - 1))/WRITEPROCESSINGSPIN;
	    }
	}
	tr_printf("**** DEBUG: Total write hits after run is: %d\n",twrites);
    }
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
    tr_printf("Start of test....\n");
    driver();
    tr_printf("End of test.\n");
}


