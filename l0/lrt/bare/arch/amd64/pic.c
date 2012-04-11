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

#include <config.h>

#include <stdint.h>

#include <arch/amd64/apic.h>
#include <arch/amd64/idt.h>
#include <arch/amd64/ioapic.h>
#include <arch/amd64/pic.h>
#include <arch/amd64/pit.h>
#include <arch/amd64/rtc.h>
#include <l0/lrt/bare/arch/amd64/acpi.h>
#include <l0/lrt/bare/arch/amd64/isr.h>
#include <l0/lrt/bare/arch/amd64/pic.h>
#include <lrt/assert.h>
#include <lrt/io.h>

uintptr_t lrt_pic_myid;

//Vectors 0-31 are reserved by the processor
//We reserve vectors 32-63 for external interrupts, IPIs, etc.
//Vectors 64-255 are available
static const uint8_t IPI_VEC = 32;

idtdesc idt[256] __attribute__ ((aligned(8)));

static inline void
idt_map_vec(uint8_t vec, lrt_pic_handler h) {
  idt[vec].offset_low = ((uintptr_t)h) & 0xFFFF;
  idt[vec].offset_high = ((uintptr_t)h) >> 16;
  idt[vec].selector = 0x8; //Our code segment
  idt[vec].ist = 0;
  idt[vec].type = 0xe;
  idt[vec].p = 1;
}  

static inline void 
init_idt(void)
{
  //setup idt
  for (int i = 0; i < 256; i++) {
    idt[i].raw[0] = 0;
    idt[i].raw[1] = 0;    
  }
  
  for (int i = 0; i < 256; i++) {
    idt_map_vec(i, isrtbl[i]);
  }
  
  load_idtr(idt, sizeof(idt));
}

void __attribute__ ((noreturn))
lrt_pic_loop(void)
{  

  //Send an IPI to ourself
  lrt_pic_ipi(lrt_pic_myid);

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
  LRT_Assert(has_lapic());
  
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

  acpi_init();

  enable_lapic();
  lrt_pic_myid = get_lapic_id();

  lrt_pic_mapipi(h);
  
  lrt_pic_loop();
}

void
lrt_pic_mapipi(lrt_pic_handler h)
{
  idt_map_vec(lrt_pic_getIPIvec(), h);
}

uint8_t 
lrt_pic_getIPIvec()
{
  return IPI_VEC;
}

intptr_t
lrt_pic_ipi(uintptr_t id)
{
  lapic_icr_low icr_low;
  icr_low.raw = 0;
  icr_low.vector = lrt_pic_getIPIvec(); //just picked a vector
  icr_low.level = 1; //must be for a fixed ipi
  icr_low.destination_shorthand = 0; //no shorthand

  lapic_icr_high icr_high;
  icr_high.raw = 0;
  icr_high.destination = id;

  send_ipi(icr_low, icr_high);  
  return 1;
}

intptr_t
lrt_pic_mapvec(lrt_pic_src *s, uintptr_t vec, lrt_pic_handler h)
{
  lrt_printf("%s: untested code!!!\n", __func__);
  lrt_printf("%s: Also, no SMP support\n", __func__);
  LRT_Assert(0);

  LRT_Assert((vec >= 0) && (vec < lrt_pic_numvec()));
  idt_map_vec(vec, h);
  ioapic_map_vec(*s, vec);
  ioapic_enable_interrupt(*s);
  
  return 0;
}

uintptr_t
lrt_pic_numvec()
{
  return 256;
}

uintptr_t 
lrt_pic_getnumlpics()
{
  lrt_printf("%s: NYI\n", __func__);
  LRT_Assert(0);
}

intptr_t lrt_pic_vecon(uintptr_t vec)
{
  lrt_printf("%s: NYI\n", __func__);
  LRT_Assert(0);
}

intptr_t lrt_pic_vecoff(uintptr_t vec)
{
  lrt_printf("%s: NYI\n", __func__);
  LRT_Assert(0);
} 

uintptr_t 
lrt_pic_getnextlpic(uintptr_t c)
{
  lrt_printf("%s: NYI\n", __func__);
  LRT_Assert(0);
}

intptr_t
lrt_pic_allocvec(uintptr_t *vec)
{
  lrt_printf("%s: NYI\n", __func__);
  LRT_Assert(0);
}

void
lrt_pic_ackipi()
{
  send_eoi();
}

void
lrt_pic_enable(uintptr_t vec)
{
  LRT_Assert(vec < lrt_pic_numvec());
  idt[vec].p = 1;
}

void
lrt_pic_enableipi()
{
  lrt_pic_enable(IPI_VEC);
}
