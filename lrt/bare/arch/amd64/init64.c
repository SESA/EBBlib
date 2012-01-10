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

#include <arch/amd64/cpu.h>
#include <arch/amd64/idt.h>
#include <arch/amd64/multiboot.h>
#include <arch/amd64/sysio.h>
#include <lrt/bare/arch/amd64/serial.h>
#include <lrt/bare/arch/amd64/stdio.h>
#include <lrt/bare/arch/amd64/isr.h>

idtdesc idt[256] __attribute__ ((aligned(8)));

FILE com1;

static inline void __attribute__ ((noreturn))
panic (void) {
  while(1)
    ;
}

typedef union {
  uint32_t raw;
  struct {
    uint32_t vector :8;
    uint32_t delivery_mode :3;
    uint32_t destination_mode :1;
    uint32_t delivery_status :1;    
    uint32_t reserved0 :1;
    uint32_t level :1;
    uint32_t trigger_mode :1;
    uint32_t reserved1 :2;
    uint32_t destination_shorthand :2;
    uint32_t reserved2 :12;
  };
} apic_icr_low;

_Static_assert(sizeof(apic_icr_low) == 4, "apic_icr_low packing issue");

typedef union {
  uint32_t raw;
  struct {
    uint32_t reserved :24;
    uint32_t destination :8;
  };
} apic_icr_high;

_Static_assert(sizeof(apic_icr_high) == 4, "apic_icr_high packing issue");

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

  /* clear bss */
  extern uint8_t sbss[];
  extern uint8_t ebss[];
  for (uint8_t *i = sbss; i < ebss; i++) {
    *i = 0;
  }

  /* serial init */
  serial_init(COM1, &com1);
  stdout = &com1;
  printf("Hello World!\n");
  
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
		    
  __asm__ volatile ("int $0x3");

  if (has_lapic()) {
    printf("Lapic support detected\n");
  }

  const uint16_t PIC_MASTER = 0x20;
  const uint16_t PIC_SLAVE = 0xa0;

  //Disable the pic by masking all irqs
  //OCW 4 to a pic with a0 set (hence the +1 addr) will mask irqs
  sysOut8(PIC_MASTER + 1, 0xFF);
  sysOut8(PIC_SLAVE + 1, 0xFF);

  const uint16_t PIT_CHANNEL_0 = 0x40;
  const uint16_t PIT_COMMAND = 0x43;
  
  //Disable the pit, irq 0 could have fired and therefore
  //wouldn't have been masked and then we enable interrupts
  //so we must reset the PIT (and we may as well prevent it from firing)
  //set the counter to zero
  sysOut8(PIT_CHANNEL_0, 0x00);
  //affecting channel 0, write to both hi/lo bytes, operate in mode 0, binary
  sysOut8(PIT_COMMAND, 0x30);

  //Disable the rtc, irq 8 could have fired and therefore
  //wouldn't have been masked and then we enable interrupts
  //so we must disable it
  const uint16_t CMOS_SELECT = 0x70;
  const uint16_t CMOS_REGISTER = 0x71;

  /* CMOS 0Bh - RTC - STATUS REGISTER B (read/write) */
  /* Bitfields for Real-Time Clock status register B: */
  /* Bit(s)  Description     (Table C002) */
  /*  7      enable cycle update */
  /*  6      enable periodic interrupt */
  /*  5      enable alarm interrupt */
  /*  4      enable update-ended interrupt */
  /*  3      enable square wave output */
  /*  2      Data Mode - 0: BCD, 1: Binary */
  /*  1      24/12 hour selection - 1 enables 24 hour mode */
  /*  0      Daylight Savings Enable - 1 enables */
  sysOut8(CMOS_SELECT, 0x0b);
  uint8_t status_b = sysIn8(CMOS_REGISTER);
  status_b &= ~0x70; //Mask off the three interrupts
  sysOut8(CMOS_SELECT, 0x0b);
  sysOut8(CMOS_REGISTER, status_b);
 
  //We read register C to clear out pending interrupts
  sysOut8(CMOS_SELECT, 0x0c);
  sysIn8(CMOS_REGISTER);

  enable_lapic();

  //volatile uint32_t *apic_base = (uint32_t *)0x0FEE0000;
  volatile uint32_t *apic_id_register = (uint32_t *)0x0FEE00020;
  uint32_t apicid = *apic_id_register >> 24;
  printf("apicid = %d\n", apicid);

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
