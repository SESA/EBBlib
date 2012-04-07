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

#include <arch/powerpc/cpu.h>
#include <arch/powerpc/mmu.h>
#include <l0/lrt/bare/arch/ppc32/trans.h>
#include <lrt/assert.h>

static uint8_t theGMem[LRT_TRANS_TBLSIZE] 
__attribute__((aligned(LRT_TRANS_TBLSIZE)));

static uint8_t BSPLMem[LRT_TRANS_TBLSIZE] 
__attribute__((aligned(LRT_TRANS_TBLSIZE)));


static uintptr_t ilog2(uintptr_t val)
{
  uintptr_t ret = 0;
  while (val >>= 1) {
    ret++;
  }
  return ret;
}

static void
map_addr (uint8_t *vaddr, uint64_t paddr, uintptr_t size) {
  //numbers taken from my dump of the TLB
  static uint8_t entry = 23;
  EBBAssert(entry < 63);
  tlb_word_0 t0;
  tlb_word_1 t1;
  tlb_word_2 t2;

  //parity is calculated, not set
  t0.val = 0;
  t0.epn = ((uintptr_t)vaddr) >> 10;
  t0.v = 1;
  t0.size = ilog2(size >> 10) / 2;

  t1.val = 0;
  t1.rpn = (uint32_t)((paddr >> 10) & 0x3fffff);
  t1.erpn = (uint32_t)((paddr >> 32) & 0xf);

  t2.val = 0;
  //we may want to play with these
  t2.wl1 = 1;
  t2.m = 1;
  t2.sx = 1;
  t2.sw = 1;
  t2.sr = 1;

  asm volatile (
		"tlbwe %[word0],%[entry],0;"
		"tlbwe %[word1],%[entry],1;"
		"tlbwe %[word2],%[entry],2;"
		:
		: [word0] "r" (t0.val),
		  [word1] "r" (t1.val),
		  [word2] "r" (t2.val),
		  [entry] "r" (entry++)
		);
}

void
lrt_trans_init()
{
  //check that the table size is an exact power of 4k
  //is power of 2?
  EBBAssert(__builtin_popcount(LRT_TRANS_TBLSIZE) == 1);
  //is > 1K?
  EBBAssert(__builtin_ctz(LRT_TRANS_TBLSIZE) >= 10);
  //is power of 4?
  EBBAssert(__builtin_ctz(LRT_TRANS_TBLSIZE) % 2 == 0);

  //put size into the expected form: truesize = 4^size kb
  uint8_t size = ilog2(LRT_TRANS_TBLSIZE >> 10) / 2;

  //Make sure table fits perfectly into a single TLB entryb
  EBBAssert(SUPPORTED_TLB_PAGE_SIZE & size);

  mmucr mmucr;
  mmucr.val = get_spr(SPRN_MMUCR);
  mmucr.stid = 0;
  set_spr(SPRN_MMUCR, mmucr);

  map_addr((uint8_t *)GMem, (uint64_t)theGMem, LRT_TRANS_TBLSIZE);
  map_addr((uint8_t *)LMem, (uint64_t)BSPLMem, LRT_TRANS_TBLSIZE);
}
