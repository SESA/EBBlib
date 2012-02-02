#ifndef _CACHESIMPLE_H_
#define _CACHESIMPLE_H_
#include "SSAC.H"

class CacheObjectIdSimple : public CacheObjectId  {
    unsigned int _id;
public:
    // INVALID must be set to a value that can never be a
    // a valid id value 
    enum {LOADSPIN=400*100,SAVESPIN=400*100,INVALID=0xffffffff};
    CacheObjectIdSimple() { _id=(unsigned int)INVALID; }
    CacheObjectIdSimple(const int id) { _id=id; }
    CacheObjectIdSimple& operator=(const CacheObjectIdSimple& src)
	{ _id=src._id; return *this; }
    CacheObjectIdSimple &operator=(const int src)
	{ _id=src; return *this; }
    int operator==(const CacheObjectIdSimple& other)
	{ return _id==other._id; }
    int operator!=(const CacheObjectIdSimple& other)
	{ return _id!=other._id; }
    int id() { return _id; }
    int valid() { return _id!=(unsigned int)INVALID; }
    void invalidate() { _id=(unsigned int)INVALID; }
    CacheObjectDataSimple load();
    EBBRC save(CacheObjectDataSimple data); 
    int index(const int &max) { return _id % max; }
};

// Simple data may want to change this later to a class
typedef void *CacheObjectDataSimple;

class  CacheEntrySimple : public CacheEntry {
public: 
    CacheObjectIdSimple id;
    enum CacheEntryStatusBits { ZERO=0, BUSY=1, DIRTY=2 };
    enum {BACKOFF=10000};
#ifndef NOLOCKSNOFLAGSNOCOUNTERS
    int flags;
    unsigned long lastused;
#endif
    CacheObjectDataSimple data;
    void sleep();
    void wakeup();
    void print();
    void dirty();
    
    CacheEntrySimple();
};
#endif
