#include "testAndDecSharedCounterCO.H"

virtual TornStatus
testAndDecSharedCounterCO::increment()
{
    lock.acquireLock();
    count++;
    lock.releaseLock();
}

virtual TornStatus
testAndDecSharedCounter::testAndDecrement()
{
    lock.acquireLock();
    if (count==0) {
	lock.releaseLock();
	return 0;
    }
    count--;
    lock.releaseLock();
    return 1;
}


