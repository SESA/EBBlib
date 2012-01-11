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
#include <stdbool.h>

#include <arch/amd64/cpu.h>
#include <arch/amd64/multiboot.h>
#include <arch/amd64/paging.h>
#include <arch/amd64/segmentation.h>
#include <l0/lrt/bare/arch/amd64/init64.h>

pml4_ent init_pml4[512] __attribute__((aligned(4096), section(".init.data32")));
pdpt_ent init_pdpt[512] __attribute__((aligned(4096), section(".init.data32")));
pd_2m_ent init_pdir[4][512] __attribute__((aligned(4096), section(".init.data32")));

gdt init_gdt __attribute__((section(".init.data32")));

//TODO DS: Maybe this should do something?
static inline void __attribute__ ((noreturn))
panic(void)
{
  while(1)
    ;
}


//all calls from this function should be inlined
void __attribute__ ((section(".init.text32"),noreturn))
init32(multiboot_info_t *mbi, uint32_t magic) 
{

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    //we weren't loaded by a multiboot compliant loader!
    panic();
  }
  
  if (!has_longmode()) {
    panic();
  }
  
  if (mbi->flags & MULTIBOOT_INFO_MEMORY) {
  }

  if (mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
    
    for (multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mbi->mmap_addr;
	 mmap < (multiboot_memory_map_t *)(mbi->mmap_addr + mbi->mmap_length);
	 mmap = (multiboot_memory_map_t *)(((unsigned char *)mmap) + 
					   mmap->size + 
					   sizeof(multiboot_uint32_t))) {
    }
  }
  
  if (mbi->flags & MULTIBOOT_INFO_MODS) {
    multiboot_module_t mod __attribute__ ((unused));
    for (int i = 0; i < mbi->mods_count; i++) {
      mod = ((multiboot_module_t *)mbi->mods_addr)[i];
    }
  }

  //zero paging structures
  for (int i = 0; i < 512; i++) {
    init_pml4[i].raw = 0;
  }

  for (int i = 0; i < 512; i++) {
    init_pdpt[i].raw = 0;
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 512; j++) {
      init_pdir[i][j].raw = 0;
    }
  }


  //map first 4GB idempotently using 2m pages
  init_pml4[0].present = 1;
  init_pml4[0].rw = 1;
  init_pml4[0].base = (uint64_t)(((uintptr_t)init_pdpt) >> 12);

  for (int i = 0; i < 4; i++) {
    init_pdpt[i].present = 1;
    init_pdpt[i].rw = 1;
    init_pdpt[i].base = (uint64_t)(((uintptr_t)init_pdir[i]) >> 12);
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 512; j++) {
      init_pdir[i][j].present = 1;
      init_pdir[i][j].rw = 1;
      init_pdir[i][j].ps = 1;
      init_pdir[i][j].base = (i * 512) + j;
    }
  }

  load_pml4(init_pml4);
  enable_pae();
  enable_longmode();
  enable_paging();
  if (!longmode_active()) {
    panic();
  }

  //setup GDT
  init_gdt.invalid.raw = 0;
  init_gdt.code.raw = 0;

  //long mode code segment
  init_gdt.code.type = 0x8; //code, execute only
  init_gdt.code.s = 1; //not a system segment
  init_gdt.code.p = 1; //present
  init_gdt.code.l = 1; //long mode code segment
  
  load_gdtr(&init_gdt, sizeof(init_gdt));

  __asm__ volatile (
		    "pushw %w[init32_cs]\n\t"
		    "pushl %[init64]\n\t"
		    "ljmp *(%%esp)"
		    :
		    :
		    [init32_cs] "r" (0x08),
		    [init64] "r" (&init64),
		    "D" (mbi)
		    );
  
  panic();
}
