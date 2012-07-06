#ifndef ARCH_AMD64_PMC_H
#define ARCH_AMD64_PMC_H

/*
 * Copyright (C) 2012 by Project SESA, Boston University
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

typedef struct {
  union {
    uint64_t val;
    struct {
      uint64_t eventselect_7_0 :8;
      uint64_t unitmask :8;
      uint64_t osusermode :2;
      uint64_t edge :1;
      uint64_t reserved3 :1;
      uint64_t enint :1;
      uint64_t reserved2 :1;
      uint64_t en :1;
      uint64_t inv :1;
      uint64_t cntmask :8;
      uint64_t eventselect_11_8 :4;
      uint64_t reserved1 :4;
      uint64_t hostguestonly :2;
      uint64_t reserved0 :22;
    } __attribute__((packed));
  };
} perf_event_select;

STATIC_ASSERT(sizeof(perf_event_select) == 8,
              "perf_event_select packing issue");

static const uint64_t PMC_CLOCKS_NOT_HALTED = 0x76;

#endif
