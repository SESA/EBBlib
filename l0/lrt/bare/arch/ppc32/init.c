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
#include <arch/powerpc/mmu.h>
#include <arch/powerpc/450/mmu.h>
#include <arch/powerpc/regs.h>


struct bgp_mailbox_desc {
  uint16_t offset;	// offset from SRAM base
  uint16_t size;	// size including header, 0=not present
} __attribute__((packed));

typedef struct bgp_mailbox {
  volatile uint16_t command;	// comand; upper bit=ack
  uint16_t len;		// length (does not include header)
  uint16_t result;		// return code from reader
  uint16_t crc;		// 0=no CRC
  char data[0];
} bgp_mailbox;

#define BGP_MAILBOX_DESC_OFFSET		(0x7fd0)
#define BGP_DCR_TEST(x)			(0x400 + (x))
#define BGP_DCR_GLOB_ATT_WRITE_SET	BGP_DCR_TEST(0x17)
#define BGP_DCR_GLOB_ATT_WRITE_CLEAR	BGP_DCR_TEST(0x18)
#define BGP_DCR_TEST_STATUS6		BGP_DCR_TEST(0x3a)
#define   BGP_TEST_STATUS6_IO_CHIP	(0x80000000U >> 3)

#define BGP_ALERT_OUT(core)	        (0x80000000U >> (24 + core))
#define BGP_ALERT_IN(core)	        (0x80000000U >> (28 + core))

void __attribute__((noreturn))
init(void) 
{
  //sets our PID to 0
/*   set_spr(SPRN_PID, 0); */
  
  //sets MMUCR_STID to 0
/*   mmucr mmucr; */
/*   mmucr.val = get_spr(SPRN_MMUCR); //read MMUCR */
/*   mmucr.sts = 0; */
/*   mmucr.stid = 0; */
/*   set_spr(SPRN_MMUCR, mmucr); //set MMUCR */
  
/*   uint32_t entry; */
/*   asm volatile ("tlbsx %[ret], 0, %[addr];" */
/* 		: [ret] "=r" (entry) */
/* 		: [addr] "r" (&init) */
/* 		); */
  //entry now holds the TLB entry we are sitting on
  //This is horribly buggy, but lets just assume that the
  // next mapping is free to be mapped.

  //FIXME: get the mbox info from the dtree
/*   char *sram_mapping = (char *)0xD0000000; */
/*   tlb_word_0 t0; */
/*   t0.val = 0; */
/*   t0.epn = ((uintptr_t)sram_mapping) >> 10; */
/*   t0.v = 1; //valid */
/*   t0.ts = 0; */
/*   //SRAM is 32k but no 32K mapping on 450 */
/*   t0.size = 3; //64K mapping */

/*   //SRAM is at physical 0x7FFFF8000 */
/*   tlb_word_1 t1; */
/*   t1.val = 0; */
/*   t1.rpn = 0xFFFF8000 >> 10; //low 22 bits */
/*   t1.erpn = 0x7; //upper 4 bits */
  
/*   tlb_word_2 t2; */
/*   t2.val = 0; */
/*   t2.i = 1; //inhibit caching */
/*   t2.g = 1; //guarded access */
/*   //read/write, no execute */
/*   t2.sx = 0; */
/*   t2.sw = 1; */
/*   t2.sr = 1; */

/*   //write word 0 last because it validates the mapping */
/*   asm volatile ( */
/* 		"tlbwe %[t1], %[ind], 1;" */
/* 		"tlbwe %[t2], %[ind], 2;" */
/* 		"tlbwe %[t0], %[ind], 0;" */
/* 		"isync;" */
/* 		: */
/* 		: [ind] "b" (entry+1), */
/* 		  [t0] "r" (t0), */
/* 		  [t1] "r" (t1), */
/* 		  [t2] "r" (t2) */
/* 		); */

/*   struct bgp_mailbox_desc* mb_desc = (struct bgp_mailbox_desc *) */
/*     (0xfffff400); */
/*   mb_desc++; //first descriptor is in, we want out */
  bgp_mailbox *mbox = (bgp_mailbox *)(0xfffff400);
  //size of data buffer in mailbox
  /* uint32_t size = mb_desc->size - sizeof(struct bgp_mailbox); */

  mbox->data[0] = 'H';
  mbox->len = 1;
  mbox->command = 2;

  asm volatile (
		"mbar;"
		"mtdcrx %[dcrn], %[val];"
		:
		: [dcrn] "r" (BGP_DCR_GLOB_ATT_WRITE_SET),
		  [val] "r" (BGP_ALERT_OUT(0))
		);

  while(1);
}
