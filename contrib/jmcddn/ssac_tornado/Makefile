# ############################################################################
# 
#                            Tornado Kernel
#                            Copyright 1994
# 
#                   Authors: Eric Parsons
# 
#       This software is free for all non-commercial use, and anyone in this
#  category may freely copy, modify, and redistribute any portion of it,
#  provided they retain this disclaimer and author list.
#       The authors do not accept responsibility for any consequences of
#  using this program, nor do they in any way guarantee its correct operation.
# 
#  $Id: Makefile,v 1.26 1998/12/11 07:04:01 jonathan Exp $
# 
# ############################################################################

default:: libs programs usrlvl

KERNEL=1
HAVE_OBJDIRS=1

ifndef INOBJDIR
#TORNNDEBUG_G=1
#FULLDEBUG=1
NDEBUG=1
#PARTIALDEBUG=1
endif

KERNDIR = ../..

include $(HOME)/.tornado/mfinc
include $(MKTORNTOP)/lib64/Makerules.torn
-include $(KERNDIR)/objfiles

ifndef SRCDIR
SRCDIR = ./
endif

INCDIRS = -I$(SRCDIR)/$(KERNDIR)
CFLAGS += $(INCDIRS) 
CXXFLAGS += $(INCDIRS) 
ASFLAGS += $(INCDIRS)

XHDRS = XmyRegionSimpleCustom.H
HDRS = $(XHDRS) SSAC.H CacheSimple.H SSACSimpleSharedArray.H \
       SSACSimpleReplicatedArray.H SSACSimpleReplicatedArray.H NewRefCountCO.H\
       RefCounCO.H scenario6.H SimplePositiveSharedcounter.H \
       testAndDecSharedCounter.H SimpleSharedCounter.H FCMDistributedBase.H \
       FCMPartitionedSimple.H 

SRCS = kern.C FCMDistributedBase.C FCMPartitionedSimple.C \
       XmyRegionSimpleCustom.C

#COMMONSRCS= main.C 
#COMMONSSACSRCS= CacheSimple.C
#REFCOUNTCOTESTSRCS = $(COMMONSRCS) RefCountCO.C RefCountCOtest.C
#VERIFYINSTSRCS = $(COMMONSRCS) verifyinst.C
#NEWREFCOUNTCOTESTSRCS = $(COMMONSRCS) NewRefCountCO.C NewRefCountCOtest.C
#NEWREFCOUNTSHAREDCOTESTSRCS = $(COMMONSRCS) NewRefCountSharedCO.C \
#                              NewRefCountSharedCOtest.C
#NEWREFCOUNTSIMPLECOTESTSRCS = $(COMMONSRCS) NewRefCountSimpleCO.C \
                              NewRefCountSimpleCOtest.C
#COUNTEROFCOUNTERSCOTESTSRCS = $(COMMONSRCS) CounterofCountersCOtest.C
#COUNTEROFCOUNTERSTESTSRCS = $(COMMONSRCS) CounterofCounterstest.C
#SCENARIO0ASRCS = $(COMMONSRCS) scenario0a.C SimplePositiveSharedCounter.C
#SCENARIO6ASRCS = $(COMMONSRCS) scenario6a.C testAndDecSharedCounter.C
#SHAREDCOUNTERSRCS = $(COMMONSRCS) scenario1.C SimpleSharedCounter.C
#SSACTESTSRCS= $(COMMONSRCS) $(COMMONSSACSRCS) SSACtest.C \
#              SSACSimpleSharedArray.C  SSACSimpleReplicatedArray.C \
#              SSACSimplePartitionedArray.C  

META_HDR  = $(XHDRS:%=Meta%)
STUB_HDR  = $(XHDRS:%=Stub%)
STUB_ASM  = $(STUB_HDR:%.H=%.s)
STUB_OBJ  = $(STUB_HDR:%.H=%.o)
STUB_INFO = $(XHDRS:%.H=%Info.h)


TARGETS = 

OBJS0 = $(SRCS:%.C=%.o)
OBJS1 = $(OBJS0:%.c=%.o)
OBJS = $(OBJS1:%.s=%.o)

TEST_OBJS = $(OBJS) $(STUB_OBJ)
OBJ_TEST_OBJS = $(TEST_OBJS:%=$(OBJDIR)/%)

TEST_TARGETS = kern


