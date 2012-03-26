OBJS := \
	init.o \
	isr.o \
	start.o

LDFLAGS := \
	-m32 \
	-static \
	-nodefaultlibs \
	-nostdlib \
	-T link.ld

CFLAGS := \
	-m32 \
	-ffreestanding \
	-std=gnu99 \
	-Wall \
	-Werror \
	-g3 

CPPFLAGS := \
	-I ../../../../../ \
	-D ARCH_PPC32 \
	-D LRT_BARE

ebbos: $(OBJS)
	gcc $(LDFLAGS) $(OBJS) -o $@ 

%.o: %.c
	gcc $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.S
	gcc $(CPPFLAGS) -D __ASSEMBLY__ $(CFLAGS) -c -o $@ $<

clean:
	-rm ebbos
	-rm $(OBJS)