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
#include "gdt.h"

typedef struct gdt_ptr gdt_ptr;
struct gdt_ptr{
  uint16_t size;
  uint32_t offset;
}__attribute__((packed));

typedef struct gdt_entry gdt_entry;
struct gdt_entry{
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t access;
  uint8_t limit_high_flags;
  uint8_t base_high;
}__attribute__((packed));

void gdt_set_entry(gdt_entry *entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags){
  entry->limit_low = limit & 0xffff;
  entry->base_low = base & 0xffff;
  entry->base_mid = (base >> 16) & 0xff;
  entry->access = access|(1<<4);
  entry->limit_high_flags = (limit >> 16) & 0x0f;
  entry->limit_high_flags |= flags & 0xf0;
  entry->base_high = base >> 24;
}

#define NUM_GDT_CONST 3
static const uint64_t NUM_GDT = NUM_GDT_CONST;
gdt_ptr gdt_p;
gdt_entry gdt[NUM_GDT_CONST];

/* Access bits */
static const uint64_t ACC_PR = (1<<7); /* present */
static inline uint64_t ACC_RING(uint64_t x) { return (x<<5); } /* sets Privl bits - should be 0 <= x <= 3 */
static const uint64_t ACC_EX = (1<<3); 
static const uint64_t ACC_DC = (1<<2);
static const uint64_t ACC_READ = (1<<1); /* readable */
static const uint64_t ACC_AC = (1<<0); /* accesssed bit - do not set manually. */

/* Flags */
static const uint64_t FLAG_G = (1<<7); /* page granularity */
static const uint64_t FLAG_D = (1<<6); /* 32 bit proteced mode */
static const uint64_t FLAG_L = (1<<5); /* longmode */

/* actually loads the gdt. */
static inline void gdt_load(gdt_ptr *p) {
  __asm__ volatile ("lgdt %0\n"
                    "movw %1, %%ds\n"
                    "movw %1, %%es\n"
                    "movw %1, %%fs\n"
                    "movw %1, %%gs\n"
                    "movw %1, %%ss\n",
                    "jmpl %2\n"
                    :: "r"(*p),
                    "r"((uint16_t)(2*sizeof(gdt_entry))),
                    "r"((uint16_t)(1*sizeof(gdt_entry))));
}

void gdt_init(void){
  gdt_p.size = NUM_GDT * sizeof(gdt_entry) - 1;
  gdt_p.offset = (uint32_t)gdt;

  gdt_set_entry(&gdt[0], 0, 0, 0, 0); /* mandatory null entry */
  gdt_set_entry(&gdt[1], 0, ~0, ACC_PR|ACC_RING(0)|ACC_READ|ACC_EX, FLAG_L); /* code segment */
  gdt_set_entry(&gdt[2], 0, ~0, ACC_PR|ACC_RING(0)|ACC_READ, FLAG_L); /* data segment */

  gdt_load(&gdt_p);
}
