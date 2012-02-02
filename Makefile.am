AM_CFLAGS = -Wall -Werror
AM_CCASFLAGS = -m64 -g3 -D ASM_FILE

include arch/Makefile.am
include l0/Makefile.am
include lrt/Makefile.am
include misc/Makefile.am
include net/Makefile.am

bin_PROGRAMS = ebbtest
ebbtest_SOURCES = EBBStart.c $(arch_sources) $(l0_sources) $(lrt_sources) \
	$(misc_sources) $(net_sources)
ebbtest_CPPFLAGS = -D LRT_ULNX

noinst_PROGRAMS = ebbos
ebbos_DEPENDENCIES = $(top_srcdir)/l0/lrt/bare/arch/amd64/link.ld \
	l0/lrt/bare/arch/amd64/init32.o
ebbos_CPPFLAGS = -D LRT_BARE
ebbos_CFLAGS = -m64 -ffreestanding -Wall -Werror -nostdlib -nodefaultlibs -g3 \
	-std=gnu99 -O2 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3
ebbos_LDFLAGS = -melf_x86_64 -static -n -z max-page-size=0x1000 \
	-Wl,--build-id=none \
	-T $(top_srcdir)/l0/lrt/bare/arch/amd64/link.ld
ebbos_SOURCES = l0/lrt/bare/arch/amd64/init64.c \
		l0/lrt/bare/arch/amd64/isr_common.c \
		l0/lrt/bare/arch/amd64/isr.c \
		l0/lrt/bare/arch/amd64/isr.h \
		l0/lrt/bare/arch/amd64/isrtbl.c \
		l0/lrt/bare/arch/amd64/lrt_start.c \
		l0/lrt/bare/arch/amd64/lrt_start_isr.S \
		l0/lrt/bare/arch/amd64/lrt_start_isr.h \
		l0/lrt/bare/arch/amd64/mem.c \
		l0/lrt/bare/arch/amd64/mem.h \
		l0/lrt/bare/arch/amd64/pic.c \
		l0/lrt/bare/arch/amd64/pic.h \
		l0/lrt/bare/arch/amd64/serial.c \
		l0/lrt/bare/arch/amd64/serial.h \
		l0/lrt/bare/arch/amd64/start.S \
		l0/lrt/bare/arch/amd64/stdio.c \
		l0/lrt/bare/arch/amd64/stdio.h \
		l0/lrt/bare/arch/amd64/string.c \
		l0/lrt/bare/arch/amd64/string.h \
		l0/lrt/bare/arch/amd64/trans.c \
		l0/lrt/bare/arch/amd64/trans.h 
ebbos_LDADD = l0/lrt/bare/arch/amd64/init32.o

#TODO DS: configure should check for grub-mkrescue
ebbos.iso: ebbos $(top_srcdir)/l0/lrt/bare/arch/amd64/grub.cfg
	grub-mkrescue -o ebbos.iso -graft-points boot/ebbos=ebbos \
	boot/grub/grub.cfg=$(top_srcdir)/l0/lrt/bare/arch/amd64/grub.cfg

#TODO DS: Should have some proper dependency tracking here
l0/lrt/bare/arch/amd64/init32.o: $(top_srcdir)/l0/lrt/bare/arch/amd64/init32.c
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(ebbos_CPPFLAGS) \
		$(CPPFLAGS) $(patsubst -m64, -m32, $(ebbos_CFLAGS)) \
		-Winline -c -o $@ $<
	objcopy -g -O elf64-x86-64 $@ $@