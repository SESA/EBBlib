/*
 * Copyright (C) 2011 by Project SESA, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <config.h>
#include <stdint.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/mem.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <l0/cobj/cobj.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>

class MemMgrPrim_Test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(MemMgrPrim_Test);
  CPPUNIT_TEST(test);
  CPPUNIT_TEST_SUITE_END();
public:
  //I have a static function as the reset function
  // I have to do all my setup here because I cannot return back off a reset
  // exit(0) will be intercepted by the test infrastructure and it will accept
  // the test as successful. Unfortunately, I cannot run a second test because
  // it will be in this address space, so a new file/runner 
  // would need to be created
  static void do_test()
  {
    EBBRC rc;
    int *i;
    lrt_mem_init();
    lrt_trans_init();
    trans_init();
    EBBMemMgrPrimInit();
    rc = EBBPrimMalloc(sizeof(int), &i, EBB_MEM_DEFAULT);
    CPPUNIT_ASSERT(EBBRC_SUCCESS(rc));
    rc = EBBPrimFree(i);
    CPPUNIT_ASSERT(EBBRC_SUCCESS(rc));
    exit(0);
  }

  void test()
  {
    lrt_pic_init(do_test);
  }

};

int
main ()
{
 CppUnit::TextUi::TestRunner runner;
 runner.addTest(MemMgrPrim_Test::suite());
 return !runner.run();
}
