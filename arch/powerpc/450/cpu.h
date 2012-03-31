#ifndef ARCH_POWERPC_450_CPU_H
#define ARCH_POWERPC_450_CPU_H

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

//From PPC450 CPU Core with L2 Cache User's Manual
// October 22, 2004. page 213
typedef union {
  uint32_t val;
  struct {
    uint32_t reserved0 :13;
    uint32_t we :1; //wait state enable
    uint32_t ce :1; //critical enable
    uint32_t reserved1 :1;
    uint32_t ee :1; //external enable
    uint32_t pr :1; //problem state
    uint32_t fp :1; //floating point avilable
    uint32_t me :1; //machine check enable
    uint32_t fe0 :1; //FP Exception Mode 0
    uint32_t dwe :1; //debug wait enable
    uint32_t de :1; //debug interrupt enable
    uint32_t fe1 :1; //FP Exception Mode 1
    uint32_t reserved2 :2;
    uint32_t is :1; //instruction address space
    uint32_t ds :1; //data address space
    uint32_t reserved3 :4;
  };
} msr;

STATIC_ASSERT(sizeof(msr) == 4, "msr packing issue");


#endif
