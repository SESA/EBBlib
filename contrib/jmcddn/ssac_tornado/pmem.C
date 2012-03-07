#include <stdlib.h> 
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <tornado/sys/WellKnown.H>
#include <tornado/Barrier.H>
#include <tornado/MallocArray.H>
#include <tornado/TAssert.H>
#include <StubXAppl.H>
#include <StubXProgram.H>
#include <StubXConsole.H>
#include <StubXCORZero.H>
#include <StubXRegionSimple.H>
#include <StubXRegionPhys.H>
#include "StubXmyRegionSimpleCustom.H"
#include <StubXPD.H>
#include <StubXMappedIO.H>

#define PAGE_SIZE	4096

extern "C" TornStatus LowlevelOpen( const char *pth, int fgs, int md,
			     ObjectHandle *fh, u_long *obj_type ) ;
  
extern "C" int NumProc( );

extern "C" { 
    void tstEvent_testinfo(int numWorkers, int numEventsPerWorker);
    void tstEvent_startworker();
    void tstEvent_endworker();
    void tstEvent_starttest();
    void tstEvent_endtest();
    void tstEvent_startdriver();
    void tstEvent_enddriver();
}


class teststruct {
public:
    unsigned long time1;
    unsigned long time2;
    unsigned long size;
    unsigned long iters;
    unsigned long test;
    unsigned long misc;
    void *        ptr;

    void init( unsigned long s, unsigned long i, unsigned long t,
	       unsigned long m, void *p ) {
	size = s; iters = i; test = t; misc = m; ptr = p; time1 = time2 = 0;
    }
};

typedef SpinOnlyBarrier BarrierType;


static void
DoConcTest( int numWorkers, tstatusfunc func, int startproc, teststruct *p )
{
    BarrierType *sbar = new BarrierType( numWorkers );

    StubXAppl appl(myAppl->getOH());

    // we always save startproc for last, synchronous call
    for( int i=1, proc=startproc+1; i < numWorkers; i++, proc++ ) {
	if( proc >= numWorkers ) proc = 0;
	appl.setVP( proc );
	appl.createProcess( func, i, (r)sbar, numWorkers, (r)&p[i], 0, 0 );
    }
    appl.setVP( startproc );
    appl.createProcessAndWait( func, 0, (r)sbar, numWorkers, (r)&p[0], 0, 0 );

    delete sbar;
}


void
DoReadMemWorker( long myId, BarrierType *bar, int numWorkers, teststruct *p )
{
    int i;
    struct timeval start, end;
    char *buf = (char *)p->ptr;
    const int size  = p->size;
    const int triggerit  = p->misc;
    const int stride = PAGE_SIZE;
    
    bar->enter();
    
    for( i = 0; i < size; i += stride ) {
	*(volatile char *)(&buf[i]);
    }

    ObjectHandle      toh;
    TornStatus rc = myProgram->getRegion( (addr_t)buf, toh );
    if( rc != 0 ) {
	printf("didn't find region, va = %lx\n", buf);
	return;
    }
    StubXRegionSimple region(toh);
    rc = region.unmap(buf, size);
    if( rc != 0 ) {
	printf("couldn't unmap region, va = %lx\n", buf);
	return;
    }

    bar->enter();
    
    gettimeofday( &start, 0 );

    if( triggerit ) tstEvent_startworker();

    for( i = 0; i < size; i += stride ) {
	*(volatile char *)(&buf[i]);
    }

    if( triggerit ) tstEvent_endworker();

    gettimeofday( &end, 0 );

    bar->enter();
    
    p->time1 = (end.tv_sec * 1000000LL + end.tv_usec) -
	(start.tv_sec * 1000000LL + start.tv_usec);

    bar->enter();
}


