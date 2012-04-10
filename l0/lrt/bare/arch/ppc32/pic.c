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
#include <l0/lrt/bare/arch/ppc32/bic.h>
#include <l0/lrt/bare/arch/ppc32/pic.h>
#include <lrt/io.h>

#define NUM_INTS (16)

#define EX_JUMP(interrupt)			    \
  extern void isr_ ## interrupt(void);		    \
  asm (						    \
       ".text;"					    \
       ".balign 4;"				    \
       "isr_" #interrupt ":;"			    \
       "nop;"					    \
       "lis 3, ivpr_common@h;"			    \
       "ori 3, 3, ivpr_common@l;"		    \
       "mtctr 3;"				    \
       "li 3, " #interrupt ";"			    \
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
  
  //Invoke my ipi IRQ
  lrt_pic_ipi(lrt_pic_myid);

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

  bic_disable_and_clear_all();

  //Set the IPIs to fire noncritical external interrupt
  //to the corresponding core
  bic_enable_irq(BIC_IPI_GROUP, 0, NONCRIT, 0);
  bic_enable_irq(BIC_IPI_GROUP, 1, NONCRIT, 1);
  bic_enable_irq(BIC_IPI_GROUP, 2, NONCRIT, 2);
  bic_enable_irq(BIC_IPI_GROUP, 3, NONCRIT, 3);

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
  bic_raise_irq(BIC_IPI_GROUP, irq);
}

uintptr_t
lrt_pic_getnumlpics()
{
  EBBAssert(0);
}

uintptr_t
lrt_pic_getnextlpic(uintptr_t c)
{
  EBBAssert(0);
}

void
lrt_pic_ackipi()
{
  bic_clear_irq(BIC_IPI_GROUP, lrt_pic_myid);
}

void
lrt_pic_enableipi()
{
  bic_enable_irq(BIC_IPI_GROUP, lrt_pic_myid, NONCRIT, lrt_pic_myid);  
}
