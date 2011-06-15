-include Makeconf

CC ?= gcc

TOOLPREFIX ?=
AR ?= $(TOOLPREFIX)ar

SRCDIR := ../libs
L4HALDIR ?= ../../l4hal/

DEFINES ?= L4

INC_DIR := $(L4HALDIR)include

VPATH := $(SRCDIR)

SRCFILES := $(shell find $(SRCDIR) -type f -name "*.c")
SRCFILES := $(notdir $(SRCFILES))
HDRFILES := $(shell find $(SRCDIR) -type f -name "*.h")

OBJFILES := $(patsubst %.c, %.o, $(SRCFILES))
DEPFILES := $(patsubst %.c, %.d, $(SRCFILES))

WARNINGS := -Wall -Wshadow -Wpointer-arith -Wstrict-prototypes
CFLAGS += -g -O2 $(WARNINGS) $(addprefix -I,$(INC_DIR)) \
		$(addprefix -D,$(DEFINES))

EBBLIB := libebb.a

.PHONY: all clean

all: $(EBBLIB)

$(EBBLIB): $(OBJFILES)
	$(AR) r $@ $?

clean: 
	rm $(wildcard $(OBJFILES) $(DEPFILES) $(EBBLIB))

-include $(DEPFILES)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@