#REFCOUNTCOTESTOBJS = $(REFCOUNTCOTESTSRCS:%.C=%.o)
#NEWREFCOUNTCOTESTOBJS = $(NEWREFCOUNTCOTESTSRCS:%.C=%.o)
#VERIFYINSTTESTOBJS = $(VERIFYINSTSRCS:%.C=%.o)
#NEWREFCOUNTSHAREDCOTESTOBJS = $(NEWREFCOUNTSHAREDCOTESTSRCS:%.C=%.o)
#NEWREFCOUNTSIMPLECOTESTOBJS = $(NEWREFCOUNTSIMPLECOTESTSRCS:%.C=%.o)
#SCENARIO1OBJS = $(SCENARIO1SRCS:%.C=%
#SCENARIO0AOBJS = $(SCENARIO0ASRCS:%.C=%.o)
#SCENARIO6AOBJS = $(SCENARIO6ASRCS:%.C=%.o)
#COUNTEROFCOUNTERSCOTESTOBJS = $(COUNTEROFCOUNTERSCOTESTSRCS:%.C=%.o)
#COUNTEROFCOUNTERSTESTOBJS = $(COUNTEROFCOUNTERSTESTSRCS:%.C=%.o)
#SSACTESTOBJS = $(SSACTESTSRCS:%.C=%.o)

#TEST_OBJS = $(OBJS) $(STUB_OBJ) $(REFCOUNTCOTESTOBJS) $(NEWREFCOUNTCOTESTOBJS)
#TEST_OBJS = $(OBJS) $(STUB_OBJ) $(REFCOUNTCOTESTOBJS) $(NEWREFCOUNTCOTESTOBJS)\
#            $(NEWREFCOUNTSHAREDCOTESTOBJS) $(VERIFYINSTTESTOBJS)\
#            $(NEWREFCOUNTSIMPLECOTESTOBJS) $(SCENARIO1OBJS) $(SCENARIO0AOBJS) \
#            $(SCENARIO6AOBJS) $(SSACSIMPLESHAREDARRAYTESTOBJS) \
#            $(SSACSIMPLEREPLICATEDARRAYTESTOBJS) \
#            $(SSACSIMPLEPARTITIONEDARRAYTESTOBJS) \
#            $(COUNTEROFCOUNTERSCOTESTOBJS) $(COUNTEROFCOUNTERSTESTOBJS)\
$            $(SSACTESTOBJS)

#OBJ_VERIFYINST_OBJS = $(VERIFYINSTTESTOBJS:%=$(OBJDIR)/%)
#OBJ_REFCOUNTCOTEST_OBJS = $(REFCOUNTCOTESTOBJS:%=$(OBJDIR)/%)
#OBJ_NEWREFCOUNTCOTEST_OBJS = $(NEWREFCOUNTCOTESTOBJS:%=$(OBJDIR)/%)
#OBJ_NEWREFCOUNTSHAREDCOTEST_OBJS = $(NEWREFCOUNTSHAREDCOTESTOBJS:%=$(OBJDIR)/%)
#OBJ_NEWREFCOUNTSIMPLECOTEST_OBJS = $(NEWREFCOUNTSIMPLECOTESTOBJS:%=$(OBJDIR)/%)
#OBJ_SCENARIO1_OBJS = $(SCENARIO1OBJS:%=$(OBJDIR)/%)
#OBJ_SCENARIO0A_OBJS = $(SCENARIO0AOBJS:%=$(OBJDIR)/%)
#OBJ_SCENARIO6A_OBJS = $(SCENARIO6AOBJS:%=$(OBJDIR)/%)
#OBJ_COUNTEROFCOUNTERSCOTEST_OBJS = $(COUNTEROFCOUNTERSCOTESTOBJS:%=$(OBJDIR)/%)
#OBJ_COUNTEROFCOUNTERSTEST_OBJS = $(COUNTEROFCOUNTERSTESTOBJS:%=$(OBJDIR)/%)
#OBJ_SSACTEST_OBJS = $(SSACTESTOBJS:%=$(OBJDIR)/%)

#TEST_TARGETS = scenario1 scenario0a scenario6a RefCountCOtest NewRefCountCOtest
#TEST_TARGETS = scenario1 scenario0a scenario6a NewRefCountCOtest NewRefCountSharedCOtest NewRefCountSimpleCOtest SSACSimpleSharedArraytest
#TEST_TARGETS = verifyinst

#TEST_TARGETS = SSACSimpleReplicatedArraytest SSACSimpleSharedArraytest
#TEST_TARGETS = SharedCountertest CounterofCounterstest SSACSimpleReplicatedArraytest
#TEST_TARGETS = CounterofCounterstest 
#TEST_TARGETS = CounterofCounterstest CounterofCountersCOtest 
#TEST_TARGETS = NewRefCountCOtest
#TEST_TARGETS = tst

objdir_libs:: $(TEST_OBJS) $(STUB_OBJ)

