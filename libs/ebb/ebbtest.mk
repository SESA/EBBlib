IXPDIR=$(BUILDDIR)/libixp-0.5
CFLAGS:=-Wall -I$(IXPDIR)/include -D SESA_ARCH=$(SESA_ARCH) -D SESA_LRT=$(SESA_LRT)
LIBS:=-lpthread -L$(IXPDIR)/lib -lixp
#CFLAGS := -O4 
CFLAGS += -g 
SRCS := EBBMgrPrim.c CObjEBBRootShared.c CObjEBB.c \
	EBBCtrPrimDistributed.c CObjEBBRootMulti.c \
	EBBMemMgrPrim.c EBBCtrPrim.c EBB9PClientPrim.c \
	EBB9PFilePrim.c P9FSPrim.c CmdMenuPrim.c ebbtest.c

OBJS := $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))
OBJS += $(patsubst %.S, %.o, $(filter %.S, $(SRCS)))
OBJS += $(BUILDDIR)/sys/arch/defFT.o
OBJS += $(BUILDDIR)/sys/defFT.o
DEPS := $(patsubst %.c, %.d, $(filter %.c, $(SRCS)))
DEPS += $(patsubst %.S, %.d, $(filter %.S, $(SRCS)))
DEPS += $(BUILDDIR)/sys/arch/defFT.d
DEPS += $(BUILDDIR)/sys/defFT.d

ebbtest: $(OBJS) ebbtest.mk $(IXPDIR)/lib/libixp.a
	gcc $(CFLAGS) $(OBJS) $(LIBS) -o $@

create_build::
	@echo "Creating EBB directories"
	@mkdir $(BUILDDIR)/sys
	@mkdir $(BUILDDIR)/sys/arch
	@echo "done Creating EBB directories"

objctest: objctest.m clrBTB.o $(OBJS) 
	gcc-mp-4.4 -fgnu-runtime $(CFLAGS) objctest.m clrBTB.o $(OBJS) -lobjc sys/arch/defFT.o -o $@ 

$(BUILDDIR)/sys/arch/defFT.o:$(SRCDIR)/ebb/sys/arch/$(SESA_ARCH)/defFT.S
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR)/sys/defFT.o:$(SRCDIR)/ebb/sys/defFT.c
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

$(IXPDIR)/lib/libixp.a:
	make -C $(IXPDIR)

clrBTB.o: jmps.S clrBTB.S
	gcc -c clrBTB.S

clrBTB.S: jmps.S
	touch clrBTB.S

jmps.S:
	./mkjmps 1024 > jmps.S

-include $(DEPS)

%.o : %.c ebbtest.mk
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

%.o: %.S ebbtest.mk
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

EBBObj.o: EBBObj.H EBBObj.c
	g++ $(CFLAGS) -c EBBObj.c -o EBBObj.o

clean::
	@echo "make clean in ebbtest.mk"
	-rm $(wildcard $(OBJS) $(DEPS) ebbtest)
