IXPDIR=$(BUILDDIR)/libixp-0.5
CFLAGS:=-Wall -I$(IXPDIR)/include -D SESA_ARCH=$(SESA_ARCH) -D SESA_LRT=$(SESA_LRT)
LIBS:=-lpthread -L$(IXPDIR)/lib -lixp -lpcap
#CFLAGS := -O4 
CFLAGS += -g 
SRCS := CObjEBBRootShared.c CObjEBB.c CObjEBBRootMulti.c \
	EBBEventMgrPrimImp.c EBBMgrPrim.c EBBMemMgrPrim.c \
	EBBStart.c \
	EBBCtrPrim.c EBBCtrPrimDistributed.c \
	EthMgrPrim.c 

OBJS := $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))
OBJS += $(patsubst %.S, %.o, $(filter %.S, $(SRCS)))
# FIMXE: JA: KLUDGE FOR LRT STUFF 
OBJS += $(BUILDDIR)/lrt/pic.o $(BUILDDIR)/lrt/lrt_start.o \
	$(BUILDDIR)/lrt/ethlib.o

OBJS += $(BUILDDIR)/sys/arch/defFT.o
OBJS += $(BUILDDIR)/sys/defFT.o
DEPS := $(patsubst %.c, %.d, $(filter %.c, $(SRCS)))
DEPS += $(patsubst %.S, %.d, $(filter %.S, $(SRCS)))

# FIMXE: JA: KLUDGE FOR LRT STUFF 
DEPS += $(BUILDDIR)/lrt/pic.d $(BUILDDIR)/lrt/lrt_start.d \
	$(BUILDDIR)/lrt/ethlib.d

DEPS += $(BUILDDIR)/sys/arch/defFT.d
DEPS += $(BUILDDIR)/sys/defFT.d

ebbtest: $(OBJS) ebb.mk $(IXPDIR)/lib/libixp.a
	gcc $(CFLAGS) $(OBJS) $(LIBS) -o $@

create_build::
	@echo "Creating EBB directories"
	@mkdir $(BUILDDIR)/sys
	@mkdir $(BUILDDIR)/sys/arch
	@mkdir $(BUILDDIR)/lrt
	@echo "done Creating EBB directories"

# FIMXE: JA: KLUDGE FOR LRT STUFF 
# FIXME: don't know why this is not working
#$(BUILDDIR)/lrt/pic.o:$(SRCDIR)/ebb/lrt/$(SESA_LRT)/pic.c
$(BUILDDIR)/lrt/pic.o:$(SRCDIR)/ebb/lrt/ulnx/pic.c
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

#$(BUILDDIR)/lrt/lrt_start.o:$(SRCDIR)/ebb/lrt/$(SESA_LRT)/lrt_start.c
$(BUILDDIR)/lrt/lrt_start.o:$(SRCDIR)/ebb/lrt/ulnx/lrt_start.c
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

#$(BUILDDIR)/lrt/ethlib.o:$(SRCDIR)/ebb/lrt/$(SESA_LRT)/ethlib.c
$(BUILDDIR)/lrt/ethlib.o:$(SRCDIR)/ebb/lrt/ulnx/ethlib.c
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR)/sys/arch/defFT.o:$(SRCDIR)/ebb/sys/arch/$(SESA_ARCH)/defFT.S
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR)/sys/defFT.o:$(SRCDIR)/ebb/sys/defFT.c
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

$(IXPDIR)/lib/libixp.a:
	make -C $(IXPDIR)

-include $(DEPS)

%.o : %.c ebb.mk
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

%.o: %.S ebb.mk
	gcc $(CFLAGS) -MMD -MP -c $< -o $@


clean::
	@echo "make clean in ebbtest.mk"
	-rm $(wildcard $(OBJS) $(DEPS) ebbtest)
