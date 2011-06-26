SRCS := cobjtest.c 
OBJS := $(patsubst %.c, %.o, $(SRCS))
DEPS := $(patsubst %.c, %.d, $(SRCS))
CFLAGS := -g -Wall -pendantic

all: cobjtest

cobjtest: $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $@

-include $(DEPS)

%.o: %.c Makefile
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm $(OBJS) $(DEPS) cobjtest
