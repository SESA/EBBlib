#ifndef ARCH_POWERPC_450_MMU_H
#define ARCH_POWERPC_450_MMU_H

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

static const uint8_t TLB_1K = 0;
static const uint8_t TLB_4K = 1;
static const uint8_t TLB_16K = 2;
static const uint8_t TLB_64K = 3;
static const uint8_t TLB_256K = 4;
static const uint8_t TLB_1M = 5;
static const uint8_t TLB_16M = 6;
static const uint8_t TLB_256M = 9;
static const uint8_t TLB_1G = 10;
static const uint32_t SUPPORTED_TLB_PAGE_SIZE =
  1 << 0  | // 1K
  1 << 1  | // 4K
  1 << 2  | // 16K
  1 << 3  | // 64K
  1 << 4  | // 256K
  1 << 5  | // 1M
  1 << 6  | // 16M
  1 << 9  | // 256M
  1 << 10;  // 1G



typedef union {
  uint32_t val;
  struct {
    uint32_t epn :22;
    uint32_t v :1;
    uint32_t ts :1;
    uint32_t size :4;
    uint32_t tpar :4;
  };
} tlb_word_0;

STATIC_ASSERT(sizeof(tlb_word_0) == 4, "tlb_word_0 packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t rpn :22;
    uint32_t par1 :2;
    uint32_t reserved :4;
    uint32_t erpn :4;
  };
} tlb_word_1;

STATIC_ASSERT(sizeof(tlb_word_1) == 4, "tlb_word_1 packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t par2 :2;
    uint32_t reserved0 :8;
    uint32_t far :1;
    uint32_t wl1 :1;
    uint32_t il1i :1;
    uint32_t il1d :1;
    uint32_t il2i :1;
    uint32_t il2d :1;
    uint32_t u0 :1;
    uint32_t u1 :1;
    uint32_t u2 :1;
    uint32_t u3 :1;
    uint32_t w :1;
    uint32_t i :1;
    uint32_t m :1;
    uint32_t g :1;
    uint32_t e :1;
    uint32_t reserved1 :1;
    uint32_t ux :1;
    uint32_t uw :1;
    uint32_t ur :1;
    uint32_t sx :1;
    uint32_t sw :1;
    uint32_t sr :1;
  };
} tlb_word_2;

STATIC_ASSERT(sizeof(tlb_word_2) == 4, "tlb_word_2 packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t reserved0 :6;
    uint32_t l2swoa :1;
    uint32_t swoa :1;
    uint32_t reserved1 :1;
    uint32_t u1te :1;
    uint32_t u2swoae :1;
    uint32_t u3l2swoae :1;
    uint32_t dulxe :1;
    uint32_t iulxe :1;
    uint32_t reserved2 :1;
    uint32_t sts :1;
    uint32_t reserved3 :8;
    uint32_t stid :8;
  };
} mmucr;

STATIC_ASSERT(sizeof(mmucr) == 4, "mmucr packing issue");
    
#endif
