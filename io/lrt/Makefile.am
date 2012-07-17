include io/lrt/ulnx/Makefile.am
include io/lrt/bare/Makefile.am

io_lrt_sources = io/lrt/LRTConsole.h io/lrt/ethlib.h

if LRT_ULNX
io_lrt_sources += $(io_lrt_ulnx_sources)
endif

if LRT_BARE
io_lrt_sources += $(io_lrt_bare_sources)
endif