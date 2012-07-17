#ifndef __BITVECTOR_H__
#define __BITVECTOR_H__

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

/*
 * Bitvector as macro for arbitrary number of bits.  Uses atomic operations
 * to set and unset bits; to use for a fred bitvector with 512 bits, say:
 * say DEF_BITVEC(fred, 512). Will aign up to a 64 bit word. 
 */
#define BV_WORDS(BITS) ((BITS/64)+1)	

#define DEF_BITVEC(NAME,BITS)					\
struct NAME ## _bvs {						\
  uint64_t vec[BV_WORDS(BITS)];				\
};								\
static inline void						\
NAME ## _set_bit_bv(struct NAME ## _bvs *bv, int bit)		\
{								\
  int word = bit/64;						\
  uint64_t mask = (uint64_t)1 << (bit%64);			\
  __sync_fetch_and_or (&bv->vec[word], mask);			\
}								\
static inline int						\
NAME ## _get_unset_bit_bv(struct NAME ## _bvs *bv)		\
{								\
  int word, bit, num;						\
  for (word = 0; word <BV_WORDS(BITS) ; word++) {		\
    if( bv->vec[word] ) break;					\
  }								\
  if (word >= BV_WORDS(BITS)) return -1;			\
								\
  /* FIXME: use gcc builtin routines for this */		\
  for (bit = 0; bit < 64; bit++) {				\
    uint64_t mask = (uint64_t)1 << bit;				\
    if (bv->vec[word] & mask) {					\
      /* found a set bit */					\
      uint64_t mask = ~((uint64_t)1 << bit);			\
      __sync_fetch_and_and(&bv->vec[word], mask);		\
      break;							\
    }								\
  }								\
  num = word * 64 + bit;					\
  LRT_Assert(num<=BITS);					\
  return num;							\
};
#endif
