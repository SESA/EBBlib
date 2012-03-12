AM_CFLAGS = -std=gnu99 -Wall -Werror -g3
AM_CCASFLAGS = -m64 -g3 -D ASM_FILE
AM_CXXFLAGS = -std=gnu++0x -g3

if powerpc64
AM_CFLAGS += -m64
AM_CCASFLAGS += -m64
endif

if powerpc
AM_CFLAGS += -m32
AM_CCASFLAGS += -m32
endif

# Make these blank so that they can be added to
CLEANFILES =
EXTRA_DIST =
noinst_LIBRARIES =

lib_LIBRARIES = libebb.a

libebb_a_SOURCES = $(arch_sources) $(l0_sources) $(lrt_sources) \
  $(misc_sources) $(net_sources) $(l1_sources)

libebb_a_LIBADD = 
libebb_a_DEPENDENCIES = 

ebbos_libs = libebb.a

if LRT_BARE

BAREFLAGS = -m64 -ffreestanding -nostdlib -nodefaultlibs -g3 \
	-mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3
AM_CFLAGS += $(BAREFLAGS)
AM_CXXFLAGS += $(BAREFLAGS) -fno-exceptions -fno-rtti -fno-stack-protector
AM_LDFLAGS = -melf_x86_64 -static -n -z max-page-size=0x1000 \
	-Wl,--build-id=none \
	-T $(top_srcdir)/l0/lrt/bare/arch/amd64/link.ld

# #TODO DS: configure should check for grub-mkrescue
# ebbos.iso: ebbos $(top_srcdir)/l0/lrt/bare/arch/amd64/grub.cfg
# 	grub-mkrescue -o ebbos.iso -graft-points boot/ebbos=ebbos \
# 	boot/grub/grub.cfg=$(top_srcdir)/l0/lrt/bare/arch/amd64/grub.cfg

endif

include apps/Makefile.am
include arch/Makefile.am
include l0/Makefile.am
include l1/Makefile.am
include lrt/Makefile.am
include misc/Makefile.am
include net/Makefile.am
