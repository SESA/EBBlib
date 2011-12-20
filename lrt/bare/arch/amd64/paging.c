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

#include "premalloc.h"
#include "util.h"
#include "paging.h"

static const uint32_t CR0_PG = (1<<31);
static const uint32_t CR4_PAE = (1<<5);
static const uint32_t IA_32E_LME = (1<<8);

static const uint32_t IA32_EFER = 0xc0000080;

/* we need a #define here because we use this one in page_struct. */
#define PAGE_ENTS_CONST 512
static const uint32_t PAGE_ENTS = PAGE_ENTS_CONST;
static const uint32_t PAGE_LEVELS = 4;
static const uint32_t PAGE_PRESENT = (1<<0);

static const uint32_t PAGE_SIZE = 0x1000;
static const uint32_t PAGE_MASK = ~0xfff;

typedef struct page_struct page_struct;
struct page_struct {
    uint64_t ents[PAGE_ENTS_CONST];
};

static inline void load_pml4(page_struct *pml4) {
  __asm__ volatile ("movl %0, %%cr3" :: "r"(pml4));
}

static inline void enable_pae(void) {
   uint32_t cr4;
  __asm__ volatile ("movl %%cr4, %0" : "=r"(cr4));
  cr4 |= CR4_PAE;
  __asm__ volatile ("movl %0, %%cr4" :: "r"(cr4));
}

static inline void enable_longmode(void) {
  __asm__ volatile("movl %%ecx, %0\n"
                   "rdmsr\n"
                   "orl %%eax, %1\n"
                   "wrmsr\n"
                   :: "r"(IA32_EFER), "r"(IA_32E_LME)
                   : "eax", "ecx", "edx");
}

static inline void enable_paging(void){
  uint32_t cr0;
  __asm__ volatile ("movl %%cr0, %0" : "=r"(cr0));
  cr0 |= CR0_PG;
  __asm__ volatile ("movl %0, %%cr0" :: "r"(cr0));
}

static page_struct *kernel_pml4;

void map_page(uintptr_t ptr, page_struct *pgs) {
  /* we truncate some addresses in here if in 32 bit mode, but this is okay,
     since we know the high bits are zero. */
  uintptr_t index, orig_ptr;
  uint8_t i;
  orig_ptr = ptr;
  ptr /= PAGE_SIZE;
  for(i = 1; i < PAGE_LEVELS; i++) {
    index = ptr / PAGE_ENTS;
    if(!pgs->ents[index]) {
      pgs->ents[index] = (uintptr_t)premalloc(sizeof(page_struct), PAGE_SIZE);
      bzero((void*)(uintptr_t)pgs->ents[index], sizeof(page_struct));
      pgs->ents[index] |= PAGE_PRESENT;
    }
    pgs = (page_struct*)(uintptr_t)(pgs->ents[index] & PAGE_MASK);
    ptr /= PAGE_SIZE;
  }
  pgs->ents[index] = (orig_ptr & PAGE_MASK) | PAGE_PRESENT;
}

void paging_init(void) {
  uintptr_t offset = 0;
  kernel_pml4 = premalloc(sizeof(page_struct), PAGE_SIZE);
  bzero(kernel_pml4, sizeof(page_struct));
  while(offset < (uintptr_t)nextptr) {
    map_page(offset, kernel_pml4);
    offset += PAGE_SIZE;
  }
  load_pml4(kernel_pml4);
  enable_pae();
  enable_longmode();
  enable_paging();
}
