
bin_PROGRAMS += apps/ssactst/ssactst

apps_ssactst_ssactst_SOURCES = apps/ssactst/ssactst.c \
	apps/ssactst/simpleTst.C \
	contrib/jmcddn/ssac_cpp/SSACSimpleSharedArray.C \
	contrib/jmcddn/ssac_cpp/CacheSimple.C 

apps_ssactst_ssactst_CPPFLAGS = $(AM_CPPFLAGS) -DEBBLIB
apps_ssactst_ssactst_LDADD = $(ebbos_libs)
apps_ssactst_ssactst_DEPENDENCIES = $(ebbos_libs)

