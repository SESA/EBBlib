OBJS := \
	../../stdio.o \
	../../string.o \
	init.o \
	isr.o \
	mailbox.o \
	start.o

LDFLAGS := \
	-m32 \
	-mno-sdata \
	-static \
	-nodefaultlibs \
	-nostdlib \
	-T link.ld \
	-Wl,-G,0

LIBS := \
	-lgcc

CFLAGS := \
	-m32 \
	-mno-sdata \
	-ffreestanding \
	-std=gnu99 \
	-Wall \
	-Werror \
	-g3 

ASFLAGS := \
	-mno-sdata \
	-Wall \
	-Werror \
	-g3

CPPFLAGS := \
	-I ../../../../../ \
	-D ARCH_PPC32 \
	-D LRT_BARE

ebbos.uimage: ebbos.bin
	mkimage -A ppc -O linux -T kernel -C none -a 0 -e 0 -n "EbbOS" -d $< $@

ebbos.bin: ebbos
	objcopy -O binary $< $@

ebbos: $(OBJS) link.ld Makefile
	gcc $(LDFLAGS) $(OBJS) $(LIBS) -o $@ 

%.o: %.c Makefile
	gcc $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.S Makefile
	gcc $(CPPFLAGS) -D __ASSEMBLY__ $(CFLAGS) -c -o $@ $<

clean:
	-rm ebbos.uimage
	-rm ebbos.bin
	-rm ebbos
	-rm $(OBJS)