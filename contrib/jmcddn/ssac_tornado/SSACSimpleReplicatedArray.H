#ifndef _SSACSIMPLEREPLICATEDARRAY_H_
#define _SSACSIMPLEREPLICATEDARRAY_H_
#include "SSAC.H"
#include "CacheSimple.H"
#include <tornado/sys/GOBJ.H>
#include <tornado/ot.H>
#include <tornado/Lock.H>


class SSACSimpleReplicatedArray : public SSAC {
    class SSACSimpleReplicatedMH : public MHReplicate {
	int  _numhashqsperrep;
	int  _associativity;
    public:
	SSACSimpleReplicatedMH(const int &numhashqs,
			       const int &associativity ) {
	    _numhashqsperrep=numhashqs;
	    _associativity=associativity;
	}
	
	ClusteredObject * createRepOn(const int vp) {
	    RFUNC0(vp,(SSACSimpleReplicatedArray **)_ref,
		   SSACSimpleReplicatedArray::init);
	    return findRepOn(vp);
	}
	
	virtual ClusteredObject * createFirstRep() {
	    return (ClusteredObject *)
		new SSACSimpleReplicatedArray(_numhashqsperrep,
					      _associativity);
	}
	virtual ClusteredObject * createRep() {
	    return (ClusteredObject *)
		new SSACSimpleReplicatedArray(_numhashqsperrep,
					      _associativity);
	}
    };
    friend class SSACSimpleReplicatedArray::SSACSimpleReplicatedMH;
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

    struct DirEntry {
	CacheObjectIdSimple id;
	enum DirLineStatusBits { ZERO=0, BUSY=1, MODIFIED=2 };
	int flags;
	reg_t vpmask;
	DirEntry()                   { flags=ZERO; vpmask=0;    }
	void clearall()              { flags = ZERO;            } 
	int  busy()                  { return flags & BUSY;     }
	void setbusy()               { flags |= BUSY;           }
	void clearbusy()             { flags &= ~BUSY;          }
	int  modified()              { return flags & MODIFIED; }
	void setmodified()           { flags |= MODIFIED;       }
	void clearmodified()         { flags &= ~MODIFIED;      }
	void addvp(const int &vp)    { vpmask |= (1<<vp);       }
	void removevp(const int &vp) { vpmask &= ~(1<<vp);      }    
	enum {BACKOFF=10000};
	void spin();
    };
  
    struct DirLine {
	SimpleSpinBlockLock lock;
	struct DirEntry * direntries;
	~DirLine() { if (direntries) delete[] direntries; }
        struct DirEntry * lookupid(CacheObjectIdSimple &id,
					  const int &associativity);
	void adddirentry(CacheObjectIdSimple &id, const int &vp,
		 const int &associativity);

    } *_dirlines;
    
    CacheEntrySimple *getremotecopyfromdir(struct DirEntry *direntry);
    void setbusyonallcopiesindir(struct DirEntry *direntry,
				 CacheEntrySimple *local);
    void updateandclearbusyonallcopiesindir(struct DirEntry *direntry,
					    CacheEntrySimple *local);
    int removefromdir(CacheObjectIdSimple &id, const int &vp,
		      int dirty);
    struct DirLine *finddirline(CacheObjectIdSimple &id);
    int      _numhashqs;
    int      _associativity;
    
    SSACRef ref() { return (SSACRef)_ref;}
    SSACSimpleReplicatedArray( const int &numhashqs,
			   const int &associativity );
public:
    static SSACRef create( const int &numhashqs,
			   const int &associativity=DEFAULT_ASSOCIATIVITY) {
	return (SSACRef)((new SSACSimpleReplicatedMH(numhashqs,
						     associativity))->ref());
    }
    virtual TornStatus init() { return 0; }
    virtual TornStatus get( CacheObjectId &id, CacheEntry* &ce,
			    const gettype &type );
    virtual TornStatus putback( CacheEntry* &ce, const putflag &flag );
    virtual TornStatus flush();
    virtual TornStatus snapshot();
    ~SSACSimpleReplicatedArray();
};
#endif