void
DoReadMem( int numWorkers, char **bufs, int size, int triggerit,
	   teststruct **parentp )
{
    int i;
    teststruct *p = new teststruct[numWorkers];

    for( i = 0; i < numWorkers; i++ ) {
	p[i].init( size, 0, 0, triggerit, bufs[i] );
    }

    DoConcTest(numWorkers,(tstatusfunc)DoReadMemWorker,MYVP,p);

    if( triggerit ) {
	*parentp = p;
    } else {
	delete [] p;
    }
}

int
ReadMem( int argc, char **argv, int numWorkers )
{
    int i, repeats;
    char **bufs;
    unsigned long pages, size;
    TornStatus rc;
    int mmap1;
    teststruct *p;
    int myproc;
    myConsole->myinfo(myproc);	// my physical processor number

    if( argv == 0 || argv[0] == 0 ) {
	// running directly from kernel; take args from kargs
	reg_t *kargs = (reg_t *)GetOut();
	myConsole->getKArgs();
	// test number is kargs[0] and kargs[1]
	pages = kargs[3];
	size = pages*PAGE_SIZE;
	mmap1 = kargs[4];
	repeats = kargs[5];
    } else {
	if( argc != 4 ) {
	    fprintf(stderr,"usage: %s <numworkers> <pages> <1|2|3|4 map> <rep>\n",
		    argv[0]);
	    return 1;
	}
	size = atoi(argv[3]);
	mmap1 = atoi(argv[4]);
	repeats = atoi(argv[5]);
    }
    
    if( numWorkers < 1 || size < numWorkers ) {
	fprintf(stderr, "usage: %s <numworkers> <pages> <1|2|3|4 map> <rep>\n",
		argv[0]);
	return 1;
    }

#if 0
    if( myConsole->OnSableSim() == 0 ) {
	sleep(1+myproc);
    } else {
	usleep(1000*(1+myproc));
    }
#endif

    FCMtype fcmtype = FCM_SIMPLE;
    
    printf("pmem: workers %d size %d mmap1 %d reps %d - starting...\n",
	   numWorkers, size, mmap1, repeats);
    
    bufs = (char **)malloc(sizeof(char *)*numWorkers);

    addr_t baseoff = 1;

    if ( mmap1 == 4 ) {
	fcmtype = FCM_PARTITIONEDSIMPLE;
	mmap1 = 1;
    }
    
    if( mmap1 == 2 || mmap1 == 3  ) {
	for( i = 0; i < numWorkers; i++ ) {
	    addr_t vaddr;
	    if ( mmap1 == 3 ) {
		if( i > 14 ) baseoff = (0x3B - i);
		vaddr = (i+baseoff)*(1ULL<<24);
	    } else {
		vaddr = 0;
	    }   
	    StubXCORZero *corZero = new StubXCORZero( size );
	    rc = StubXmyRegionSimpleCustom::BindWriteRegion( myProgram->getOH(),
							     corZero->getOH(),
							     fcmtype,
							     size, vaddr );
	    tassert( rc == 0, 0 );
	    bufs[i] = (char *)vaddr;
	    printf("*** Region %d Located @ %llx\n",i,vaddr);
	}
    } else if ( mmap1 == 1 ) {
	addr_t vaddr = 0;
	StubXCORZero corZero( size*numWorkers ); 
	rc = StubXmyRegionSimpleCustom::BindWriteRegion( myProgram->getOH(),
						 corZero.getOH(),
						 fcmtype,
						 size*numWorkers, vaddr );
	tassert( rc == 0, 0 );
	for( i = 0; i < numWorkers; i++ ) {
	    bufs[i] = (char *)(vaddr+i*size);
	}
    } else {
	fprintf(stderr, "usage: %s <numworkers> <pages> <1|2|3|4 map> <rep>\n",
		argv[0]);
	return 1;
    }

#if 0
    if( myConsole->OnSableSim() == 0 ) {
	sleep(NumProc()-myproc);
    } else {
	usleep(1000*(NumProc()-myproc));
    }
#endif

    tstEvent_testinfo(numWorkers, size/PAGE_SIZE);
    int triggerit = 0;
    for( i = 0; i < repeats; i++ ) {
	triggerit = (i == (repeats/2));
	DoReadMem( numWorkers, bufs, size, triggerit, &p );
    }

    printf("fault: workers %d, size %d\n",
	   numWorkers, size);
    for( i = 0; i < numWorkers; i++ ) {
	printf("\t%d: Total: %ld iter: %ld\n", i,
	       p[i].time1, p[i].time1/(size/PAGE_SIZE));
    }
    //printf("pmem: %d %d %d - done\n", numWorkers, size, repeats);
#if 0
    if( myConsole->OnSableSim() == 0 ) {
	sleep(30);
    } else {
	usleep(10000*30);
    }
#endif
    
    return 0;
}

