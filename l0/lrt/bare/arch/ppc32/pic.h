#ifndef L0_LRT_BARE_ARCH_PPC32_PIC_H
#define L0_LRT_BARE_ARCH_PPC32_PIC_H

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

#include <arch/powerpc/regs.h>

extern uintptr_t lrt_pic_myid;

typedef void *lrt_pic_handler; //a PC
typedef uintptr_t lrt_pic_src;

extern void lrt_pic_mapvec(uintptr_t vec, lrt_pic_handler h);
extern void lrt_pic_ipi(uintptr_t id);
extern void __attribute__ ((noreturn)) lrt_pic_init(lrt_pic_handler h);
extern uintptr_t lrt_pic_getnumlpics(void); /* get number of logical pics, i.e., cores */
extern uintptr_t lrt_pic_getnextlpic(uintptr_t c); /* get next pic in some sequence; loops */
extern void lrt_pic_ackipi(void);
extern void lrt_pic_enableipi(void);

#endif
