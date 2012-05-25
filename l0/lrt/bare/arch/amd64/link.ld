/*
 * Copyright (C) 2011 by Project SESA, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

OUTPUT_ARCH(i386:x86-64)
OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
        . = 0x00100000;
        kstart = .;

        .init32 :
        {
                /* the mb_header has to be in the first 8k
                 * of the file so I put it here.
                 */
                *(.mb_header);
                *(.init.startup32);
                *(.init.text32);
                *(.init.smp);
                _smp_end = .;
                *(.init.data32);
        }
        .text ALIGN (4K) :
        {
                *(.text);
                *(.gnu.linkonce.t*);
        }

        .rodata ALIGN (4K) :
        {
                start_ctors = .;
                *(.ctor*)
                end_ctors = .;

                *(.rodata*)
                *(.gnu.linkonce.r*)
        }
        .data ALIGN (4K) :
        {
                *(.data)
                *(.gnu.linkonce.d*)
        }

        .bss :
        {
                sbss = .;
                *(COMMON)
                *(.bss)
                *(.gnu.linkonce.b*)
                ebss = .;
        }

        /DISCARD/ :
        {
                /* *(.debug*); */
                *(.comment);
                *(.eh_frame);
                *(.note*);
        }
        kend = .;
}
