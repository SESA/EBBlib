#ifndef ARCH_AMD64_IDT_H
#define ARCH_AMD64_IDT_H

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

typedef union {
  uint64_t raw[2];
  struct {
    uint64_t offset_low :16;
    uint64_t selector :16;
    uint64_t ist :3;
    uint64_t zero :5;
    uint64_t type :4;
    uint64_t s :1;
    uint64_t dpl :2;
    uint64_t p :1;
    uint64_t offset_high :48 __attribute__((packed));
    uint64_t reserved :32;
  };
} idtdesc;

_Static_assert(sizeof(idtdesc) == 16, "idtdesc packing issue");

typedef struct {
  uint16_t limit;
  uint64_t base __attribute__((packed));
} idtr;

_Static_assert(sizeof(idtr) == 10, "idtr packing issue");

static inline void
load_idtr(idtdesc *base, uint16_t limit)
{
  idtr idtr;
  idtr.limit = limit - 1; //limit is length -1
  idtr.base = (uint64_t)base;
  __asm__ volatile (
		    "lidt %[idtr]"
		    :
		    : [idtr] "m" (idtr), "m" (*base)
		    );
}

#endif
