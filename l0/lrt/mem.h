#ifndef __LRT_MEM_H__
#define __LRT_MEM_H__
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

#include <l0/lrt/event_loc.h>
extern void *lrt_mem_alloc(size_t size, size_t aligned,
                           lrt_event_loc loc);
extern uintptr_t lrt_mem_start(void);
extern uintptr_t lrt_mem_end(void);
extern void lrt_mem_preinit(int cores);
extern void lrt_mem_init(void);

// note, this is internal state used only by mem.c, put it here
// for sharing between the machine independant and machine specific mem
// allocation routines, should probably move to a different .h file
struct lrt_mem_desc {
  char *start;
  char *current;
  char *end;
};

extern struct lrt_mem_desc *bootmem;


#ifdef LRT_ULNX
#include <l0/lrt/ulnx/mem.h>
#else
#include <l0/lrt/bare/mem.h>
#endif

#endif