void
DoFileLenWorker( long myId, BarrierType *bar, int numWorkers, teststruct *p )
{
    int i;
    struct timeval start, end;
    ObjectHandle *fileHandles = (ObjectHandle *)p->ptr;
    const int iters  = p->iters;
    const int triggerit  = p->misc;
    StubXMappedIO stub(fileHandles[myId]);


    // get things warmed up
    for( i = 0; i < 5; i++ ) {
	(void)stub.getFileLength();
	(void)stub.getFileLength();
	(void)stub.getFileLength();
    }

    bar->enter();

    gettimeofday( &start, 0 );

    if( triggerit ) tstEvent_startworker();

    for( i = 0; i < iters; i++ ) {
	(void)stub.getFileLength();
    }

    if( triggerit ) tstEvent_endworker();

    gettimeofday( &end, 0 );

    bar->enter();
    
    p->time1 = (end.tv_sec * 1000000LL + end.tv_usec) -
	(start.tv_sec * 1000000LL + start.tv_usec);

    bar->enter();
}


void
DoFileLen( int numWorkers, ObjectHandle *fileHandles, int iters, int triggerit,
	   teststruct **parentp )
{
    int i;
    teststruct *p = new teststruct[numWorkers];

    for( i = 0; i < numWorkers; i++ ) {
	p[i].init( 0, iters, 0, triggerit, fileHandles );
    }

    DoConcTest(numWorkers,(tstatusfunc)DoFileLenWorker,MYVP,p);

//    if( triggerit ) {
	*parentp = p;
//    } else {
//	delete [] p;
//    }
}

