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
#include <l0/lrt/event.h>
#include <l0/lrt/mem.h>
#include <l0/lrt/trans.h>
#include <lrt/assert.h>
#include <lrt/io.h>

// I want to map just a single superpage for each of these,
// Under the assumption that they are in the top 256 GB of memory
// (meaning last entry of pml4), then we need to allocate one pdpt
// entry for each, and one 2MB superpage pdir for each

STATIC_ASSERT(PML4_INDEX(LRT_TRANS_GMEM) == PML4_INDEX(LRT_TRANS_LMEM),
               "Gmem and Lmem are not within the same pml4 entry");

STATIC_ASSERT(PDPT_INDEX(LRT_TRANS_GMEM) != PDPT_INDEX(LRT_TRANS_LMEM),
               "Gmem and Lmem are within the same pdpt entry");

STATIC_ASSERT(LRT_TRANS_TBLSIZE == (LARGE_PAGE_SIZE),
               "Table mapping will not fit within a large page, "
               "fix code accordingly");

static char *theGMem;

static lrt_trans_ltrans **lmem_table;

void
lrt_trans_preinit(int cores) {
  theGMem = lrt_mem_alloc(LRT_TRANS_TBLSIZE, LARGE_PAGE_SIZE, 0);
  pml4_ent *pml4 = get_pml4();
  pdpt_ent *trans_pdpt = lrt_mem_alloc(sizeof(pdpt_ent) * 512, PAGE_SIZE,
                                       0);
  pd_2m_ent *trans_pdir = lrt_mem_alloc(sizeof(pd_2m_ent) * 1024,
                                        PAGE_SIZE,
                                        0);
  for (int i = 0; i < 512; i++) {
    trans_pdpt[i].raw = 0;
    trans_pdir[i].raw = 0;
  }

  trans_pdir[PDIR_INDEX((void *)LRT_TRANS_GMEM)].present =1;
  trans_pdir[PDIR_INDEX((void *)LRT_TRANS_GMEM)].rw = 1;
  trans_pdir[PDIR_INDEX((void *)LRT_TRANS_GMEM)].ps = 1;
  trans_pdir[PDIR_INDEX((void *)LRT_TRANS_GMEM)]. base =
    ((uint64_t)theGMem) >> 21;

  trans_pdpt[PDPT_INDEX((void *)LRT_TRANS_GMEM)].present = 1;
  trans_pdpt[PDPT_INDEX((void *)LRT_TRANS_GMEM)].rw = 1;
  trans_pdpt[PDPT_INDEX((void *)LRT_TRANS_GMEM)].base =
    ((uint64_t)trans_pdir) >> 12;

  pml4[PML4_INDEX((void *)LRT_TRANS_GMEM)].present = 1;
  pml4[PML4_INDEX((void *)LRT_TRANS_GMEM)].rw = 1;
  pml4[PML4_INDEX((void *)LRT_TRANS_GMEM)].base =
    ((uint64_t)trans_pdpt) >> 12;
  lmem_table = lrt_mem_alloc(sizeof(lrt_trans_ltrans *) * cores,
                             sizeof(lrt_trans_ltrans *),
                             0);
}

void
lrt_trans_specific_init() {
  pml4_ent *mypml4 = lrt_mem_alloc(sizeof(pml4_ent) * 512, PAGE_SIZE,
                                   lrt_my_event_loc());
  pdpt_ent *trans_pdpt = lrt_mem_alloc(sizeof(pdpt_ent) * 512, PAGE_SIZE,
                                       lrt_my_event_loc());
  pd_2m_ent *trans_pdir = lrt_mem_alloc(sizeof(pd_2m_ent) * 512, PAGE_SIZE,
                                        lrt_my_event_loc());
  lrt_trans_ltrans *lmem = lrt_mem_alloc(LRT_TRANS_TBLSIZE, LARGE_PAGE_SIZE,
                             lrt_my_event_loc());
  lmem_table[lrt_my_event_loc()] = lmem;
  pml4_ent *pml4 = get_pml4();
  pdpt_ent *existing_pdpt =
    (pdpt_ent *)((uintptr_t)pml4[PML4_INDEX((void *)LRT_TRANS_GMEM)].base <<
                 12);

  for (int i = 0; i < 512; i++) {
    mypml4[i].raw = pml4[i].raw;
    trans_pdpt[i].raw = existing_pdpt[i].raw;
    trans_pdir[i].raw = 0;
  }

  trans_pdir[PDIR_INDEX((void *)LRT_TRANS_LMEM)].present = 1;
  trans_pdir[PDIR_INDEX((void *)LRT_TRANS_LMEM)].rw = 1;
  trans_pdir[PDIR_INDEX((void *)LRT_TRANS_LMEM)].ps = 1;
  trans_pdir[PDIR_INDEX((void *)LRT_TRANS_LMEM)].base = ((uint64_t)lmem) >> 21;

  trans_pdpt[PDPT_INDEX((void *)LRT_TRANS_LMEM)].present = 1;
  trans_pdpt[PDPT_INDEX((void *)LRT_TRANS_LMEM)].rw = 1;
  trans_pdpt[PDPT_INDEX((void *)LRT_TRANS_LMEM)].base =
    ((uint64_t)trans_pdir) >> 12;

  mypml4[PML4_INDEX((void *)LRT_TRANS_GMEM)].present = 1;
  mypml4[PML4_INDEX((void *)LRT_TRANS_GMEM)].rw = 1;
  mypml4[PML4_INDEX((void *)LRT_TRANS_GMEM)].base = ((uint64_t)trans_pdpt) >> 12;

  set_pml4(mypml4);

  //Probably not necessary to invalid the TLB entries but just to be sure
  __asm__ volatile (
                    "mfence\n\t"
                    "invlpg %[gmem]\n\t"
                    "invlpg %[lmem]"
                    :
                    : [gmem] "m" (*(char *)LRT_TRANS_GMEM), [lmem] "m" (*(char *)LRT_TRANS_LMEM)
                    );
}

void lrt_trans_invalidate_rltrans(lrt_event_loc el, lrt_trans_id oid)
{
  lrt_trans_ltrans *lmem = lmem_table[el];
  ptrdiff_t index = oid - lrt_trans_idbase();
  lrt_trans_ltrans *rlt = lmem + index; /* pointer to remote local entry */

  lrt_trans_ltrans *lt = lrt_trans_id2lt(oid);
  rlt->ref = &lt->rep;
  rlt->rep = lrt_trans_def_rep;
  
}
