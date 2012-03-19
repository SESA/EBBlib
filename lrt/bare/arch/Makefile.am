include lrt/bare/arch/amd64/Makefile.am
include lrt/bare/arch/ppc64/Makefile.am

lrt_bare_arch_sources = \
	lrt/bare/arch/assert.h

if x86_64
lrt_bare_arch_sources += $(lrt_bare_arch_amd64_sources)
else
if powerpc64
lrt_bare_arch_sources += $(lrt_bare_arch_ppc64_sources)
endif
endif
