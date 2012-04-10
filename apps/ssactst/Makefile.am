bin_PROGRAMS += apps/ssactst/ssactst
apps_ssactst_ssactst_SOURCES = apps/ssactst/ssactst.cpp \
	contrib/jmcddn/ssac_cpp/SSACSimpleSharedArray.C \
	contrib/jmcddn/ssac_cpp/CacheSimple.C \
	contrib/jmcddn/ssac_cpp/Test.C
apps_ssactst_ssactst_CPPFLAGS = $(AM_CPPFLAGS) -DEBBLIB
apps_ssactst_ssactst_LDADD = $(ebbos_libs) $(LDADD)
apps_ssactst_ssactst_DEPENDENCIES = $(ebbos_libs)

