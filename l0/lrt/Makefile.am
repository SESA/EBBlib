include l0/lrt/bare/Makefile.am
include l0/lrt/ulnx/Makefile.am

l0_lrt_sources = l0/lrt/const.h l0/lrt/pic.h l0/lrt/types.h \
	l0/lrt/trans.h l0/lrt/mem.h l0/lrt/exit.h l0/lrt/trans.c

if LRT_ULNX
l0_lrt_sources += $(l0_lrt_ulnx_sources)
else
if LRT_BARE
l0_lrt_sources += $(l0_lrt_bare_sources)
endif
endif