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

#include <arch/amd64/apic.h>
#include <arch/amd64/cpu.h>
#include <arch/amd64/idt.h>
#include <arch/amd64/multiboot.h>
#include <arch/amd64/pic.h>
#include <arch/amd64/pit.h>
#include <arch/amd64/rtc.h>
#include <lrt/bare/arch/amd64/isr.h>
#include <lrt/bare/arch/amd64/serial.h>
#include <lrt/bare/arch/amd64/stdio.h>

FILE com1;

static inline void __attribute__ ((noreturn))
panic (void) {
  while(1)
    ;
}

idtdesc idt[256] __attribute__ ((aligned(8)));

static inline void 
init_idt(void)
{
  //setup idt
  for (int i = 0; i < 256; i++) {
    idt[i].raw[0] = 0;
    idt[i].raw[1] = 0;    
  }
  
  for (int i = 0; i < 256; i++) {
    idt[i].offset_low = (((uint64_t*)isrtbl)[i] & 0xFFFF);
    idt[i].offset_high = (((uint64_t*)isrtbl)[i] >> 16);
    idt[i].selector = 0x8; //Our code segment
    idt[i].ist = 0;
    idt[i].type = 0xe;
    idt[i].p = 1;
  }
  
  load_idtr(idt, sizeof(idt));
}

static inline void
clear_bss(void)
{
  extern uint8_t sbss[];
  extern uint8_t ebss[];
  for (uint8_t *i = sbss; i < ebss; i++) {
    *i = 0;
  }
}

void __attribute__ ((noreturn))
init64(multiboot_info_t *mbi) { 

  /* Zero out these segment selectors so we dont have issues later */
  __asm__ volatile (
		    "mov %w[zero], %%ds\n\t"
		    "mov %w[zero], %%es\n\t"
		    "mov %w[zero], %%ss\n\t"
		    "mov %w[zero], %%gs\n\t"
		    "mov %w[zero], %%fs\n\t"
		    :
		    :
		    [zero] "r" (0x0)
		    );

  /* serial init */
  serial_init(COM1, &com1);
  stdout = &com1;
  printf("Hello World!\n");

  if (!has_lapic()) {
    printf("No lapic support\n");
    panic();
  }
  
  init_idt();

  disable_pic();
  
  //Disable the pit, irq 0 could have fired and therefore
  //wouldn't have been masked and then we enable interrupts
  //so we must reset the PIT (and we may as well prevent it from firing)
  disable_pit();

  //Disable the rtc, irq 8 could have fired and therefore
  //wouldn't have been masked and then we enable interrupts
  //so we must disable it
  disable_rtc();

  enable_lapic();

  printf("apicid = %d\n", get_apicid());

  //TODO: need to finish moving this stuff into the apic header

  volatile uint32_t *apic_version_register = (uint32_t *)0x0FEE00030;
  uint32_t eoi_broadcast_suppression = *apic_version_register >> 24 & 0x1;
  uint32_t max_lvt_entry = *apic_version_register >> 16 & 0xFF;
  uint32_t apic_version = *apic_version_register & 0xFF;
  printf("apic_version = %d\n", apic_version);
  printf("max_lvt_entry = %d\n", max_lvt_entry);
  printf("eoi_broadcast_suppression = %d\n", eoi_broadcast_suppression);

  apic_icr_low tmp_icr_low;
  tmp_icr_low.raw = 0;
  tmp_icr_low.vector = 32; //just picked a vector
  tmp_icr_low.level = 1; //must be for a fixed ipi
  tmp_icr_low.destination_shorthand = 1; //self only

  apic_icr_high tmp_icr_high;
  tmp_icr_high.raw = 0;

  // IPI is fired on the lowest dword being set so we set high first
  volatile apic_icr_high *icr_high = (apic_icr_high *)0xFEE00310;
  volatile apic_icr_low *icr_low = (apic_icr_low *)0xFEE00300;
  icr_high->raw = tmp_icr_high.raw;
  icr_low->raw = tmp_icr_low.raw; //this should fire an IPI

  printf("before enabled interrupt\n");

  __asm__ volatile (
  		    "sti"
  		    );
  //test firing a second ipi, this can be a race but assuming the 
  //handler was called since we enabled interrupts,
  //the vector bit in the ISR should be set (no EOI yet)
  //so this sets the bit in the IRR (corresponding to the vector)
  icr_high->raw = tmp_icr_high.raw;
  icr_low->raw = tmp_icr_low.raw;

  volatile uint32_t *apic_eoi_register = (uint32_t *)0x0FEE000B0;
  //when we write to the eoi register it should clear the highest priority
  //bit in the ISR and therefore the pending interrupt in the IRR should
  //fire, causing our second IPI
  *apic_eoi_register = 0;
  
  panic();
}
