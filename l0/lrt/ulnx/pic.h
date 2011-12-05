#ifndef __LRT_ULNX_PIC_H__
#define __LRT_ULNX_PIC_H__
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

enum {LRT_PIC_LG2MAX_PICS = 7}; // 128 
enum {LRT_PIC_MAX_PICS = (1 << LRT_PIC_LG2MAX_PICS) };
enum {LRT_PIC_FIRST_PIC_ID = 0};
enum {LRT_PIC_MAX_PIC_ID = (LRT_PIC_MAX_PICS-1)};

// FIXME: implement a genereric bitvector set of macroes
typedef uintptr_t lrt_pic_id;
typedef uint64_t lrt_pic_set[LRT_PIC_MAX_PICS/64];

inline static void 
lrt_pic_set_copy(lrt_pic_set src, lrt_pic_set dest) 
{
  uintptr_t i;
  for (i=0; i<(sizeof(lrt_pic_set)/sizeof(uint64_t)); i++)
    dest[i]=src[i];
}

inline static void 
lrt_pic_set_clear(lrt_pic_set s) 
{
  uintptr_t i;
  for (i=0; i<(sizeof(lrt_pic_set)/sizeof(uint64_t)); i++)
    s[i]=0;
}

inline static void 
lrt_pic_set_add(lrt_pic_set s, lrt_pic_id i)
{
  if (i>LRT_PIC_MAX_PIC_ID) return;
  uint64_t mask = (uint64_t)1 << (i%64);
  s[i/64] |= mask;
}

inline static void 
lrt_pic_set_remove(lrt_pic_set s, lrt_pic_id i)
{
  if (i>LRT_PIC_MAX_PIC_ID) return;
  uint64_t mask = ~((uint64_t)1 << (i%64));
  s[i/64] &= mask;
}

inline static uintptr_t 
lrt_pic_set_test(lrt_pic_set s, lrt_pic_id i)
{
  if (i>LRT_PIC_MAX_PIC_ID) return 0;
  uint64_t mask = (uint64_t)1 << (i%64);
  return ((s[i/64] & mask) != 0);
}

inline static void
lrt_pic_set_addall(lrt_pic_set s)
{
  uintptr_t i;
  for (i=0; i<(sizeof(lrt_pic_set)/sizeof(uint64_t)); i++)
    s[i]=-1;
}

typedef void (*lrt_pic_handler)(void);
typedef uintptr_t lrt_pic_src;

//FIXME:  JA would like these to be consts
#ifdef __APPLE__
#include <pthread.h>
extern pthread_key_t lrt_pic_myid_pthreadkey;
#define lrt_pic_myid ((lrt_pic_id)pthread_getspecific(lrt_pic_myid_pthreadkey))
#else
extern __thread lrt_pic_id lrt_pic_myid;
#endif
extern lrt_pic_id lrt_pic_firstid;
extern lrt_pic_id lrt_pic_lastid;

extern uintptr_t lrt_pic_firstvec(void);
extern uintptr_t lrt_pic_numvec(void);
extern intptr_t lrt_pic_init(uintptr_t numlpics, lrt_pic_handler h);
extern intptr_t lrt_pic_loop(lrt_pic_id id);
extern intptr_t lrt_pic_allocvec(uintptr_t *vec);
extern intptr_t lrt_pic_mapvec(lrt_pic_src src, uintptr_t vec, lrt_pic_handler h);
extern intptr_t lrt_pic_mapipi(lrt_pic_handler h);
extern intptr_t lrt_pic_mapreset(lrt_pic_handler h);
extern intptr_t lrt_pic_reset(void);
extern intptr_t lrt_pic_ipi(lrt_pic_id targets);
extern void lrt_pic_ackipi(void);
extern void lrt_pic_enable(uintptr_t vec);
extern void lrt_pic_enableipi(void);
extern void lrt_pic_disable(uintptr_t vec);
extern void lrt_pic_disableipi(void);
#endif
