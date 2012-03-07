#include "testAndDecSharedCounter.H"

void testAndDecSharedCounter::increment() {
    lock.acquireLock();
    count++;
    lock.releaseLock();
}

int testAndDecSharedCounter::testAndDecrement() {
    lock.acquireLock();
    if (count==0) {
	lock.releaseLock();
	return 0;
    }
    count--;
    lock.releaseLock();
    return 1;
}


