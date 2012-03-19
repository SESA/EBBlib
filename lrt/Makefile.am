include lrt/bare/Makefile.am
include lrt/ulnx/Makefile.am

lrt_sources = lrt/assert.h lrt/io.h lrt/misc.h lrt/startinfo.h \
	lrt/string.c \
	lrt/string.h

if LRT_ULNX
lrt_sources += $(lrt_ulnx_sources)
else
if LRT_BARE
lrt_sources += $(lrt_bare_sources)
endif
endif
