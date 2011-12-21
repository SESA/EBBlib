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

// temporary - for debugging.
#include "fmt.h"

static const uint64_t CR0_PG = (1<<31);
static const uint64_t CR4_PAE = (1<<5);
static const uint64_t IA_32E_LME = (1<<8);

static const uint32_t IA32_EFER = 0xc0000080;

/* we need a #define here because we use this one in page_struct. */
#define PAGE_ENTS_CONST 512
static const uint64_t PAGE_ENTS = PAGE_ENTS_CONST;
static const uint64_t PAGE_LEVELS = 4;
static const uint64_t PAGE_PRESENT = (1<<0);
static const uint64_t PAGE_RW = (1<<1);
static const uint64_t PAGE_US = (1<<2);

static const uint64_t PAGE_SIZE = 0x1000;
static const uint64_t PAGE_MASK = ~0xfff;

static const uint64_t PAGE_ENT_MASK = 0x1ff;

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
  __asm__ volatile("movl %0, %%ecx\n"
                   "rdmsr\n"
                   "orl %1, %%eax\n"
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

void map_page(uint64_t laddr, page_struct *pgs) {
  uintptr_t shift = 39;
  int i;
  for(i = 0; i < 3; i++) {
    uintptr_t entnum = PAGE_ENT_MASK & (laddr>>shift);
    if(!(pgs->ents[entnum] & PAGE_PRESENT)) {
      pgs->ents[entnum] = (uintptr_t)premalloc(sizeof(page_struct), PAGE_SIZE);
      bzero((void*)(uintptr_t)pgs->ents[entnum], sizeof(page_struct));
      pgs->ents[entnum] |= PAGE_PRESENT | PAGE_RW;
    }
    pgs = (page_struct*)(uintptr_t)(pgs->ents[entnum] & PAGE_MASK);
    shift -= 9;
  }
  pgs->ents[laddr>>shift] = (laddr & PAGE_MASK) | PAGE_PRESENT | PAGE_RW;

}

void print_pagestruct(unsigned int level, page_struct *pgs) {
  unsigned int i, j;
  if(level >= PAGE_LEVELS)
    return;
  for(j = 0; j < level; j++)
    printf(" ");
  printf("level %d page structure = {\n", level);
  for(i = 0; i < PAGE_ENTS; i++) {
    for(j = 0; j < level; j++)
      printf(" ");
    printf("[%d] = 0x%x\n", i, pgs->ents[i]);
    if(pgs->ents[i])
      print_pagestruct(level+1, (page_struct*)(uintptr_t)(pgs->ents[i] & PAGE_MASK));
  } 
  for(j = 0; j < level; j++)
    printf(" ");
  printf("}\n");
  
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
  print_pagestruct(0, kernel_pml4);
  enable_paging();
}
