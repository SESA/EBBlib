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
#include <stdlib.h>
#include <l0/lrt/mem.h>

#include <l0/lrt/event_loc.h>
#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

struct lrt_mem_desc *bootmem;

enum { LRT_MEM_PAGESIZE=4096, LRT_MEM_PAGESPERPIC=1024 };
enum { LRT_MEM_PERPIC=LRT_MEM_PAGESIZE * LRT_MEM_PAGESPERPIC };

uintptr_t 
lrt_mem_start(void)
{
  return (uintptr_t)bootmem[lrt_my_event_loc()].current;
}

uintptr_t 
lrt_mem_end(void)
{
  return (uintptr_t)bootmem[lrt_my_event_loc()].end;
}

static intptr_t
lrt_mem_init_loc(lrt_event_loc el)
{
  struct lrt_mem_desc *bm = &(bootmem[el]);
  bm->start = mmap(NULL, LRT_MEM_PERPIC, 
		   PROT_READ|PROT_WRITE|PROT_EXEC, 
		   MAP_ANON|MAP_PRIVATE, -1, 0);     
  if (bm->start == MAP_FAILED) {
    perror(__func__);
    printf("%d\n", errno);
    assert(0);
  }
  bm->end = bm->start + LRT_MEM_PERPIC;
  bm->current = bm->start;
  return 1;
}

void
lrt_mem_init(void)
{
  if (lrt_my_event_loc() != 0) { /* initialize zero core in preinit */
    lrt_mem_init_loc(lrt_my_event_loc());
  }
}

// called once on first core to initialize the array of descriptors
void 
lrt_mem_preinit(int cores)
{
  bootmem = malloc(sizeof(struct lrt_mem_desc) * cores);

  // now allocate for core 0 the memory to be used, since need
  // to allcoate memory for event_preinit, and don't know location by
  // normal mechanism until then. 
  lrt_mem_init_loc(0);
}
