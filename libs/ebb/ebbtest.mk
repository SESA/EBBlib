CFLAGS := -O4
SRCS := EBBMgrPrim.c CObjEBBRootShared.c CObjEBB.c \
	sys/defFT.c 
OBJS := $(patsubst %.c, %.o, $(SRCS))
DEPS := $(patsubst %.c, %.d, $(SRCS))

all: ebbtest

ebbtest: ebbtest.o clrBTB.o EBBCtrPrim.o $(OBJS) sys/arch/amd64/defFT.o
	gcc $(CFLAGS) ebbtest.o clrBTB.o EBBCtrPrim.o $(OBJS) sys/arch/amd64/defFT.o -o $@

objctest: objctest.m clrBTB.o $(OBJS) sys/arch/amd64/defFT.o
	gcc-mp-4.4 -fgnu-runtime $(CFLAGS) objctest.m clrBTB.o $(OBJS) -lobjc sys/arch/amd64/defFT.o -o $@ 

clrBTB.o: jmps.S clrBTB.S
	gcc -c clrBTB.S

clrBTB.S: jmps.S
	touch clrBTB.S

jmps.S:
	./mkjmps 1024 > jmps.S

-include $(DEPS)

%.o: %.c Makefile
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

sys/arch/amd64/defFT.o: sys/arch/amd64/defFT.S
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm $(OBJS) $(DEPS) sys/arch/amd64/defFT.o sys/arch/amd64/defFT.d ebbtest
