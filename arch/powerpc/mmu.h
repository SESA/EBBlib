#ifndef ARCH_PPC64_MMU_H
#define ARCH_PPC64_MMU_H

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

#ifdef ARCH_PPC32
#include <arch/powerpc/450/mmu.h>
#endif

typedef union {
  uint32_t val;
  struct {
    uint32_t atsel :1;
    uint32_t reserved0 :12;
    uint32_t esel :3;
    uint32_t reserved1 :1;
    uint32_t hes :1;
    uint32_t wq :2;
    uint32_t reserved2 :12;
  };
} mas0;

STATIC_ASSERT(sizeof(mas0) == 4, "mas0 packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t v :1;
    uint32_t iprot :1;
    uint32_t tid :14;
    uint32_t reserved0 :2;
    uint32_t ind :1;
    uint32_t ts :1;
    uint32_t tsize :4;
    uint32_t reserved1 :8;
  };
} mas1;

STATIC_ASSERT(sizeof(mas1) == 4, "mas1 packing issue");

typedef union {
  uint64_t val;
  struct {
    uint64_t epn :52;
    uint64_t reserved :7;
    uint64_t w :1;
    uint64_t i :1;
    uint64_t m :1;
    uint64_t g :1;
    uint64_t e :1;
  };
} mas2;

STATIC_ASSERT(sizeof(mas2) == 8, "mas2 packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t rpnl :21;
    uint32_t reserved :1;
    uint32_t u0 :1;
    uint32_t u1 :1;
    uint32_t u2 :1;
    uint32_t u3 :1;
    uint32_t ux :1;
    uint32_t sx :1;
    uint32_t uw :1;
    uint32_t sw :1;
    uint32_t ur :1;
    uint32_t sr :1;
  };
} mas3;

STATIC_ASSERT(sizeof(mas3) == 4, "mas3 packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t sgs :1;
    uint32_t reserved :23;
    uint32_t slpid :8;
  };
} mas5;

STATIC_ASSERT(sizeof(mas5) == 4, "mas5 packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t reserved0 :2;
    uint32_t spid :14;
    uint32_t reserved1 :4;
    uint32_t isize :4;
    uint32_t reserved2 :6;
    uint32_t sind :1;
    uint32_t sas :1;
  };
} mas6;

STATIC_ASSERT(sizeof(mas6) == 4, "mas6 packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t reserved :22;
    uint32_t rpnu :10;
  };
} mas7;

STATIC_ASSERT(sizeof(mas7) == 4, "mas7 packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t reserved0 :17;
    uint32_t x :1;
    uint32_t r :1;
    uint32_t c :1;
    uint32_t ecl :1;
    uint32_t tid_nz :1;
    uint32_t class :2;
    uint32_t wlc :2;
    uint32_t resvattr :1;
    uint32_t reserved1 :1;
    uint32_t thdid :4;
  };
} mmucr3;

STATIC_ASSERT(sizeof(mmucr3) == 4, "mmucr3 packing issue");

#endif
