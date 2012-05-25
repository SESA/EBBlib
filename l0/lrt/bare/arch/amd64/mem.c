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

#include <l0/lrt/event.h>
#include <l0/lrt/mem.h>
#include <l0/lrt/bare/arch/amd64/init64.h>
#include <lrt/io.h>

extern char kend[];

struct lrt_mem_desc {
  char *start;
  char *current;
  char *end;
};

struct lrt_mem_desc *bootmem;

void
lrt_mem_preinit(int cores) {
  bootmem = (struct lrt_mem_desc *)kend;
  char *ptr = kend + (sizeof(struct lrt_mem_desc) * cores);
  uint64_t num_bytes = (((uint64_t)bootinfo->mem_upper) << 10) -
    ((uint64_t)ptr - 0x100000);
  for (int i = 0; i < cores; i++) {
    bootmem[i].start = bootmem[i].current = ptr;
    ptr += num_bytes / cores;
    bootmem[i].end = ptr;
  }
}

void *
lrt_mem_alloc(size_t size, size_t aligned, lrt_event_loc loc) {
  struct lrt_mem_desc *desc = &bootmem[loc];
  char *ptr = desc->current;
  //align up
  ptr = (char *)((((uintptr_t)ptr + aligned - 1) / aligned) * aligned);
  LRT_Assert((ptr + size) < desc->end);
  desc->current = ptr + size;
  return ptr;
}

uintptr_t
lrt_mem_start() {
  //we give the current pointer, all other memory
  // will never be freed
  return (uintptr_t)bootmem[lrt_my_event_loc()].current;
}

uintptr_t
lrt_mem_end() {
  return (uintptr_t)bootmem[lrt_my_event_loc()].end;
}

void lrt_mem_init() {
}
