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

#include <arch/ppc64/cpu.h>
#include <arch/ppc64/mmu.h>
#include <l0/lrt/bare/arch/ppc64/trans.h>
#include <lrt/assert.h>

static uint8_t theGMem[LRT_TRANS_TBLSIZE]
__attribute__((aligned(LRT_TRANS_TBLSIZE)));

static uint8_t BSPLMem[LRT_TRANS_TBLSIZE]
__attribute__((aligned(LRT_TRANS_TBLSIZE)));

static void
map_addr(uint8_t *virt, uint8_t *real, uint64_t size)
{
  //zero these for the search
  mas5 mas5;
  mas5.val = get_spr(SPRN_MAS5);
  mas5.val = 0;
  set_spr(SPRN_MAS5, mas5);

  mas6 mas6;
  mas6.val = get_spr(SPRN_MAS6);
  mas6.val = 0;
  set_spr(SPRN_MAS6, mas6);

  //The tlbsx only tells me if there is a mapping for the address
  //because I want to map in a large page, I have to walk
  //the tlb in small pages to make sure no smaller page is mapped
  //within the page I want
  for (char *addr = (char *)virt; 
      addr < (char *)(virt + size);
      addr += (1 << 12)) {
    asm volatile(
		 "tlbsx 0, %[ea]"
		 :
		 : [ea] "b" (addr)
		 );
    
    mas1 mas1;
    mas1.val = get_spr(SPRN_MAS1);
    LRT_Assert(!mas1.v);
  }

  //Ok there is no virtual mapping sitting in our TLB in the space
  // we want to map, now to find a free spot in the congruence
  // class
  
  //Check that there is a free spot in the congruence class
  //so we don't evict an entry!
  int i;
  for (i = 0; i < 4; i++) {
    mas0 mas0;
    mas0.val = 0;
    mas0.hes = 0; //turn off hardware select so we select the entry
    mas0.esel = i; //select the ith entry
    set_spr(SPRN_MAS0, mas0);

    mas1 mas1;
    mas1.val = 0;
    
    //FIXME: This is hard-coded!!!!
    LRT_Assert(size == (1 << 20));
    mas1.tsize = 5; //1 MB size
    
    set_spr(SPRN_MAS1, mas1);

    mas2 mas2;
    mas2.val = 0;
    mas2.epn = (uintptr_t)virt >> 12; //which addr
    set_spr(SPRN_MAS2, mas2);

    asm volatile("tlbre");

    mas1.val = get_spr(SPRN_MAS1);
    if(!mas1.v) {
      break;
    }
  }
  LRT_Assert(i < 4);
  //WOO! We have a free entry in the TLB to write to
  
  mas0 mas0;
  mas0.val = 0;
  mas0.hes = 0;
  mas0.esel = i; //select the free ith entry
  set_spr(SPRN_MAS0, mas0);
  
  mas1 mas1;
  mas1.val = 0;
  mas1.v = 1;
  mas1.tid = get_spr(SPRN_PID);

  //FIXME: This is hard-coded!!!!
  LRT_Assert(size == (1 << 20));
  mas1.tsize = 5; //1 MB size

  set_spr(SPRN_MAS1, mas1);

  mas2 mas2;
  mas2.val = 0;
  mas2.m = 1; //memory coherence (maybe?)
  mas2.epn = (uintptr_t)virt >> 12;
  set_spr(SPRN_MAS2, mas2);

  //TODO: set RPN to mas3 and mas 7
  mas3 mas3;
  mas3.val = 0;
  mas3.rpnl = ((uintptr_t)real >> 11) & ((1 << 21) - 1);
  mas3.sw = 1; //turn on supervisor write
  mas3.sr = 1; //turn on supervisor read
  set_spr(SPRN_MAS3, mas3);

  mas7 mas7;
  mas7.val = 0;
  mas7.rpnu = ((uintptr_t)real >> 32) & ((1 << 10) - 1);
  set_spr(SPRN_MAS7, mas7);

  //make sure mmucr3 is set so all thdids are on
  mmucr3 mmucr3;
  mmucr3.val = 0;
  mmucr3.thdid = 0xf;
  set_spr(SPRN_MMUCR3, mmucr3);

  asm volatile(
	       "sync\n\t"
	       "tlbwe\n\t"
	       "sync"
	       );  
}

void
lrt_trans_init()
{
  //check that the table size is the exact power of 2
  LRT_Assert(__builtin_popcount(LRT_TRANS_TBLSIZE) == 1);
  
  //Make sure the table size fits within a single TLB entry
  uint32_t tlbps = get_spr(SPRN_TLB0PS);
  LRT_Assert(tlbps & LRT_TRANS_TBLSIZE);

  map_addr((uint8_t *)GMem, theGMem, LRT_TRANS_TBLSIZE);

  map_addr((uint8_t *)LMem, BSPLMem, LRT_TRANS_TBLSIZE);  

}

// returns the pointer to a remote local translation entry for a object id
struct lrt_trans *lrt_trans_id2rlt(lrt_pic_id picid, uintptr_t oid)
{
  // only supports one core for now
  LRT_Assert(picid == lrt_pic_myid);

  return lrt_trans_id2lt(oid);
}