int
FileLen( int argc, char **argv, int numWorkers )
{
    int i, repeats, sameFile, iters;
    TornStatus   rc;
    int          oflags;
    int          mode;
    ObjectHandle fhandle, *fileHandles;
    u_long       obj_type;
    char        *filefmt = "/nfstop/file%c";
    char         filename[128];
    int          myproc;
    int          filenameOffset;
    teststruct *p;
    myConsole->myinfo(myproc);	// my physical processor number
    filenameOffset = myproc;

    if( argv == 0 || argv[0] == 0 ) {
	// running directly from kernel; take args from kargs
	reg_t *kargs = (reg_t *)GetOut();
	myConsole->getKArgs();
	// test number is kargs[0] and kargs[1]
	iters	   = kargs[3];
	sameFile   = kargs[4];
	repeats	   = kargs[5];
    } else {
	if( argc != 4 ) {
	    fprintf(stderr,
		    "usage: %s <numworkers> <iters> <0|1/samefile> <rep>\n",
		    argv[0]);
	    return 1;
	}
	// test number is argv[1]
	iters	   = atoi(argv[3]);
	sameFile   = atoi(argv[4]);
	repeats	   = atoi(argv[5]);
    }
    
    if( numWorkers < 1 ) {
	fprintf(stderr,
		"usage: %s <numworkers> <iters> <0|1/samefile> <rep>\n",
		argv[0]);
	return 1;
    }

    if( myConsole->OnSableSim() == 0 ) {
	sleep(1+myproc);
    } else {
	usleep(1000*(1+myproc));
    }

    printf("filelen (%d): workers %d iters %d sameFile %d repeats %d - starting...\n",
	   filenameOffset, numWorkers, iters, sameFile, repeats);

    fileHandles = (ObjectHandle *)malloc(sizeof(ObjectHandle)*numWorkers);

    // for multi-programmed test, sameFile means don't use physical proc
    // as offset, so all programs are using the same filename
    if( sameFile ) filenameOffset = 0;
    
    for( i = 0; i < numWorkers; i++ ) {

	if( i == 0 || !sameFile ) {
	    sprintf( filename, filefmt, 'A'+filenameOffset+i );

	    //printf("Using file %s for worker %d\n", filename, i);

#if 0
	    printf("Creating %s\n", filename);
	    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0700);
	    if( fd < 0 ) {
		fprintf( stderr, "couldn't open the file '%s', %d\n",
			 filename, fd );
		return 1;
	    }
	    close(fd);
#endif

	    oflags   = O_RDONLY;
	    mode     = 0;
	    obj_type = 0;
	    rc = LowlevelOpen( filename, oflags, mode, &fhandle, &obj_type );
	    if( rc < 0 ) {
		fprintf( stderr, "couldn't lopen the file '%s', %lx\n",
			 filename, rc );
		return 1;
	    } else if( ! SISDERIVED( XMappedIO, obj_type ) ) {
		fprintf( stderr, "file '%s' wrong type: %lx\n",
			 filename, obj_type );
		return 1;
	    } 
	}
	fileHandles[i] = fhandle;
    }

    if( myConsole->OnSableSim() == 0 ) {
	sleep(NumProc()-myproc);
    } else {
	usleep(1000*(NumProc()-myproc));
    }

    int triggerit = 0;
    for( int k = 0; k < repeats; k++ ) {
	triggerit = (k == (repeats/2));
	printf("repeat %d triggerit %d\n", k, triggerit);
	DoFileLen( numWorkers, fileHandles, iters, triggerit, &p );

    printf("FileLen: workers %d, iters %d\n", numWorkers, iters);
    for( i = 0; i < numWorkers; i++ ) {
	printf("\t%d: Total: %ld iter: %ld\n", i,
	       p[i].time1, p[i].time1/iters);
    }
    }


    for( i = 0; i < numWorkers; i++ ) {
	if( i == 0 || !sameFile ) {
	    StubXMappedIO *stub = new StubXMappedIO(fileHandles[i]);
	    rc = stub->close();
	    if( rc != 0 ) {
		fprintf( stderr, "could not close file: %lx\n",rc);
	    }
	}
    }

    if( myConsole->OnSableSim() == 0 ) {
	sleep(30);
    } else {
	usleep(10000*30);
    }

    return 0;
}

class Thread {
    TornStatus   rc;			// thread result
    SimpleBLock  joinit;		// sync for joining

    ~Thread() {}			// null to prevent user delete

    // called on thread start side from createThread call
    static void start( Thread *thread, tstatusfunc func, void *arg ) {
	thread->rc = (*(TornStatus (*)(...))(func))(arg);
	thread->joinit.releaseLock();
    }

    Thread( tstatusfunc func, void *arg ) {
	joinit.acquireLock();		// thread will release when done
	myAppl->createProcess( (tstatusfunc)Thread::start,
			       (r)this, (r)func, (r)arg, 0, 0, 0 );
    }

public:

    static Thread *create( tstatusfunc func, void *arg ) {
	return new Thread( func, arg );
    }
    TornStatus join() {
	joinit.acquireLock();
	TornStatus val = rc;
	delete this;
	return val;
    }
};

TornStatus
CreateThreadFunc( reg_t val )
{
    return val;
}

