include net/lrt/bare/Makefile.am
include net/lrt/ulnx/Makefile.am

net_lrt_sources = net/lrt/ethlib.h
if LRT_ULNX
net_lrt_sources += $(net_lrt_ulnx_sources)
else
if LRT_BARE
net_lrt_sources += $(net_lrt_bare_sources)
endif
endif
