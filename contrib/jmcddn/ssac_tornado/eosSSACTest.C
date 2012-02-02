#include <stdio.h>

#define TRACE(fmt, ...)	fprintf(stderr, "%s: " fmt "\n", __func__, ##__VA_ARGS__)

typedef int EBBRC;

class EBB {
protected:
  int _ref;
};

#include "SSAC.H"

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

#if 0
class foo {
  int _i;
public:
  int val() { return _i; }
  int val(int i) { int old=_i; _i = i; return old;}
};
#endif

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

  return 0;
}
