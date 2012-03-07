#ifndef _SSACSIMPLEPARTITIONEDARRAY_H_
#define _SSACSIMPLEPARTITIONEDARRAY_H_
#include "SSAC.H"
#include "CacheSimple.H"
#include <tornado/sys/GOBJ.H>
#include <tornado/ot.H>
#include <tornado/Lock.H>


class SSACSimplePartitionedArray : public SSAC {
    class SSACSimplePartitionedMH : public MHReplicate {
	int  _numhashqsperrep;
	int  _associativity;
    public:
	SSACSimplePartitionedMH(const int &numhashqs,
			       const int &associativity ) {
	    _numhashqsperrep=numhashqs;
	    _associativity=associativity;
	}
	
	ClusteredObject * createRepOn(const int vp) {
	    RFUNC0(vp,(SSACSimplePartitionedArray **)_ref,
		   SSACSimplePartitionedArray::init);
	    return findRepOn(vp);
	}
	
	virtual ClusteredObject * createFirstRep() {
	    return (ClusteredObject *)
		new SSACSimplePartitionedArray(_numhashqsperrep,
					      _associativity);
	}
	virtual ClusteredObject * createRep() {
	    return (ClusteredObject *)
		new SSACSimplePartitionedArray(_numhashqsperrep,
					      _associativity);
	}
    };
    friend class SSACSimplePartitionedArray::SSACSimplePartitionedMH;
    enum {DEFAULT_ASSOCIATIVITY=4};
    struct HashQueues {
#ifndef NOLOCKSNOFLAGSNOCOUNTERS 
	SimpleSpinBlockLock lock;
	unsigned long count;
#endif
	CacheEntrySimple *entries;
	void init(const int &numentries);
	inline CacheEntrySimple *search(CacheObjectIdSimple &id,
					const int &numentries);
//	inline CacheEntrySimple *lruentry(const int &numentries);
	CacheEntrySimple *lruentry(const int &numentries);
	void rollover();
	HashQueues(); 
	~HashQueues();
    } *_hashqs;

    int      _numhashqs;
    int      _associativity;

    void partition(const int &index, int &vp, int &offset);
    SSACRef ref() { return (SSACRef)_ref;}
    SSACSimplePartitionedArray( const int &numhashqs,
			   const int &associativity );
public:
    static SSACRef create( const int &numhashqs,
			   const int &associativity=DEFAULT_ASSOCIATIVITY) {
	return (SSACRef)((new SSACSimplePartitionedMH(numhashqs,
						     associativity))->ref());
    }
    virtual TornStatus init() { return 0; }
    virtual TornStatus get( CacheObjectId &id, CacheEntry* &ce,
			    const gettype &type );
    virtual TornStatus putback( CacheEntry* &ce, const putflag &flag );
    virtual TornStatus flush();
    virtual TornStatus snapshot();
    ~SSACSimplePartitionedArray();
};
#endif
