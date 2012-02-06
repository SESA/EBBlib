#ifndef ARCH_AMD64_PAGING_H
#define ARCH_AMD64_PAGING_H

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

//I wish these had good names, I think I just made these up
#define PAGE_SIZE (1 << 12) //4k
#define LARGE_PAGE_SIZE (1 << 21) //2m
#define HUGE_PAGE_SIZE (1 << 30) //1g

//number of bits to right shift an address and then the number
// of bits to mask to get the index into the structure
#define PML4_INDEX_SHIFT (39)
#define PML4_INDEX_BITS (9)

#define PDPT_INDEX_SHIFT (30)
#define PDPT_INDEX_BITS (9)

#define PDIR_INDEX_SHIFT (21)
#define PDIR_INDEX_BITS (9)

#define PTAB_INDEX_SHIFT (12)
#define PTAB_INDEX_BITS (9)

#define PML4_SIZE (4096)
#define PDPT_SIZE (4096)
#define PDIR_SIZE (4096)
#define PTAB_SIZE (4096)

#define PML4_ALIGN (4096)
#define PDPT_ALIGN (4096)
#define PDIR_ALIGN (4096)
#define PTAB_ALIGN (4096)

#define PML4_NUM_ENTS (PML4_SIZE / sizeof (pml4_ent))
#define PDPT_NUM_ENTS (PDPT_SIZE / sizeof (pdpt_ent))
#define PDIR_NUM_ENTS (PDIR_SIZE / sizeof (pd_ent))
#define PTAB_NUM_ENTS (PTAB_SIZE / sizeof (pt_4k_ent))

typedef union {
  uint64_t raw;
  struct {
    uint64_t present :1;
    uint64_t rw :1;
    uint64_t privilege :1;
    uint64_t write_through :1;
    uint64_t cache_disable :1;
    uint64_t accessed :1;
    uint64_t ignored0 :1;
    uint64_t reserved0 :1;
    uint64_t ignored1 :4;
    uint64_t base :40;
    uint64_t ignored2 :11;
    uint64_t nx :1;
  };
} pml4_ent;

_Static_assert(sizeof(pml4_ent) == 8, "pml4_ent packing issue");

typedef union {
  uint64_t raw;
  struct {
    uint64_t present :1;
    uint64_t rw :1;
    uint64_t privilege :1;
    uint64_t write_through :1;
    uint64_t cache_disable :1;
    uint64_t accessed :1;
    uint64_t dirty :1;
    uint64_t ps :1; // must be set for 1g page
    uint64_t global :1;
    uint64_t ignored0 :3;
    uint64_t pat :1;
    uint64_t reserved0 :17;
    uint64_t base :22;
    uint64_t ignored1 :11;
    uint64_t nx :1;
  };
} pdpt_1g_ent;

_Static_assert(sizeof(pdpt_1g_ent) == 8, "pdpt_1g_ent packing issue");

typedef union {
  uint64_t raw;
  struct {
    uint64_t present :1;
    uint64_t rw :1;
    uint64_t privilege :1;
    uint64_t write_through :1;
    uint64_t cache_disable :1;
    uint64_t accessed :1;
    uint64_t ignored0 :1;
    uint64_t ps :1; // must be unset for standard entry
    uint64_t ignored1 :4;
    uint64_t base :40;
    uint64_t ignored2 :11;
    uint64_t nx :1;
  };
} pdpt_ent;

_Static_assert(sizeof(pdpt_ent) == 8, "pdpt_ent packing issue");

typedef union {
  uint64_t raw;
  struct {
    uint64_t present :1;
    uint64_t rw :1;
    uint64_t privilege :1;
    uint64_t write_through :1;
    uint64_t cache_disable :1;
    uint64_t accessed :1;
    uint64_t dirty :1;
    uint64_t ps :1; // must be set for 2m entry
    uint64_t global :1;
    uint64_t ignored0 :3;
    uint64_t pat :1;
    uint64_t reserved0 :8;
    uint64_t base :31;
    uint64_t ignored1 :11;
    uint64_t nx :1;
  };
} pd_2m_ent;

_Static_assert(sizeof(pd_2m_ent) == 8, "pd_2m_ent packing issue");

typedef union {
  uint64_t raw;
  struct {
    uint64_t present :1;
    uint64_t rw :1;
    uint64_t privilege :1;
    uint64_t write_through :1;
    uint64_t cache_disable :1;
    uint64_t accessed :1;
    uint64_t ignored0 :1;
    uint64_t ps :1; // must be unset for standard entry
    uint64_t ignored1 :4;
    uint64_t base :40;
    uint64_t ignored2 :11;
    uint64_t nx :1;
  };
} pd_ent;

_Static_assert(sizeof(pd_ent) == 8, "pd_ent packing issue");

typedef union {
  uint64_t raw;
  struct {
    uint64_t present :1;
    uint64_t rw :1;
    uint64_t privilege :1;
    uint64_t write_through :1;
    uint64_t cache_disable :1;
    uint64_t accessed :1;
    uint64_t dirty :1;
    uint64_t pat :1; // must be set for 2m entry
    uint64_t global :1;
    uint64_t ignored0 :3;
    uint64_t base :40;
    uint64_t ignored1 :11;
    uint64_t nx :1;
  };
} pt_4k_ent;

_Static_assert(sizeof(pt_4k_ent) == 8, "pt_4k_ent packing issue");

static inline pml4_ent *
get_pml4(void)
{
  pml4_ent *pml4;
  __asm__ volatile (
	 "mov %%cr3, %[pml4]"
	 : [pml4] "=r" (pml4)
	 :
	 );
  return pml4;
}

static inline void
set_pml4(pml4_ent pml4[512])
{
  __asm__ volatile (
	 "mov %[pml4], %%cr3"
	 :
	 : [pml4] "r" (pml4)
	 );
}

//These are inlines so that I can make some static assertions with them
#define PML4_INDEX(addr) \
  ((((uintptr_t)addr) >> PML4_INDEX_SHIFT) & ((1 << PML4_INDEX_BITS) - 1))

#define PDPT_INDEX(addr) \
  ((((uintptr_t)addr) >> PDPT_INDEX_SHIFT) & ((1 << PDPT_INDEX_BITS) - 1))

#define PDIR_INDEX(addr) \
  ((((uintptr_t)addr) >> PDIR_INDEX_SHIFT) & ((1 << PDIR_INDEX_BITS) - 1))

#define PTAB_INDEX(addr) \
  ((((uintptr_t)addr) >> PTAB_INDEX_SHIFT) & ((1 << PTAB_INDEX_BITS) - 1))

#endif

