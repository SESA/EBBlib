#include "SimpleSharedCounter.H"

int SimpleSharedCounter::inc() {
    count++;
    return count;
}

int SimpleSharedCounter::dec() {
    count--;
    return count;
}

int SimpleSharedCounter::add(const int &i) {
    count+=i;
    return count;
}
