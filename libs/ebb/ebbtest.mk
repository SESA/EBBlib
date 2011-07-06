CFLAGS := -g 
SRCS := ebbtest.c EBBMgrPrim.c EBBCtrPrim.c CObjEBBRootShared.c CObjEBB.c \
	sys/defFT.c 
OBJS := $(patsubst %.c, %.o, $(SRCS))
DEPS := $(patsubst %.c, %.d, $(SRCS))

all: ebbtest

ebbtest: $(OBJS) sys/arch/amd64/defFT.o
	gcc $(CFLAGS) $(OBJS) sys/arch/amd64/defFT.o -o $@

-include $(DEPS)

%.o: %.c Makefile
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

sys/arch/amd64/defFT.o: sys/arch/amd64/defFT.S
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm $(OBJS) $(DEPS) sys/arch/amd64/defFT.o sys/arch/amd64/defFT.d ebbtest
