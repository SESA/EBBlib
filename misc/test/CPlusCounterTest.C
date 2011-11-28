#include <config.h>
#include <stdint.h>
#include <cppunit/extensions/HelperMacros.h>
#include <l0/types.h>
#include <l0/cobj/cobj.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cplus/CPlusEBB.H>
#include <l0/cplus/CPlusEBBRoot.H>
#include <l0/cplus/CPlusEBBRootShared.H>
#include <misc/CtrCPlus.H>

class CPlusCounterTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(CPlusCounterTest);
  CPPUNIT_TEST(testIncrement);
  CPPUNIT_TEST_SUITE_END();
private:
  CtrCPlus *ctr;
public:
  void setUp() {
    static CtrCPlus ctrObj;
    ctr = &ctrObj;
    ctr->init();
  }
  
  void tearDown() {
  }
  
  void testIncrement() {
    uintptr_t v1, v2;
    ctr->val(v1);
    ctr->inc();
    ctr->val(v2);
    CPPUNIT_ASSERT_EQUAL(v1,v2 - 1);
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(CPlusCounterTest);
