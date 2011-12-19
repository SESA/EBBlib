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

AS_32=as --32
CC_32=cc -m32
LD_32=ld -melf_i386
CFLAGS_32=\
	-ffreestanding\
	-Wall\
	-Wextra\
	-pedantic\
	-nostdlib\
	-g
LDFLAGS_32=\
	-Tlink.ld

TARG=kern
OFILES=\
	start.o\
	serial.o\
	lrt_start.o\
	fmt.o\
	premalloc.o

$(TARG): $(OFILES)
	$(LD_32) -o $@ $(LDFLAGS_32) $(OFILES)
clean:
	rm -f *.o $(TARG)
%.o: %.c
	$(CC_32) -c -o $@ $< $(CFLAGS_32)
%.o: %.s
	$(AS_32) -o $@ $< $(ASFLAGS_32)
