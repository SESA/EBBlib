AM_CFLAGS = -Wall -Werror
if powerpc64
AM_CFLAGS += -m64
AM_CCASFLAGS = -m64
endif

include apps/Makefile.am
include arch/Makefile.am
include l0/Makefile.am
include l1/Makefile.am
include lrt/Makefile.am
include misc/Makefile.am
include net/Makefile.am

lib_LIBRARIES = libebb.a

libebb_a_SOURCES = $(arch_sources) $(l0_sources) $(lrt_sources) \
  $(misc_sources) $(net_sources) $(l1_sources)
