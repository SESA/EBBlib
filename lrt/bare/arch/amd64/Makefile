# Copyright (C) 2011 by Project SESA, Boston University
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

ASFLAGS = -m64 -g3 -D ASM_FILE
CPPFLAGS = -I ../../../..
CFLAGS = -m64 -ffreestanding -Wall -Werror -nostdlib -nodefaultlibs -g3 -std=c99 -O2 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3
LDFLAGS = -melf_x86_64 -static -n -z max-page-size=0x1000

OBJS = \
	start.o\
	init32.o\
	init64.o\
	stdio.o\
	serial.o\
	string.o\
	isr.o\
	isr_common.o\
	isrtbl.o

kernel.bin: $(OBJS) Makefile link.ld
	$(LD) $(LDFLAGS) -o $@ -Tlink.ld $(OBJS)

clean:
	rm -f *.o kernel.bin iso/boot/kernel.bin bootable.iso

# compile as 32 bit and stuff into elf64
# we turn on Winline because if something isn't inlined then we call
# into code in a different section which may cause problems
init32.o: init32.c Makefile
	$(CC) $(CPPFLAGS) $(patsubst -m64, -m32, $(CFLAGS)) -Winline \
		-c -o $@ $<
	objcopy -g -O elf64-x86-64 $@ $@

%.o: %.c Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
%.o: %.S Makefile
	$(CC) $(CPPFLAGS) $(ASFLAGS) -c -o $@ $<
bootable.iso: kernel.bin
	cp kernel.bin iso/boot/
	grub-mkrescue -o bootable.iso iso

	
