#include <stdio.h>
#include "EBBKludge.H"

#include "SSACSimpleSharedArray.H"

class MySSAC : public SSAC {
public:
  virtual EBBRC get( CacheObjectId &id, CacheEntry* &ce,
		     const gettype &type ); 
  virtual EBBRC putback( CacheEntry* &ce, const putflag &flag );
  virtual EBBRC flush();
  virtual EBBRC snapshot();
  static  EBBRC create(MySSAC **o);
};

/* static */ EBBRC 
MySSAC::create(MySSAC **o)
{
  *o = new(MySSAC);
  return 0;
}

/* virtual */ EBBRC 
MySSAC::get( CacheObjectId &id, CacheEntry* &ce,
	     const gettype &type ) 
{
  return (EBBRC)0;
}

/* virtual */ EBBRC 
MySSAC::putback( CacheEntry* &ce, const putflag &flag )
{
  return (EBBRC)0;
}

/* virtual */ EBBRC 
MySSAC::flush()
{
  TRACE("BEGIN");
  return (EBBRC)0;
}

/* virtual */ EBBRC 
MySSAC::snapshot()
{
  return (EBBRC)0;
}

void
SSACSimpleSharedArrayTest(void)
{
  EBBRC status;
  TRACE("BEGIN");

  const int HASHTABLESIZE=128;
  CacheObjectIdSimple id(0);
  CacheEntrySimple *entry=0;

  SSACRef ssac = SSACSimpleSharedArray::create(HASHTABLESIZE);

  status=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
			 SSAC::GETFORREAD);
  entry->print();

  TRACE("END");
}

int 
main(int argc, char **argv)
{
  MySSAC *ssac;
  EBBRC rc;
  rc = MySSAC::create(&ssac);

  if (rc == 0) {
    ssac->flush();
  } else {
    fprintf(stderr, "ERROR: create failed\n");
  }

  SSACSimpleSharedArrayTest();

  return 0;
}
