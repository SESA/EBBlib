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
#include <l0/lrt/bare/arch/ppc64/pic.h>

/* I only load 16 bits so I need this to be aligned so that
   there are only 16 significant bits. So I confirm that it
   is within the first 4GB (no longer than 32 bits) */
void *int_table[27] __attribute__ ((aligned(1 << 16)));

STATIC_ASSERT(&int_table <= 0xFFFFFFFFULL, 
	      "int_table not linked at appropriate location");

uintptr_t lrt_pic_myid;

typedef union {
  uint32_t val;
  struct {
    uint32_t type :5;
    uint32_t broadcast :1;
    uint32_t lpidtag :12;
    uint32_t pirtag :14;
  };
} doorbell;

void __attribute__ ((noreturn))
lrt_pic_loop(void)
{
  //Send an IPI to ourself
  lrt_pic_ipi(lrt_pic_myid);

  //enable external interrupts
  msr msr = get_msr();
  msr.ee = 1;
  set_msr(msr);

  while (1) {
    asm volatile("wait");
  }
}

void __attribute__ ((noreturn))
lrt_pic_init(lrt_pic_handler h)
{
  lrt_pic_myid = get_spr(SPRN_PIR);
  
  lrt_pic_mapipi(h);
  lrt_pic_loop();
}

void
lrt_pic_mapipi(lrt_pic_handler h)
{
  int_table[IV_processor_dbell] = h;
}

void
lrt_pic_ipi(uintptr_t id)
{
  doorbell db;
  db.val = 0;
  db.pirtag = id;

  asm volatile (
		"msgsnd %[db]"
		:
		: [db] "r" (db)
		);
}
