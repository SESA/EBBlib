#ifndef ARCH_AMD64_SEGMENTATION_H
#define ARCH_AMD64_SEGMENTATION_H

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

#include <lrt/assert.h>

typedef union {
  uint64_t raw;
  struct {
    uint64_t limit_low :16;
    uint64_t base_low :24;
    uint64_t type :4;
    uint64_t s :1;
    uint64_t dpl :2;
    uint64_t p :1;
    uint64_t limit_high	:4;
    uint64_t avl :1;
    uint64_t l :1;
    uint64_t d :1;
    uint64_t g :1;
    uint64_t base_high :8;
  };
} segdesc;

STATIC_ASSERT(sizeof(segdesc) == 8, "segdesc packing issue");

typedef union {
  uint64_t raw[2];
  struct {
    uint64_t limit_low :16;
    uint64_t base_low :24;
    uint64_t type :4;
    uint64_t s :1;
    uint64_t dpl :2;
    uint64_t p :1;
    uint64_t limit_high :4;
    uint64_t avl :1;
    uint64_t l :1;
    uint64_t d :1;
    uint64_t g :1;
    uint64_t base_high :40;
    uint64_t reserved0 :8;
    uint64_t zero :5;
    uint64_t reserved1 :19;
  };
} tssdesc;

STATIC_ASSERT(sizeof(tssdesc) == 16, "tssdesc packing issue");

typedef struct {
  uint32_t reserved0;
  uint64_t rsp[3] __attribute__ ((packed));
  uint64_t reserved1;
  uint64_t ist[7] __attribute__ ((packed));
  uint64_t reserved2;
  uint16_t reserved3;
  uint16_t iopbm_offset;
} tss;

STATIC_ASSERT(sizeof(tss) == 104, "tss packing issue");

typedef struct {
  uint16_t limit;
  uintptr_t base __attribute__((packed));
} gdtr;

STATIC_ASSERT(sizeof(gdtr) == (2 + sizeof(uintptr_t)), "gdtr packing issue");

static inline void
load_gdtr(segdesc *base, uint16_t limit)
{
  gdtr gdtr;
  gdtr.limit = limit - 1; //limit is length - 1
  gdtr.base = (uintptr_t)base;
  __asm__ volatile (
                    "lgdt %[gdtr]"
                    :
                    : [gdtr] "m" (gdtr), "m" (*base) //stupid gcc
                    );
}

#endif
