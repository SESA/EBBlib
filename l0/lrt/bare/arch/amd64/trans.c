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

#include <arch/amd64/paging.h>
#include <l0/lrt/bare/arch/amd64/trans.h>
#include <lrt/assert.h>
#include <lrt/io.h>

//The idea is to statically allocate global memory and the first
// processor's local memory and map them into the page table
// this lets us bring up ebbs such as our memory allocator
// then we can bring up the other cores as necessary

static uint8_t theGMem[LRT_TRANS_TBLSIZE] 
__attribute__((aligned(LARGE_PAGE_SIZE)));

static uint8_t BSPLMem[LRT_TRANS_TBLSIZE] 
__attribute__((aligned(LARGE_PAGE_SIZE)));

// I want to map just a single superpage for each of these,
// Under the assumption that they are in the top 256 GB of memory
// (meaning last entry of pml4), then we need to allocate one pdpt
// entry for each, and one 2MB superpage pdir for each

//TODO make those inlines in paging.h into macros so I can do this
STATIC_ASSERT(PML4_INDEX(GMem) == PML4_INDEX(LMem),
	       "Gmem and Lmem are not within the same pml4 entry");

STATIC_ASSERT(PDPT_INDEX(GMem) != PDPT_INDEX(LMem),
	       "Gmem and Lmem are within the same pdpt entry");

STATIC_ASSERT(LRT_TRANS_TBLSIZE <= (LARGE_PAGE_SIZE),
	       "Table mapping will not fit within a large page, "
	       "fix code accordingly");

static pdpt_ent trans_pdpt[512] __attribute__((aligned(PAGE_SIZE)));
static pd_2m_ent trans_pdir[2][512] __attribute__((aligned(PAGE_SIZE)));

void
lrt_trans_init() {
  printf("lrt_trans_init called!\n");

  pml4_ent *pml4 = get_pml4();
  
  trans_pdir[0][PDIR_INDEX((void *)GMem)].present = 1;
  trans_pdir[0][PDIR_INDEX((void *)GMem)].rw = 1;
  trans_pdir[0][PDIR_INDEX((void *)GMem)].ps = 1;
  trans_pdir[0][PDIR_INDEX((void *)GMem)].base = ((uint64_t)theGMem) >> 21;
  
  trans_pdir[1][PDIR_INDEX((void *)LMem)].present = 1;
  trans_pdir[1][PDIR_INDEX((void *)LMem)].rw = 1;
  trans_pdir[1][PDIR_INDEX((void *)LMem)].ps = 1;
  trans_pdir[1][PDIR_INDEX((void *)LMem)].base = ((uint64_t)BSPLMem) >> 21;
  
  trans_pdpt[PDPT_INDEX((void *)GMem)].present = 1;
  trans_pdpt[PDPT_INDEX((void *)GMem)].rw = 1;
  trans_pdpt[PDPT_INDEX((void *)GMem)].base = ((uint64_t)trans_pdir[0]) >> 12;

  trans_pdpt[PDPT_INDEX((void *)LMem)].present = 1;
  trans_pdpt[PDPT_INDEX((void *)LMem)].rw = 1;
  trans_pdpt[PDPT_INDEX((void *)LMem)].base = ((uint64_t)trans_pdir[1]) >> 12;

  pml4[PML4_INDEX((void *)GMem)].present = 1;
  pml4[PML4_INDEX((void *)GMem)].rw = 1;
  pml4[PML4_INDEX((void *)GMem)].base = ((uint64_t)trans_pdpt) >> 12;

  //Probably not necessary to invalid the TLB entries but just to be sure
  __asm__ volatile (
		    "mfence\n\t"
		    "invlpg %[gmem]\n\t"
		    "invlpg %[lmem]"
		    : 
		    : [gmem] "m" (*(char *)GMem), [lmem] "m" (*(char *)LMem)
		    );
}
