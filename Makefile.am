AM_CFLAGS = -Wall -Werror

include arch/Makefile.am
include l0/Makefile.am
include lrt/Makefile.am
include misc/Makefile.am
include net/Makefile.am

include test/Makefile.am

bin_PROGRAMS = ebbtest

ebbtest_SOURCES = EBBStart.c $(arch_sources) $(l0_sources) $(lrt_sources) \
	$(misc_sources) $(net_sources)

TESTS = cppunit
check_PROGRAMS = cppunit
cppunit_SOURCES = $(test_cppunit_sources) $(misc_cppunit_sources) \
	$(ebbtest_SOURCES)
cppunit_CXXFLAGS = $(CPPUNIT_CFLAGS)
cppunit_LDFLAGS = $(CPPUNIT_LIBS)