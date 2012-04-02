
bin_PROGRAMS += apps/ssac/ssac
apps_ssac_ssac_SOURCES = apps/ssac/ssac.c \
	contrib/jmcddn/ssac_cpp/SSACSimpleSharedArray.C \
	contrib/jmcddn/ssac_cpp/CacheSimple.C \
	contrib/jmcaddn/ssac_cpp/Test.C
apps_ssac_ssac_CPPFLAGS = $(AM_CPPFLAGS) -DEBBLIB
apps_ssac_ssac_LDADD = $(ebbos_libs)
apps_ssac_ssac_DEPENDENCIES = $(ebbos_libs)

