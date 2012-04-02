
#include <apps/ssactst/simpleTst.H>
#include <contrib/jmcddn/ssac_cpp/EBBKludge.H>
#include <contrib/jmcddn/ssac_cpp/SSACSimpleSharedArray.H>

enum {HASHTABLESIZE=8192};//this is the size of hashqs, each with an 'associative' ammount of entries.

extern "C" void simpleTst();

#if 1
void
simpleTst()
{
  SSACId ssac;
  CacheObjectIdSimple id(0);
  CacheEntrySimple *entry=0;
  SSAC *tmp;
  EBBRC rc;

  SSACSimpleSharedArray::Create(ssac, HASHTABLESIZE);
  tmp = EBBId_DREF(ssac);

  for (unsigned long i=0; i<HASHTABLESIZE; i++) {
    id = i;
    rc=DREF(ssac)->get((CacheObjectId &)id,(CacheEntry * &)entry,
		       SSAC::GETFORWRITE);
    entry->data = (void *)i; // set data pointer to i TODO: verify
    rc=DREF(ssac)->putback((CacheEntry * &)entry, SSAC::KEEP);
  }
}
#else
#include <misc/CtrCPlus.H>
void
simpleTst()
{
  test_cplus_counter();
}
#endif

