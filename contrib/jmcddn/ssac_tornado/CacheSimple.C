#include "CacheSimple.H"
#include <tornado/TAssert.H>
 
CacheObjectDataSimple
CacheObjectIdSimple :: load()
{
    register int i=0;
    while(i<LOADSPIN) i++;
    return (void *)long(_id);
}

TornStatus
CacheObjectIdSimple :: save(CacheObjectDataSimple data)
{
    register int i=0;
    while(i<SAVESPIN) i++;
    return 0;
}
 
CacheEntrySimple :: CacheEntrySimple()
{
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
    flags=ZERO;
    lastused=0;
#endif
    data=0;
}
  
void 
CacheEntrySimple :: sleep()
{
    // spin for a short period of time
    register int i=0;
    while(i<BACKOFF) i++;
}

void
CacheEntrySimple :: wakeup()
{
    return;
}

void
CacheEntrySimple :: dirty()
{
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
    flags |= DIRTY;
#endif
    return;
}

void
CacheEntrySimple :: print()
{
    printf("CacheEntrySimple: \n\tthis=%lx\n\tid=%d\n\tflags=",this,id.id());
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
    if (flags & BUSY) printf("BUSY | "); else printf("FREE | ");
    if (flags & DIRTY) printf("DIRTY"); else printf("CLEAN ");
    printf("\n\tlastused=%U\n\tdata=%lx\n",lastused,data);
#endif
}
