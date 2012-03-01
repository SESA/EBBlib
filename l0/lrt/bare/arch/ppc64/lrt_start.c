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

#include <l0/lrt/bare/uart.h>
#include <l0/lrt/bare/arch/ppc64/lrt_start.h>
#include <l0/lrt/bare/arch/ppc64/mem.h>

/* load address of lrt_start and load toc then call it */
asm (
     ".globl lrt_start_isr\n\t"
     "lrt_start_isr:\n\t"
     "lis 20, lrt_start@highest\n\t"
     "ori 20, 20, lrt_start@higher\n\t"
     "rldicr 20, 20, 32, 31\n\t"
     "oris 20, 20, lrt_start@h\n\t"
     "ori 20, 20, lrt_start@l\n\t"
     "ld 2, 8(20)\n\t"
     "ld 20, 0(20)\n\t"
     "mtctr 20\n\t"
     "bctrl\n\t"
     "rfi"
     );

static char *uart_addr = (char *)0xffc000c000ULL;

void
lrt_start(void)
{  
  uart_write(uart_addr, "In int\n", 7);
  
  lrt_mem_init();

}


