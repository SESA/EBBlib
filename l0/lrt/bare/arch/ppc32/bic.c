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

#include <l0/lrt/bare/arch/ppc32/bic.h>
#include <lrt/assert.h>

struct bg_irqctrl_group {
  volatile unsigned int status; // status (read and write) 0
  volatile unsigned int rd_clr_status; // status (read and clear) 4
  volatile unsigned int status_clr; // status (write and clear)8
  volatile unsigned int status_set; // status (write and set) c
  
  // 4 bits per IRQ
  volatile unsigned int target_irq[4]; // target selector 10-20
  volatile unsigned int noncrit_mask[4];// mask 20-30
  volatile unsigned int crit_mask[4]; // mask 30-40
  volatile unsigned int mchk_mask[4]; // mask 40-50
  
  unsigned char __align[0x80 - 0x50];
} __attribute__((packed));

struct bg_irqctrl {
  struct bg_irqctrl_group groups[15];
  volatile unsigned int core_non_crit[4];
  volatile unsigned int core_crit[4];
  volatile unsigned int core_mchk[4];
} __attribute__((packed));

static struct bg_irqctrl * const bg_irqctrl = 
  (struct bg_irqctrl *)0xfffde000;

void
bic_dump()
{
  for (int i = 0; i < 15; i++){
    printf("group: %d\n", i);
    printf("status = %x\n", bg_irqctrl->groups[i].status);
    printf("targets:\n");
    for (int j = 0; j < 4; j++) {
      printf("target %d = %x\n", j, bg_irqctrl->groups[i].target_irq[j]);
    }
  }
}

static inline uint8_t
irq_to_target_index(uint8_t irq) {
  return irq / 8; //division rounds towards zero
}

static inline void
set_target(uint8_t group, uint8_t irq, uint8_t target) {
  uint8_t offset = 28 - (irq * 4);
  uint32_t val = 
    bg_irqctrl->groups[group].target_irq[irq_to_target_index(irq)];
  val &= ~(0xf << offset); //mask off the correct bits
  val |= (target & 0xf) << offset; //set the correct bits appropriately
  bg_irqctrl->groups[group].target_irq[irq_to_target_index(irq)] = val;
}

void
bic_disable_and_clear_all()
{
  for (int i = 0; i < BIC_NUM_GROUPS; i++) {
    bg_irqctrl->groups[i].target_irq[0] = 0;
    bg_irqctrl->groups[i].target_irq[1] = 0;
    bg_irqctrl->groups[i].target_irq[2] = 0;
    bg_irqctrl->groups[i].target_irq[3] = 0;
    bg_irqctrl->groups[i].status = 0;    
  }
  asm volatile ("mbar");
}

void
bic_disable_irq(uint8_t group, uint8_t irq)
{
  LRT_Assert(group < BIC_NUM_GROUPS);
  LRT_Assert(irq < BIC_NUM_IRQS);
  set_target(group, irq, 0);
}

void
bic_enable_irq(uint8_t group, uint8_t irq, 
	       enum bic_int_type type, int8_t loc)
{
  LRT_Assert(group < BIC_NUM_GROUPS);
  LRT_Assert(irq < BIC_NUM_IRQS);
  LRT_Assert(loc >= -1 && loc <= 3);
  //set the target selector
  uint8_t target = type + 1;
  if (loc != -1) { //not broadcast
    target <<= 2;
    target |= loc;
  }
  set_target(group, irq, target);
}

void
bic_raise_irq(uint8_t group, uint8_t irq)
{
  LRT_Assert(group < BIC_NUM_GROUPS);
  LRT_Assert(irq < BIC_NUM_IRQS);
  bg_irqctrl->groups[group].status_set = 1 << (31 - irq);
}

void
bic_clear_irq(uint8_t group, uint8_t irq)
{
  LRT_Assert(group < BIC_NUM_GROUPS);
  LRT_Assert(irq < BIC_NUM_IRQS);
  bg_irqctrl->groups[group].status_clr = 1 << (31 - irq);
}
