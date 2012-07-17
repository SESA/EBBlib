include arch/amd64/Makefile.am
include arch/powerpc/Makefile.am

arch_sources = \
	arch/args.h \
	arch/cpu.h

if x86_64
arch_sources += $(arch_amd64_sources)
endif

if powerpc
arch_sources += $(arch_powerpc_sources)
endif
