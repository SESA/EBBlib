SRCS := cobjtest.c 
OBJS := $(patsubst %.c, %.o, $(SRCS))
DEPS := $(patsubst %.c, %.d, $(SRCS))

all: cobjtest

cobjtest: $(OBJS)
	gcc $(OBJS) -o $@

test: $(OBJS)
	gcc $(OBJS) -o $@

-include $(DEPS)

%.o: %.c Makefile
	gcc -MMD -MP -c $< -o $@