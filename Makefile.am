TESTS = scripts/regression

AM_CFLAGS = -Wall -Werror -fno-strict-aliasing -g3
if STD_GNU_99
AM_CFLAGS += -std=gnu99
endif

AM_CCASFLAGS = -D ASM_FILE -g3 -I$(top_srcdir) -I.

AM_CXXFLAGS = -Wall -Werror -Wno-non-virtual-dtor -g3
#for now no CPPFLAGS
AM_CPPFLAGS =
if STD_GNUXX_0x
AM_CXXFLAGS += -std=gnu++0x
endif

if x86_64
AM_CFLAGS += -m64
AM_CCASFLAGS += -m64
endif

if powerpc64
AM_CFLAGS += -m64
AM_CCASFLAGS += -m64
endif

if powerpc
AM_CFLAGS += -m32
AM_CCASFLAGS += -m32
endif

# Make these blank so that they can be added to
CLEANFILES = apps/last_res
EXTRA_DIST = scripts/regression
noinst_LIBRARIES =

lib_LIBRARIES = libebb.a

libebb_a_SOURCES = $(arch_sources) $(l0_sources) $(lrt_sources) \
  $(misc_sources) $(net_sources) $(l1_sources) $(io_sources) $(sync_sources)

libebb_a_LIBADD =
libebb_a_DEPENDENCIES =

ebbos_libs = libebb.a

if LRT_BARE

BAREFLAGS = -nostdlib -nodefaultlibs
AM_LDFLAGS = -u _start


if CXX_FFREESTANDING
BAREFLAGS += -ffreestanding
endif

if x86_64
BAREFLAGS += -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3
AM_LDFLAGS += -melf_x86_64 -static -n -z max-page-size=0x1000 \
	-Wl,--build-id=none \
	-T $(top_srcdir)/l0/lrt/bare/arch/amd64/link.ld

#TODO DS: configure should check for grub-mkrescue
%.iso: % $(top_srcdir)/l0/lrt/bare/arch/amd64/grub.cfg
	grub-mkrescue -o $@ -graft-points boot/ebbos=$< \
	boot/grub/grub.cfg=$(top_srcdir)/l0/lrt/bare/arch/amd64/grub.cfg

endif

if powerpc
BAREFLAGS += -mno-sdata
AM_LDFLAGS += -m32 -mno-sdata -static -nodefaultlibs -nostdlib \
	-Wl,-G,0 -T $(top_srcdir)/l0/lrt/bare/arch/ppc32/link.ld
LDADD = -lgcc
%.uimage: %.bin
	mkimage -A ppc -O linux -T kernel -C none -a 0 -e 0 -n "EbbOS" -d $< $@
%.bin: %
	objcopy -O binary $< $@
endif

AM_CFLAGS += $(BAREFLAGS)
AM_CXXFLAGS += $(BAREFLAGS) -fno-exceptions -fno-rtti -fno-stack-protector

endif

include apps/Makefile.am
include arch/Makefile.am
include l0/Makefile.am
include l1/Makefile.am
include lrt/Makefile.am
include misc/Makefile.am
include net/Makefile.am
include io/Makefile.am
include sync/Makefile.am
