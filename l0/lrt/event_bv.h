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

#ifndef __LRT_EVENT_BV_H__
#define __LRT_EVENT_BV_H__

#define LRT_EVENT_BV_NUM_WORDS ((LRT_EVENT_NUM_EVENTS/64)+1)
struct corebv {
  uint64_t vec[LRT_EVENT_BV_NUM_WORDS];
};

static struct corebv *lrt_event_bv; 

static void
lrt_event_set_bit_bv(struct corebv *bv, lrt_event_num num) 
{
  int word = num/64;
  uint64_t mask = (uint64_t)1 << (num%64);
  __sync_fetch_and_or (&bv->vec[word], mask);
}

static int
lrt_event_get_unset_bit_bv(struct corebv *bv) 
{
  int word, bit, num;
  for (word = 0; word <LRT_EVENT_BV_NUM_WORDS ; word++) {
    if( bv->vec[word] ) break;
  }
  if (word >= LRT_EVENT_BV_NUM_WORDS) return -1;
  
  // FIXME: use gcc builtin routines for this
  for (bit = 0; bit < 64; bit++) {
    uint64_t mask = (uint64_t)1 << bit;
    if (bv->vec[word] & mask) {
      // found a set bit
      uint64_t mask = ~((uint64_t)1 << bit);
      __sync_fetch_and_and(&bv->vec[word], mask);
      break;
    }
  }
  num = word * 64 + bit;
  LRT_Assert(num<=LRT_EVENT_NUM_EVENTS);
    
  return num;
};


static void
lrt_event_set_bit(lrt_event_loc loc, lrt_event_num num) 
{
  lrt_event_set_bit_bv(&lrt_event_bv[loc], num);
};

static int
lrt_event_get_unset_bit(lrt_event_loc loc) 
{
  return lrt_event_get_unset_bit_bv(&lrt_event_bv[loc]);
};
#endif
