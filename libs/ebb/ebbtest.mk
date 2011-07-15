CFLAGS := -O4
SRCS := EBBMgrPrim.c CObjEBBRootShared.c CObjEBB.c \
	sys/defFT.c 
OBJS := $(patsubst %.c, %.o, $(SRCS))
DEPS := $(patsubst %.c, %.d, $(SRCS))

all: ebbtest

ebbtest: ebbtest.o EBBCtrPrim.o $(OBJS) sys/arch/amd64/defFT.o
	gcc $(CFLAGS) ebbtest.o EBBCtrPrim.o $(OBJS) sys/arch/amd64/defFT.o -o $@

objctest: objctest.m $(OBJS) sys/arch/amd64/defFT.o
	gcc-mp-4.4 -fgnu-runtime $(CFLAGS) objctest.m $(OBJS) -lobjc sys/arch/amd64/defFT.o -o $@ 

-include $(DEPS)

%.o: %.c Makefile
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

sys/arch/amd64/defFT.o: sys/arch/amd64/defFT.S
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm $(OBJS) $(DEPS) sys/arch/amd64/defFT.o sys/arch/amd64/defFT.d ebbtest