void
DoCreateThreadWorker(long myId,BarrierType *bar,int numWorkers,teststruct *p)
{
    int i;
    struct timeval start, end;
    const int iters  = p->iters;
    const int triggerit  = p->misc;
    
    bar->enter();

    gettimeofday( &start, 0 );

    if( triggerit ) tstEvent_startworker();

    for( i = 0; i < iters; i++ ) {
	Thread *t = Thread::create( (tstatusfunc)CreateThreadFunc, 0 );
	(void)t->join();
    }

    if( triggerit ) tstEvent_endworker();

    gettimeofday( &end, 0 );

    bar->enter();
    
    p->time1 = (end.tv_sec * 1000000LL + end.tv_usec) -
	(start.tv_sec * 1000000LL + start.tv_usec);

    bar->enter();
}


void
DoCreateThread( int numWorkers, int iters, int triggerit,
		teststruct **parentp )
{
    int i;
    teststruct *p = new teststruct[numWorkers];

    for( i = 0; i < numWorkers; i++ ) {
	p[i].init( 0, iters, 0, triggerit, 0 );
    }

    DoConcTest(numWorkers,(tstatusfunc)DoCreateThreadWorker,MYVP,p);

    if( triggerit ) {
	*parentp = p;
    } else {
	delete [] p;
    }
}

int
CreateThread( int argc, char **argv, int numWorkers )
{
    int i, repeats, iters;
    teststruct *p;
    int myproc;
    myConsole->myinfo(myproc);	// my physical processor number

    if( argv == 0 || argv[0] == 0 ) {
	// running directly from kernel; take args from kargs
	reg_t *kargs = (reg_t *)GetOut();
	myConsole->getKArgs();
	// test number is kargs[0] and kargs[1]
	iters	   = kargs[3];
	repeats	   = kargs[4];
    } else {
	if( argc != 4 ) {
	    fprintf(stderr,
		    "usage: %s <numworkers> <iters> <rep>\n",
		    argv[0]);
	    return 1;
	}
	// test number is argv[1]
	iters	   = atoi(argv[3]);
	repeats	   = atoi(argv[4]);
    }
    
    if( numWorkers < 1 ) {
	fprintf(stderr,
		"usage: %s <numworkers> <iters> <rep>\n",
		argv[0]);
	return 1;
    }

    if( myConsole->OnSableSim() == 0 ) {
	sleep(1+myproc);
    } else {
	usleep(1000*(1+myproc));
    }

    printf("CreateThread: workers %d iters %d repeats %d - starting...\n",
	   numWorkers, iters, repeats);

    if( myConsole->OnSableSim() == 0 ) {
	sleep(NumProc()-myproc);
    } else {
	usleep(1000*(NumProc()-myproc));
    }

    int triggerit = 0;
    for( i = 0; i < repeats; i++ ) {
	triggerit = (i == (repeats/2));
	DoCreateThread( numWorkers, iters, triggerit, &p );
    }

    printf("CreateThread: workers %d, iters %d\n", numWorkers, iters);
    for( i = 0; i < numWorkers; i++ ) {
	printf("\t%d: Total: %ld iter: %ld\n", i,
	       p[i].time1, p[i].time1/iters);
    }

    //printf("CreateThread: - done\n");
    if( myConsole->OnSableSim() == 0 ) {
	sleep(30);
    } else {
	usleep(10000*30);
    }

    return 0;
}


void
DoConcKMATestWorker( long myId, BarrierType *bar, int numWorkers,
		     teststruct *p )
{
    struct timeval start, end;
    const int iters  = p->iters;
    const int size  = p->size;
    const int triggerit  = p->misc;

    void **elems = (void **)malloc( size * sizeof(void *) );
    
    bar->enter();

    if( triggerit ) tstEvent_startworker();
    gettimeofday( &start, 0 );
    for( int i = 0; i < iters; i++ ) {
	for( int j = 0; j < size; j++ ) {
	    elems[j] = LMALLOC(256);
	}
	for( j = 0; j < size; j++ ) {
	    LFREE(elems[j], 256);
	}
    }
    gettimeofday( &end, 0 );
    if( triggerit ) tstEvent_endworker();

    bar->enter();

    p->time1 = (end.tv_sec * 1000000LL + end.tv_usec) -
	(start.tv_sec * 1000000LL + start.tv_usec);

    free(elems);
    
    bar->enter();
}


