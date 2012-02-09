#include <stdio.h>
#include <stdlib.h>

#include "EBBKludge.H"
#include "CacheSimple.H"
 
CacheObjectDataSimple
CacheObjectIdSimple :: load()
{
    register int i=0;
    while(i<LOADSPIN) i++; 
    return (void *)long(_id);
}

EBBRC
CacheObjectIdSimple :: save(CacheObjectDataSimple data)
{
    register int i=0;
    while(i<SAVESPIN) i++;
    return 0;
}
 
CacheEntrySimple :: CacheEntrySimple()
{
    flags=ZERO;
    lastused=0;
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
    flags |= DIRTY;
    return;
}

void
CacheEntrySimple :: print()
{
    printf("CacheEntrySimple: \n\tthis=%p\n\tid=%d\n\tflags=",this,id.id());
    if (flags & BUSY) printf("BUSY | "); else printf("FREE | ");
    if (flags & DIRTY) printf("DIRTY"); else printf("CLEAN ");
    printf("\n\tlastused=%ld\n\tdata=%p\n",lastused,data);
}
