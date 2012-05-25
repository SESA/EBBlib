include l0/lrt/bare/arch/amd64/Makefile.am
include l0/lrt/bare/arch/ppc32/Makefile.am

l0_lrt_bare_arch_sources = \
	l0/lrt/bare/arch/event.h \
	l0/lrt/bare/arch/event_loc.h \
	l0/lrt/bare/arch/mem.h \
	l0/lrt/bare/arch/pic.h \
	l0/lrt/bare/arch/trans.h


if x86_64
l0_lrt_bare_arch_sources += $(l0_lrt_bare_arch_amd64_sources)
endif

if powerpc
l0_lrt_bare_arch_sources += $(l0_lrt_bare_arch_ppc32_sources)
endif
