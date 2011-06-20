SRCS := objtest.c test.c EBBMgrPrimitive.c
OBJS := $(patsubst %.c, %.o, $(SRCS))
DEPS := $(patsubst %.c, %.d, $(SRCS))

all: objtest test

objtest: objtest.o
	gcc objtest.o -o $@

test: $(OBJS)
	gcc $(OBJS) -o $@

-include $(DEPS)

%.o: %.c Makefile
	gcc -MMD -MP -c $< -o $@