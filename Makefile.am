include arch/Makefile.am
include l0/Makefile.am
include lrt/Makefile.am
include misc/Makefile.am
include net/Makefile.am

AM_CFLAGS = -std=gnu99 -Wall -Werror -g3
AM_CCASFLAGS = -m64 -g3 -D ASM_FILE
AM_CXXFLAGS = -std=gnu++0x -g3

noinst_PROGRAMS = ebbos
ebbos_CFLAGS = $(AM_CFLAGS)
ebbos_CXXFLAGS = $(AM_CXXFLAGS)

if x86_64
AM_CPPFLAGS = -D ARCH_AMD64
else
if powerpc64
AM_CPPFLAGS = -D ARCH_PPC64
endif
endif

if LRT_ULNX
ebbos_SOURCES = $(arch_sources) $(l0_sources) $(lrt_sources) \
	$(misc_sources) $(net_sources)
# ebbos_SOURCES = EBBStart.c $(arch_sources) $(l0_sources) $(lrt_sources) \
# 	$(misc_sources) $(net_sources)
ebbos_CPPFLAGS = $(AM_CPPFLAGS) -D LRT_ULNX
else
if LRT_BARE
ebbos_SOURCES = $(arch_sources) $(l0_lrt_sources) $(lrt_sources) \
		l0/sys/trans.h \
		l0/sys/trans.c \
		l0/sys/defFT.c \
		l0/sys/arch/amd64/defFT.S 


#ebbos_DEPENDENCIES = $(top_srcdir)/l0/lrt/bare/arch/amd64/link.ld \
	l0/lrt/bare/arch/amd64/init32.o
ebbos_CPPFLAGS = $(AM_CPPFLAGS) -D LRT_BARE
BAREFLAGS = -m64 -ffreestanding -nostdlib -nodefaultlibs -g3 \
	-O2 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3
ebbos_CFLAGS += $(BAREFLAGS)
ebbos_CXXFLAGS += $(BAREFLAGS)
ebbos_LDFLAGS = -melf_x86_64 -static -n -z max-page-size=0x1000 \
	-Wl,--build-id=none \
	-T $(top_srcdir)/l0/lrt/bare/arch/amd64/link.ld
#TODO DS: configure should check for grub-mkrescue
ebbos.iso: ebbos $(top_srcdir)/l0/lrt/bare/arch/amd64/grub.cfg
	grub-mkrescue -o ebbos.iso -graft-points boot/ebbos=ebbos \
	boot/grub/grub.cfg=$(top_srcdir)/l0/lrt/bare/arch/amd64/grub.cfg

# ebbos_SOURCES = \
# 		arch/amd64/apic.h \
# 		arch/amd64/asm.h \
# 		arch/amd64/cpu.h \
# 		arch/amd64/fpr.h \
# 		arch/amd64/idt.h \
# 		arch/amd64/multiboot.h \
# 		arch/amd64/paging.h \
# 		arch/amd64/pic.h \
# 		arch/amd64/pit.h \
# 		arch/amd64/rtc.h \
# 		arch/amd64/segmentation.h \
# 		arch/amd64/sysio.h \
# 		l0/lrt/bare/pic.h \
# 		l0/lrt/bare/trans.h \
# 		l0/lrt/bare/arch/amd64/init64.c \
# 		l0/lrt/bare/arch/amd64/init64.h \
# 		l0/lrt/bare/arch/amd64/isr_common.c \
# 		l0/lrt/bare/arch/amd64/isr.c \
# 		l0/lrt/bare/arch/amd64/isr.h \
# 		l0/lrt/bare/arch/amd64/isrtbl.c \
# 		l0/lrt/bare/arch/amd64/lrt_start.c \
# 		l0/lrt/bare/arch/amd64/lrt_start.h \
# 		l0/lrt/bare/arch/amd64/mem.c \
# 		l0/lrt/bare/arch/amd64/mem.h \
# 		l0/lrt/bare/arch/amd64/pic.c \
# 		l0/lrt/bare/arch/amd64/pic.h \
# 		l0/lrt/bare/arch/amd64/serial.c \
# 		l0/lrt/bare/arch/amd64/serial.h \
# 		l0/lrt/bare/arch/amd64/start.S \
# 		l0/lrt/bare/arch/amd64/stdio.c \
# 		l0/lrt/bare/arch/amd64/stdio.h \
# 		l0/lrt/bare/arch/amd64/string.c \
# 		l0/lrt/bare/arch/amd64/string.h \
# 		l0/lrt/bare/arch/amd64/trans.c \
# 		l0/lrt/bare/arch/amd64/trans.h \
# 		l0/sys/trans.h \
# 		l0/sys/trans.c \
# 		l0/sys/defFT.c \
# 		l0/sys/arch/amd64/defFT.S \
# 		lrt/bare/assert.h \
# 		lrt/bare/io.h
# ebbos_LDADD = l0/lrt/bare/arch/amd64/init32.o
# EXTRA_DIST = l0/lrt/bare/arch/amd64/link.ld l0/lrt/bare/arch/amd64/grub.cfg \
# 	l0/lrt/bare/arch/amd64/init32.c

# CLEANFILES = l0/lrt/bare/arch/amd64/init32.o
endif
endif