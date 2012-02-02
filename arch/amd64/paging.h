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

//number of bits to right shift an address and then the number
// of bits to mask to get the index into the structure
static const uintptr_t PML4_INDEX_SHIFT = 39;
static const uintptr_t PML4_INDEX_BITS = 9;
static const uintptr_t PDPT_INDEX_SHIFT = 30;
static const uintptr_t PDPT_INDEX_BITS = 9;
static const uintptr_t PDIR_INDEX_SHIFT = 21;
static const uintptr_t PDIR_INDEX_BITS = 9;
static const uintptr_t PTAB_INDEX_SHIFT = 12;
static const uintptr_t PTAB_INDEX_BITS = 9;

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

static inline uint16_t 
pml4_index(void *addr)
{
  uintptr_t val = (uintptr_t)addr;
  return (val >> PML4_INDEX_SHIFT) & ((1 << PML4_INDEX_BITS) - 1);
}

static inline uint16_t 
pdpt_index(void *addr)
{
  uintptr_t val = (uintptr_t)addr;
  return (val >> PDPT_INDEX_SHIFT) & ((1 << PDPT_INDEX_BITS) - 1);
}

static inline uint16_t 
pdir_index(void *addr)
{
  uintptr_t val = (uintptr_t)addr;
  return (val >> PDIR_INDEX_SHIFT) & ((1 << PDIR_INDEX_BITS) - 1);
}

static inline uint16_t 
ptab_index(void *addr)
{
  uintptr_t val = (uintptr_t)addr;
  return (val >> PTAB_INDEX_SHIFT) & ((1 << PTAB_INDEX_BITS) - 1);
}

#endif

