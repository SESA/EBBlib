#ifndef __LRT_ULNX_PIC_UNIX_H__
#define __LRT_ULNX_PIC_UNIX_H__
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

/* *********************************************************************** 

 *  Module Purpose: pic-unix implements the functionality to implement
 *    a pic on top of cabilities provided by a standard unix system.

 * ***********************************************************************/

/*
 * Vectors are defined by the HW platform
 * To simplify debugging, we will allocate FDs such that
 * there is a one-to-one mapping between fd and vector. 
 */

#ifdef __APPLE__
enum {FIRST_VECFD = 128};
//enum {FIRST_VECFD = 16};
#else
enum {FIRST_VECFD = 16};
#endif
// reserve 2 : 1 for ipi and 1 for reset

enum {NUM_RES_VEC = 2};
// make sure interrupt vector below supports this number of bits
// enum {NUM_MAPPABLE_VEC = FD_SETSIZE - (FIRST_VECFD + 1)  - NUM_RES_VEC };
enum {NUM_MAPPABLE_VEC = 32};
enum {RES0_VEC = (NUM_MAPPABLE_VEC)};
enum {RES1_VEC = (RES0_VEC + 1)};
enum {IPI_VEC = (RES0_VEC)};
enum {RST_VEC = (RES1_VEC)};
enum {NUM_VEC = (NUM_MAPPABLE_VEC + NUM_RES_VEC)};

typedef fd_set lrt_pic_unix_ints;

static inline void lrt_pic_unix_ints_clear(lrt_pic_unix_ints *s) {
  FD_ZERO(s); 
}
static inline void lrt_pic_unix_ints_set(lrt_pic_unix_ints *s, unsigned i) {
  FD_SET(i,s); 
}
static inline void lrt_pic_unix_ints_remove(lrt_pic_unix_ints *s, unsigned i) { 
FD_CLR(i,s); 
}

static inline uintptr_t lrt_pic_unix_ints_test(lrt_pic_unix_ints *s, unsigned i) { 
  return FD_ISSET(i,s); 
}

void lrt_pic_unix_wakeup(uintptr_t lcore);
int lrt_pic_unix_init();

uintptr_t lrt_pic_unix_addcore(void *(*routine)(void *), void *arg);	
int lrt_pic_unix_locked_map(lrt_pic_src *src, uintptr_t vec);
int lrt_pic_unix_locked_enable(uintptr_t vec);
int lrt_pic_unix_locked_disable(uintptr_t vec);

// blocks for interrupt, on success returns number interrupts 
// and bitvector of pending interrupts
int lrt_pic_unix_blockforinterrupt(lrt_pic_unix_ints *s);

// FIXME: get rid of this
uintptr_t lrt_pic_unix_getlcoreid();	/* for base thread FIXME, get rid off */

#endif
