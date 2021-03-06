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

#include <stdint.h>
#include <l0/lrt/ulnx/pic.h>
#include <l0/lrt/ulnx/mem.h>

#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

enum { LRT_MEM_PAGESIZE=4096, LRT_MEM_PAGESPERPIC=1024 };
enum { LRT_MEM_PERPIC=LRT_MEM_PAGESIZE * LRT_MEM_PAGESPERPIC };

struct BootMemDesc {
  uintptr_t start;
  uintptr_t end;
} bootMem[LRT_PIC_MAX_PICS];

uintptr_t 
lrt_mem_start(void)
{
  return bootMem[lrt_pic_myid].start;
}

uintptr_t 
lrt_mem_end(void)
{
  return bootMem[lrt_pic_myid].end;
}

intptr_t
lrt_mem_init(void)
{
  struct BootMemDesc *bm = &(bootMem[lrt_pic_myid]);
  bm->start = (intptr_t)mmap(NULL, LRT_MEM_PERPIC, 
			     PROT_READ|PROT_WRITE|PROT_EXEC, 
			     MAP_ANON|MAP_PRIVATE, -1, 0);     
  if (bm->start == (intptr_t)MAP_FAILED) {
    perror(__func__);
    printf("%d\n", errno);
    assert(0);
  }
  bm->end = bm->start + LRT_MEM_PERPIC;
  return 1;
}

