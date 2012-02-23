l0_lrt_bare_arch_amd64_sources = \
	l0/lrt/bare/arch/amd64/init64.c \
	l0/lrt/bare/arch/amd64/init64.h \
	l0/lrt/bare/arch/amd64/isr_common.c \
	l0/lrt/bare/arch/amd64/isr.c \
	l0/lrt/bare/arch/amd64/isr.h \
	l0/lrt/bare/arch/amd64/isrtbl.c \
	l0/lrt/bare/arch/amd64/lrt_start.c \
	l0/lrt/bare/arch/amd64/lrt_start.h \
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

if LRT_BARE

#TODO DS: Should have some proper dependency tracking here
l0/lrt/bare/arch/amd64/init32.o: $(top_srcdir)/l0/lrt/bare/arch/amd64/init32.c
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(ebbos_CPPFLAGS) \
		$(CPPFLAGS) $(patsubst -m64, -m32, $(ebbos_CFLAGS)) \
		-Winline -c -o $@ $<
	objcopy -g -O elf64-x86-64 $@ $@

ebbos_DEPENDENCIES = $(top_srcdir)/l0/lrt/bare/arch/amd64/link.ld \
	l0/lrt/bare/arch/amd64/init32.o
ebbos_LDADD += l0/lrt/bare/arch/amd64/init32.o
CLEANFILES = l0/lrt/bare/arch/amd64/init32.o

endif

EXTRA_DIST = l0/lrt/bare/arch/amd64/link.ld l0/lrt/bare/arch/amd64/grub.cfg \
	l0/lrt/bare/arch/amd64/init32.c