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

#include <config.h>

#include <stdint.h>
#include <stdbool.h>

#include <l0/l0_start.h>
#include <l0/lrt/bare/arch/amd64/lrt_start.h>
#include <l0/lrt/bare/arch/amd64/mem.h>
#include <l0/lrt/bare/arch/amd64/pic.h>
#include <l0/lrt/bare/arch/amd64/stdio.h>
#include <l0/lrt/bare/arch/amd64/trans.h>
#include <l0/sys/trans.h>
#include <lrt/assert.h>
#include <lrt/io.h>

//We get here after some very early initialization occurs:
// 1. grub boots us into start.S, we put ourselves on a small boot
//     stack and call init32 in init32.c
// 2. init32.c sets up enough paging to idempotently map the first
//     4 GB of memory, enables long mode and the GDT then jumps to init64
//     which is in a 64 bit code segment in init64.c
// 3. init64.c initializes the "pic" and sends an ipi to ourself which goes
//     through lrt_start_isr.S and then gets here

//We assume the early boot stack is enough until later on when, for example,
// the event manager gets us on an event and an associated stack. 
void 
lrt_start(void)
{
  printf("lrt_start called!\n");

  lrt_mem_init();
  lrt_trans_init();

  l0_start(0);
  
  //Because we get here on an IPI, we need to send an eoi before returning
  // to the assembly which does our iretq
  lrt_pic_ackipi();
}

uintptr_t
lrt_startinfo_addr()
{
  EBB_LRT_printf("%s: NYI", __func__);
  EBBAssert(0);
}

intptr_t
lrt_startinfo_size()
{
  EBB_LRT_printf("%s: NYI", __func__);
  EBBAssert(0);
}
