#include "SimplePositiveSharedCounter.H"

int SimplePositiveSharedCounter::inc() {
    lock.acquireLock();
    count++;
    lock.releaseLock();
    return count;
}

int SimplePositiveSharedCounter::dec() {
    lock.acquireLock();
    count--;
    lock.releaseLock();
    return count;
}


