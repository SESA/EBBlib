include io/lrt/Makefile.am
io_sources = io/EthMgr.h io/EthMgrPrim.c io/EthMgrPrim.h \
	io/EthTypeMgr.h $(io_lrt_sources) 

if LRT_BARE
include io/bare/Makefile.am
io_lrt_sources += $(io_lrt_bare_sources)
io_sources += $(io_bare_sources)
endif