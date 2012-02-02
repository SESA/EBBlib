#ifndef _SSACSIMPLESHAREDARRAY_H_
#define _SSACSIMPLESHAREDARRAY_H_
#include "SSAC.H"
#include "CacheSimple.H"

class SSACSimpleSharedArray : public SSAC {
  class SimpleLock {
    int l;
  public:
		void releaseLock() {}
		void acquireLock() {}
  };
    enum {DEFAULT_ASSOCIATIVITY=4};
    struct HashQueues {
	SimpleLock lock;
	unsigned long count;
	CacheEntrySimple *entries;
	void init(const int &numentries);
	inline CacheEntrySimple *search(CacheObjectIdSimple &id,
					const int &numentries);
//	inline CacheEntrySimple *lruentry(const int &numentries);
	CacheEntrySimple *lruentry(const int &numentries);
	void rollover();
	HashQueues(); 
	~HashQueues();
    }       *_hashqs;
    int      _numhashqs;
    int      _associativity;
    
    SSACRef ref() { return (SSACRef)_ref;}
    SSACSimpleSharedArray( const int &numhashqs,
			   const int &associativity );
public:
    static SSACRef create( const int &numhashqs,
			   const int &associativity=DEFAULT_ASSOCIATIVITY) {
	return (SSACRef)((new SSACSimpleSharedArray(numhashqs,
						    associativity))->ref());
    }
    virtual EBBRC get( CacheObjectId &id, CacheEntry* &ce,
			    const gettype &type );
    virtual EBBRC putback( CacheEntry* &ce, const putflag &flag );
    virtual EBBRC flush();
    virtual EBBRC snapshot();
    ~SSACSimpleSharedArray();
};
#endif
