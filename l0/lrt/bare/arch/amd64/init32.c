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

#include <stdbool.h>
#include <stdint.h>

#include <arch/amd64/cpu.h>
#include <arch/amd64/multiboot.h>
#include <arch/amd64/paging.h>
#include <arch/amd64/segmentation.h>
#include <l0/lrt/bare/arch/amd64/init64.h>

pml4_ent init_pml4[PML4_NUM_ENTS]
__attribute__((aligned(PML4_ALIGN), section(".init.data32")));

pdpt_ent init_pdpt[PDPT_NUM_ENTS]
__attribute__((aligned(PDPT_ALIGN), section(".init.data32")));

pd_2m_ent init_pdir[PDIR_NUM_ENTS]
__attribute__((aligned(PDIR_ALIGN), section(".init.data32")));

pdpt_ent apic_pdpt[PDPT_NUM_ENTS]
__attribute__((aligned(PDPT_ALIGN), section(".init.data32")));

pd_2m_ent apic_pdir[PDIR_NUM_ENTS]
__attribute__((aligned(PDIR_ALIGN), section(".init.data32")));

//One invalid entry and a code segment entry
segdesc init_gdt[2] __attribute__((aligned(8), section(".init.data32")));
#define INIT32_CS (0x8)

//TODO DS: Maybe this should do something?
static inline void __attribute__ ((section(".init.text32"),noreturn))
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

  //Eventually these statements can be used to parse the mbi
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

  //now the real work starts

  //FIXME: In the long run consider optimization
  //zero paging structures
  for (int i = 0; i < PML4_NUM_ENTS; i++) {
    init_pml4[i].raw = 0;
  }

  for (int i = 0; i < PDPT_NUM_ENTS; i++) {
    init_pdpt[i].raw = 0;
  }

  for (int i = 0; i < PDIR_NUM_ENTS; i++) {
    init_pdir[i].raw = 0;
    apic_pdir[i].raw = 0;
  }


  //map first 1GB idempotently using 2m pages
  init_pml4[0].present = 1;
  init_pml4[0].rw = 1;
  init_pml4[0].base = (uint64_t)(((uintptr_t)init_pdpt) >> 12);

  init_pdpt[0].present = 1;
  init_pdpt[0].rw = 1;
  init_pdpt[0].base = (uint64_t)(((uintptr_t)init_pdir) >> 12);

  init_pdpt[3].present = 1;
  init_pdpt[3].rw = 1;
  init_pdpt[3].base = (uint64_t)(((uintptr_t)apic_pdir) >> 12);

  for (int i = 0; i < 256; i++) {
    init_pdir[i].present = 1;
    init_pdir[i].rw = 1;
    init_pdir[i].ps = 1;
    init_pdir[i].base = i;
  }

  apic_pdir[502].present = 1;
  apic_pdir[502].rw = 1;
  apic_pdir[502].write_through = 1;
  apic_pdir[502].cache_disable = 1;
  apic_pdir[502].ps = 1;
  apic_pdir[502].base = (uint64_t)(((uintptr_t)0xfec00000) >> 21);

  apic_pdir[503].present = 1;
  apic_pdir[503].rw = 1;
  apic_pdir[503].write_through = 1;
  apic_pdir[503].cache_disable = 1;
  apic_pdir[503].ps = 1;
  apic_pdir[503].base = (uint64_t)(((uintptr_t)0xfee00000) >> 21);

  // now that data structures are ready we load them into the VMM
  // facilities and turn on:
  //    1) Physical Address Extention (PAE)
  //    2) Long Mode
  //    3) Paging on
  set_pml4(init_pml4);
  enable_pae();
  enable_longmode();
  enable_paging();
  if (!longmode_active()) {
    panic();
  }

  //setup GDT
  init_gdt[0].raw = 0;
  init_gdt[1].raw = 0;

  //long mode code segment
  init_gdt[1].type = 0x8; //code, execute only
  init_gdt[1].s = 1; //not a system segment
  init_gdt[1].p = 1; //present
  init_gdt[1].l = 1; //long mode code segment

  load_gdtr(init_gdt, sizeof(init_gdt));

  //since the GDT is now loaded with a longmode code segment, we must do
  // a long jump to init64
  __asm__ volatile (
                    "andl $0xFFFFFFF0, %%esp\n"
                    "sub $2, %%esp\n\t" //for alignment!
                    "pushw %w[init32_cs]\n\t"
                    "pushl %[init64]\n\t"
                    "ljmp *(%%esp)"
                    :
                    :
                    [init32_cs] "r" (INIT32_CS),
                    [init64] "r" (&init64),
                    "D" (mbi)
                    );

  panic();
}
