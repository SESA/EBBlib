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

#include <arch/amd64/idt.h>
#include <arch/amd64/apic.h>
#include <arch/amd64/pic.h>
#include <arch/amd64/pit.h>
#include <arch/amd64/rtc.h>
#include <l0/lrt/bare/arch/amd64/pic.h>
#include <lrt/bare/arch/amd64/isr.h>
#include <lrt/bare/arch/amd64/stdio.h>

//Vectors 0-31 are reserved by the processor
//We reserve vectors 32-63 for external interrupts, IPIs, etc.
//Vectors 64-255 are available
static const uint8_t IPI_VEC = 32;

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

void __attribute__ ((noreturn))
lrt_pic_loop(void)
{

  //Send ourselves an IPI
  lapic_icr_low icr_low;
  icr_low.raw = 0;
  icr_low.vector = lrt_pic_getIPIvec(); //just picked a vector
  icr_low.level = 1; //must be for a fixed ipi
  icr_low.destination_shorthand = 1; //self only

  lapic_icr_high icr_high;
  icr_high.raw = 0;


  send_ipi(icr_low, icr_high);
  
  //After we enable interrupts we just halt, an interrupt should wake us up
  //Once we finish the interrupt, we halt again and repeat
  
  __asm__ volatile("sti"); //enable interrupts
  while (1) {
    __asm__ volatile("hlt");
  }
}

void 
lrt_pic_init(lrt_pic_handler h)
{
  if (!has_lapic()) {
    printf("No lapic support, panicing\n");
    while(1)
      ;
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

  lrt_pic_mapipi(h);
  
  lrt_pic_loop();
}

void
lrt_pic_mapipi(lrt_pic_handler h)
{
  uint64_t ptr = (uint64_t)h;
  idt[lrt_pic_getIPIvec()].offset_low = (ptr & 0xFFFF);
  idt[lrt_pic_getIPIvec()].offset_high = (ptr >> 16);
  idt[lrt_pic_getIPIvec()].selector = 0x8; //Our code segment
  idt[lrt_pic_getIPIvec()].ist = 0;
  idt[lrt_pic_getIPIvec()].type = 0xe;
  idt[lrt_pic_getIPIvec()].p = 1;
}

uint8_t 
lrt_pic_getIPIvec()
{
  return IPI_VEC;
}
