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
#include "CounterRemoteCO.H"
                   
// Test Parameters
double FRACTIONDEC=0.5,
       FRACTIONREAD=0.01;

int WORKERS=NUMPROC,
    NUMREQUESTS=4096,
    PERPROC=0,
    MAXRANDOM=0x7fffffff;
 
// Configure Test behaviour

#define FLUSHHARDWARECACHES 
#define MEASURETIMEPERWORKER 
#define INITALLREPS

void
printparms()
{
    tr_printf("**** PARMS: FRACTIONREAD=%lf\n",FRACTIONREAD);
    tr_printf("**** PARMS: FRACTIONDEC=%lf\n",FRACTIONDEC);
    tr_printf("**** PARMS: WORKERS=%d\n",WORKERS);
    tr_printf("**** PARMS: NUMREQUESTS=%d\n",NUMREQUESTS);
    tr_printf("**** PARMS: PERPROC=%d\n",PERPROC);
}

void
setparm(char *token, char *value)
{
//    tr_printf("token=%s value=%s\n",token,value);

    
    if (strcmp(token,"FRACTIONDEC")==0)
    {
	FRACTIONDEC=atof(value);
	return;
    }
    if (strcmp(token,"FRACTIONREAD")==0)
    {
	FRACTIONREAD=atof(value);
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

struct request {
    enum typeofreqeust {INC,DEC,VAL} type;
};

#ifdef MEASURETIMEPERWORKER
struct result {
    void *pda;
    unsigned long time;
} *results;
#endif

SpinBarrier *endworkerbar;
SpinBarrier *startworkerbar;

int
dosomething(integerCounterRef counter, int action)
{ 
    SpinDelayNS((unsigned long)5000UL);
    return 1;
}

void
worker(integerCounterRef counter, request *requests, int numids)
{

    int val;

    for (register int i=0;i<numids;i++)
    {
//	tr_printf("i=%d id=%d\n",i,id.id());
	if  (requests[i].type==request::INC)
	{
	    DREF(counter)->increment();
	    dosomething(counter, requests[i].type);
	    continue;
	}
	if  (requests[i].type==request::DEC)
	{
	    DREF(counter)->decrement();
	    dosomething(counter, requests[i].type);
	    continue;
	}
	if  (requests[i].type==request::VAL)
	{
	    DREF(counter)->value(val);
	    dosomething(counter, requests[i].type);
	}
    }
}
  

void
startworker(integerCounterRef counter, request *requests, int numids)
{
    //setup
    // create array of ids in processor local memory
#ifdef MEASURETIMEPERWORKER
    unsigned long time=0;
#endif    
    request *localreqs=new request[numids];
     
    // copy ids from global array into local memory
    for (register int i=0;i<numids;i++)
    {
	localreqs[i]=requests[i];
    }
#ifdef INITALLREPS
    DREF((CounterRemoteCO **)counter)->increment();
    DREF((CounterRemoteCO **)counter)->decrement();
#endif
    //starting gate (wait for all to be ready)
//   tr_printf("Before flush startworkerbar=%llx\n",startworkerbar);
#ifdef FLUSHHARDWARECACHES
    FlushAllPCache();
    FlushAllSCache();
#endif
//    tr_printf("After flush startworkerbar=%llx\n",startworkerbar);
    startworkerbar->enter();
//    tr_printf("**** ***** HERE I AM %d\n",MYVP);
#ifdef MEASURETIMEPERWORKER
    time=GetTicks();
#endif
    //run test
    worker(counter,localreqs,numids);
    
#ifdef MEASURETIMEPERWORKER
    time=GetTicks()-time;
    results[MYVP].pda=(void *)GLocal->activePDA;
    results[MYVP].time=2*time;
//    tr_printf("**** STATS: Worker_%d_%#llx: Total StarttoEnd=%lld\n",MYVP,
//	      GLocal->activePDA,2*time);
#endif 
    delete[] localreqs;
    
    //all done (wait for all to finish)
    endworkerbar->enter();
}


request::typeofreqeust
picktype()
{
    if (FRACTIONREAD == 1) return request::VAL;
    if ( ((double)random()/(double)MAXRANDOM) <= FRACTIONREAD )
    {
	return request::VAL;
    }
    else
    {
	if ( ((double)random()/(double)MAXRANDOM) <= FRACTIONDEC )
	    return request::DEC;
	return request::INC;
    }
}

  
 
void
driver()
{
    integerCounterRef counter = CounterRemoteCO::create();
    StubXAppl appl(myAppl->getOH());
    int i=0,vp=0,dcount=0,icount=0,vcount=0;
    TornStatus status;
    
    IncActiveCalls();

    // note startworkerbar is never deleted explicity
    tassert(WORKERS<=NUMPROC, ass_printf("**** ERROR:  More workers than number of processors\n"));
    
#ifdef MEASURETIMEPERWORKER
    results=new result[WORKERS];
    GlobalUpdate((void **)&results);
#endif
    startworkerbar=new SpinBarrier(WORKERS);
    GlobalUpdate((void **)&startworkerbar);
    endworkerbar=new SpinBarrier(WORKERS);
    GlobalUpdate((void **)&endworkerbar);
    int wcount=0, hcount=0;
    
    int numreqsperproc=NUMREQUESTS/WORKERS;
    
    if (PERPROC)
	numreqsperproc=NUMREQUESTS;
    
    request *requests=new request[numreqsperproc*WORKERS];

    for (i=0;
	 i<numreqsperproc*WORKERS;
	 i++)
    {
	requests[i].type=picktype();
//	tr_printf("*** request[%d].id=%d\n",i,requests[i].id);
	if (requests[i].type == request::DEC)
	    dcount++;
	if (requests[i].type == request::INC)
	    icount++;
	if (requests[i].type == request::VAL)
	    vcount++;
    }
    tr_printf("**** DEBUG: number of requests per processor %d\n", numreqsperproc);
    tr_printf("**** DEBUG: number of actual inc requests are %d\n",
	      icount);
    tr_printf("**** DEBUG: number of actual dec requests are %d\n",
	      dcount);
    tr_printf("**** DEBUG: number of actual val requests are %d\n",
	      vcount);
    // start a perprocessor test on all processors
    for (vp=0;vp<WORKERS;vp++) {
	// run tests asynchronously all processors except one
	if (vp!=MYVP)
	{
	    appl.setVP(vp);
	    appl.createProcess( (tstatusfunc) startworker, (reg_t) counter,
				(reg_t)&requests[vp*numreqsperproc],
				(reg_t)numreqsperproc,
				(reg_t)0, (reg_t)0, (reg_t)0 );    
	}
    }
    appl.setVP(MYVP);
    appl.createProcessAndWait( (tstatusfunc) startworker, (reg_t) counter,
				(reg_t)&requests[MYVP*numreqsperproc],
			       (reg_t)numreqsperproc,
			       (reg_t)0, (reg_t)0, (reg_t)0 );

    {
	int val=0;
	DREF(counter)->value(val);
	tr_printf("**** DEBUG: Final value of counter is: %d\n",val);
    }
#ifdef MEASURETIMEPERWORKER
    for (i=0;i<WORKERS;i++)
    {
	tr_printf("**** STATS: Worker_%d_%#llx: Total StarttoEnd=%lld\n",i,
		  results[i].pda,results[i].time);
    }
    delete[] results;
#endif
    DREF(counter)->destroy();
    delete[] requests;
    delete startworkerbar;
    delete endworkerbar;
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


