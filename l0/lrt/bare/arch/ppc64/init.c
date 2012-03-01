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

#include <arch/ppc64/cpu.h>
#include <arch/ppc64/regs.h>
#include <l0/lrt/bare/uart.h>
#include <l0/lrt/bare/arch/ppc64/lrt_start.h>
#include <l0/lrt/bare/arch/ppc64/pic.h>
#include <lrt/assert.h>

extern uint8_t _vec_start[];
extern uint8_t _start[];

static inline void __attribute__ ((noreturn))
panic (void) {
  asm volatile("attn");
}

static inline void
clear_bss(void) {
  extern uint8_t sbss[];
  extern uint8_t ebss[];
  for (uint8_t *i = sbss; i < ebss; i++) {
    *i = 0;
  }
}

//This is so I can write to _start without a complaint
// that it should never be NULL
#pragma GCC diagnostic ignored "-Waddress"

void __attribute__ ((noreturn))
init(void)
{
  /* setup IVPR */
  set_ivpr(_vec_start);

  //start at medium priority
  asm volatile ("or 2, 2, 2");

  //setup MSR
  msr msr = get_msr();

  //enable machine check
  msr.me = 1;

  //make sure external interrupts are off
  msr.ee = 0;

  //enable fpu
  msr.fp = 1;

  //enable 64 bit mode
  msr.cm = 1;
  
  //set interrupt compute mode
  epcr epcr;
  epcr.val = 0;
  epcr.icm = 1;
  set_spr(SPRN_EPCR, epcr);

  //write msr
  set_msr(msr);

  //attn on null pointer jump
  if (_start == 0) {
    asm volatile (
  		  "stw %[attn], 0(0)\n\t"
  		  "dcbst 0, %[zero]\n\t"
  		  "sync\n\t"
  		  "icbi 0, %[zero]\n\t"
  		  "isync"
  		  :
  		  : [attn] "r" (0x200), //opcode for attn
  		    [zero] "r" (0)
  		  );
  }

  clear_bss();

  lrt_pic_init(lrt_start_isr);

}
