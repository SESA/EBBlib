#ifndef _SSAC_H_
#define _SSAC_H_

class SSAC;
#ifndef EBBLIB
typedef SSAC *SSACId;
#else
typedef SSAC **SSACId;
#endif

#define DOTRACE 1
#undef NOLOCKSNOFLAGSNOCOUNTERS

typedef void *CacheObjectDataSimple;

class CacheObjectId {
public:
  virtual int index(const int  &max)=0;
  virtual int valid()=0;
  virtual CacheObjectDataSimple load()=0;
  virtual EBBRC save(CacheObjectDataSimple data)=0;
};

// Here to provided base type for Cache Entries
class  CacheEntry {};

// Base Class for SSAC classes
class SSAC : public CPlusEBB {  
public:
  enum putflag { KEEP, DISCARD };
  enum gettype { GETFORREAD, GETFORWRITE };
  virtual EBBRC get( CacheObjectId &id, CacheEntry* &ce,
		     const gettype &type )=0; 
  virtual EBBRC putback( CacheEntry* &ce, const putflag &flag )=0;
  virtual EBBRC flush()=0;
  virtual EBBRC snapshot()=0;
};
#endif




