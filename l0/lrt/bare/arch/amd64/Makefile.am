l0_lrt_bare_arch_amd64_sources = \
	l0/lrt/bare/arch/amd64/acpi.c \
	l0/lrt/bare/arch/amd64/acpi.h \
	l0/lrt/bare/arch/amd64/crt.c \
	l0/lrt/bare/arch/amd64/event.c \
	l0/lrt/bare/arch/amd64/event.h \
	l0/lrt/bare/arch/amd64/init64.c \
	l0/lrt/bare/arch/amd64/init64.h \
	l0/lrt/bare/arch/amd64/isr.S \
	l0/lrt/bare/arch/amd64/lrt_start.c \
	l0/lrt/bare/arch/amd64/lrt_start.h \
	l0/lrt/bare/arch/amd64/mem.c \
	l0/lrt/bare/arch/amd64/mem.h \
	l0/lrt/bare/arch/amd64/serial.c \
	l0/lrt/bare/arch/amd64/serial.h \
	l0/lrt/bare/arch/amd64/start.S \
	l0/lrt/bare/arch/amd64/trans.c \
	l0/lrt/bare/arch/amd64/trans.h

if LRT_BARE
if x86_64

#TODO DS: Should have some proper dependency tracking here
l0/lrt/bare/arch/amd64/init32.o: $(top_srcdir)/l0/lrt/bare/arch/amd64/init32.c
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
		$(CPPFLAGS) $(patsubst -m64, -m32, $(AM_CFLAGS)) \
		$(CFLAGS) -Winline -c -o $@ $<
	objcopy -g -O elf64-x86-64 $@ $@

libebb_a_DEPENDENCIES += $(top_srcdir)/l0/lrt/bare/arch/amd64/link.ld \
	l0/lrt/bare/arch/amd64/init32.o
libebb_a_LIBADD += l0/lrt/bare/arch/amd64/init32.o
CLEANFILES += l0/lrt/bare/arch/amd64/init32.o

endif
endif

EXTRA_DIST += l0/lrt/bare/arch/amd64/link.ld l0/lrt/bare/arch/amd64/grub.cfg \
	l0/lrt/bare/arch/amd64/init32.c
