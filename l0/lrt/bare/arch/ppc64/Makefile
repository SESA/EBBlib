OBJS := \
	init.o \
	isr.o \
	lrt_start.o \
	mem.o \
	pic.o \
	start.o \
	stdio.o \
	string.o \
	trans.o \
	../../uart.o \
	../../../../sys/trans.o \
	../../../../sys/defFT.o \
	../../../../sys/arch/ppc64/defFT.o

LDFLAGS := \
	-m64 \
	-static \
	-nodefaultlibs \
	-nostdlib \
	-T link.ld

CFLAGS := \
	-m64 \
	-ffreestanding \
	-std=gnu99 \
	-Wall \
	-Werror \
	-g3 

CPPFLAGS := \
	-I ../../../../../ \
	-D ARCH_PPC64 \
	-D LRT_BARE

ebbos: $(OBJS)
	gcc $(LDFLAGS) $(OBJS) -o $@ 

%.o: %.c
	gcc $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.S
	gcc $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	-rm ebbos
	-rm $(OBJS)