void
DoConcKMATest( int numWorkers, int size, int triggerit,
	       teststruct **parentp )
{
    int i;
    teststruct *p = new teststruct[numWorkers];

    for( i = 0; i < numWorkers; i++ ) {
	p[i].init( size, 1, 0, triggerit, 0 );
    }

    DoConcTest(numWorkers,(tstatusfunc)DoConcKMATestWorker,MYVP,p);

    if( triggerit ) {
	*parentp = p;
    } else {
	delete [] p;
    }
}

int
KMATest( int argc, char **argv, int numWorkers )
{
    int i, repeats, iters;
    teststruct *p;
    int myproc;
    myConsole->myinfo(myproc);	// my physical processor number

    if( argv == 0 || argv[0] == 0 ) {
	// running directly from kernel; take args from kargs
	reg_t *kargs = (reg_t *)GetOut();
	myConsole->getKArgs();
	// test number is kargs[0] and kargs[1]
	iters	   = kargs[3];
	repeats	   = kargs[4];
    } else {
	if( argc != 4 ) {
	    fprintf(stderr,
		    "usage: %s <numworkers> <iters> <rep>\n",
		    argv[0]);
	    return 1;
	}
	// test number is argv[1]
	iters	   = atoi(argv[3]);
	repeats	   = atoi(argv[4]);
    }
    
    if( numWorkers < 1 ) {
	fprintf(stderr,
		"usage: %s <numworkers> <iters> <rep>\n",
		argv[0]);
	return 1;
    }

    if( myConsole->OnSableSim() == 0 ) {
	sleep(1+myproc);
    } else {
	usleep(1000*(1+myproc));
    }

    printf("KMATest: workers %d iters %d repeats %d - starting...\n",
	   numWorkers, iters, repeats);

    if( myConsole->OnSableSim() == 0 ) {
	sleep(NumProc()-myproc);
    } else {
	usleep(1000*(NumProc()-myproc));
    }

    int triggerit = 0;
    for( i = 0; i < repeats; i++ ) {
	triggerit = (i == (repeats/2));
	DoConcKMATest( numWorkers, iters, triggerit, &p );
    }

    printf("KMATest: workers %d, iters %d\n", numWorkers, iters);
    for( i = 0; i < numWorkers; i++ ) {
	printf("\t%d: Total: %ld iter: %f\n", i,
	       p[i].time1, p[i].time1*1.0/iters);
    }

    //printf("CreateThread: - done\n");
    if( myConsole->OnSableSim() == 0 ) {
	sleep(30);
    } else {
	usleep(10000*30);
    }

    return 0;
}

extern "C" unsigned long GetIdleTime( int proc );

int
main( int argc, char **argv )
{
    int test, numproc, rc;
    
    if( argv == 0 || argv[0] == 0 ) {
	// running directly from kernel; take args from kargs
	reg_t *kargs = (reg_t *)GetOut();
	myConsole->getKArgs();
	test = kargs[1];
	numproc = kargs[2];
    } else {
	test = atoi(argv[1]);
	numproc = atoi(argv[2]);
    }

    if( numproc == 0 ) {
	numproc = NumProc( );
    }

    unsigned long idle = GetIdleTime(0);

    switch ( test ) {
    case 1:
	rc = ReadMem( argc, argv, numproc );
	break;
    case 2:
	rc = FileLen( argc, argv, numproc );
	break;
    case 3:
	rc = CreateThread( argc, argv, numproc );
	break;
    case 4:
	rc = KMATest( argc, argv, numproc );
	break;
    default:
	printf("Unknown test %d\n", test);
	printf("\t1 = ReadMem; 2 = FileLen; 3 = CreateThread; 4 = kma\n");
	rc = 1;
	break;
    }

    printf("Idle time is %ld\n", GetIdleTime(0) - idle);
    
    return rc;
}
