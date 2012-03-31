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

#include <arch/powerpc/cpu.h>
#include <arch/powerpc/regs.h>
#include <l0/lrt/bare/arch/ppc32/pic.h>
#include <lrt/io.h>

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

#define NUM_INTS (16)

#define EX_JUMP(interrupt)			    \
  extern void isr_ ## interrupt(void);		    \
  asm (						    \
       ".text;"					    \
       ".balign 4;"				    \
       "isr_" #interrupt ":;"			    \
       "nop;"					    \
       "mtspr " xstr(SPRN_SPRG2) ", 3;"		    \
       "li 3, " #interrupt ";"			    \
       "lis 20, ivpr_common@h;"			    \
       "ori 20, 20, ivpr_common@l;"		    \
       "mtctr 20;"				    \
       "bctr"					    \
      );

EX_JUMP(0) EX_JUMP(1) EX_JUMP(2) EX_JUMP(3) EX_JUMP(4)
EX_JUMP(5) EX_JUMP(6) EX_JUMP(7) EX_JUMP(8) EX_JUMP(9)
EX_JUMP(10) EX_JUMP(11) EX_JUMP(12) EX_JUMP(13) EX_JUMP(14)
EX_JUMP(15)

void *int_table[NUM_INTS] =
{
  isr_0, isr_1, isr_2, isr_3, isr_4, isr_5, isr_6, isr_7, isr_8,
  isr_9, isr_10, isr_11, isr_12, isr_13, isr_14, isr_15
};

uintptr_t lrt_pic_myid;

void __attribute__ ((noreturn))
lrt_pic_loop(void)
{
  // IPIs are 32 IRQs (all of group 0) on the BIC
  // Each IRQ can be invoked to trigger either a
  // non-critical, critical, or machine check interrupt
  // on either a particular core, or all cores

  //We set the first ipi irq to invoke a non-critical
  // interrupt on core 0
  bg_irqctrl->groups[0].target_irq[0] = 0x40000000;
  

  //Invoke the 0th ipi IRQ
  lrt_pic_ipi(0);

  msr msr = get_msr();
  msr.ee = 1; //enable external interrupts
  msr.we = 1; //enable wait state
  set_msr(msr);

  EBBAssert(0);
}

void __attribute__ ((noreturn))
lrt_pic_init(lrt_pic_handler h)
{
  lrt_pic_myid = get_spr(SPRN_PIR);

  //disable dec and watchdog interrupts
  //FIXME: make bit fields for this
  uint32_t tcr = get_spr(SPRN_TCR);
  tcr &= ~0x30;
  set_spr(SPRN_TCR, tcr);

  //mask and clear all interrupts on the BIC
  for (int i = 0; i < 15; i++) {
    bg_irqctrl->groups[i].target_irq[0] = 0;
    bg_irqctrl->groups[i].target_irq[1] = 0;
    bg_irqctrl->groups[i].target_irq[2] = 0;
    bg_irqctrl->groups[i].target_irq[3] = 0;
    bg_irqctrl->groups[i].status = 0;    
    asm volatile ("mbar");
  }  

  lrt_pic_mapvec(IV_external, h);
  lrt_pic_loop();
}

void
lrt_pic_mapvec(uintptr_t vec, lrt_pic_handler h)
{
  EBBAssert(vec < NUM_INTS);
  int_table[vec] = h;
}

void
lrt_pic_ipi(uintptr_t irq)
{
  bg_irqctrl->groups[0].status_set = 1 << (31 - irq);
}