objdir_install_libs:: $(STUB_OBJ)
	$(AR) ruv $(LIBDIR)/libc.a $(STUB_OBJ) 

programs:: $(TEST_TARGETS)

usrlvl:
	make -f Makefile.usrlvl

.PHONY: libs


kern : libs $(OBJ_TEST_OBJS) $(KERNOBJS) Makefile 
	@echo Making kern -- $(LDFLAGS)
	@$(CXX) -o $@ $(OBJ_TEST_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000


#tst : libs $(OBJ_TEST_OBJS) $(KERNOBJS) Makefile 
#	@echo Making tst -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_TEST_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000
# in the future, we may want to link the data section at an absolute address
#	@$(CXX) -o $@ $(OBJ_TEST_OBJS) $(KERNOBJS) $(LDFLAGS) -T 0xffffffff80020000 -D `./dataoffset $@` -Wl,-rdata_shared


#RefCountCOtest : libs $(OBJ_REFCOUNTCOTEST_OBJS) $(KERNOBJS) Makefile 
#	@echo Making RefCountCOtest -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_REFCOUNTCOTEST_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000


#verifyinst : libs $(OBJ_VERIFYINST_OBJS) $(KERNOBJS) Makefile 
#	@echo Making verifyinst -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_VERIFYINST_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000

#NewRefCountCOtest : libs $(OBJ_NEWREFCOUNTCOTEST_OBJS) $(KERNOBJS) Makefile 
#	@echo Making NewRefCountCOtest -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_NEWREFCOUNTCOTEST_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000

#NewRefCountSharedCOtest : libs $(OBJ_NEWREFCOUNTSHAREDCOTEST_OBJS) $(KERNOBJS) Makefile 
#	@echo Making NewRefCountSharedCOtest -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_NEWREFCOUNTSHAREDCOTEST_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000

#NewRefCountSimpleCOtest : libs $(OBJ_NEWREFCOUNTSIMPLECOTEST_OBJS) $(KERNOBJS) Makefile 
#	@echo Making NewRefCountSimpleCOtest -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_NEWREFCOUNTSIMPLECOTEST_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000

#scenario1 : libs $(OBJ_SCENARIO1_OBJS) $(KERNOBJS) Makefile 
#	@echo Making scenario1 -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_SCENARIO1_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000

#scenario0a : libs $(OBJ_SCENARIO0A_OBJS) $(KERNOBJS) Makefile 
#	@echo Making scenario0a -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_SCENARIO0A_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000

#scenario6a : libs $(OBJ_SCENARIO6A_OBJS) $(KERNOBJS) Makefile 
#	@echo Making scenario6a -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_SCENARIO6A_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000

#CounterofCountersCOtest : libs $(OBJ_COUNTEROFCOUNTERSCOTEST_OBJS) $(KERNOBJS) Makefile 
#	@echo Making CounterofCountersCOtest -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_COUNTEROFCOUNTERSCOTEST_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000

#CounterofCounterstest : libs $(OBJ_COUNTEROFCOUNTERSTEST_OBJS) $(KERNOBJS) Makefile 
#	@echo Making CounterofCounterstest -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_COUNTEROFCOUNTERSTEST_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000

#SSACtest : libs $(OBJ_SSACTEST_OBJS) $(KERNOBJS) Makefile 
#	@echo Making SSACtest -- $(LDFLAGS)
#	@$(CXX) -o $@ $(OBJ_SSACTEST_OBJS) $(KERNOBJS) $(LDFLAGS) -n -T 0xffffffff80020000

relink::
	-rm -f $(TEST_TARGETS) ; make programs

srcdir_clean objdir_clean ::
	-rm -f $(OBJS) $(LIBOBJS) $(TARGETS) $(TEST_TARGETS) \
		$(STUB_HDR) $(STUB_ASM) $(STUB_OBJ) $(META_HDR) $(STUB_INFO)

structure::
	-rm -f gserver nfs nload nload_numa pty \
		nserver sysinit sysinit-secondary ttime kernel
	ln -s boot kernel
	ln -s $(KERNDIR)/../boot/gserver/gserver .
	ln -s $(KERNDIR)/../boot/download/nload .
	ln -s $(KERNDIR)/../boot/download/nload_numa .
	ln -s $(KERNDIR)/../servers/nfs/nfs .
	ln -s $(KERNDIR)/../servers/pty/pty .
	ln -s $(KERNDIR)/../boot/numa/nserver .
	ln -s $(KERNDIR)/../boot/numa/sysinit .
	ln -s $(KERNDIR)/../numa/sysinit-secondary .
	ln -s $(KERNDIR)/..//numa/ttime .

include $(MKTORNTOP)/lib64/Makerules.end
