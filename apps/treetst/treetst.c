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

#include <stdbool.h>
#include <stdint.h>

#include <l1/App.h>
#include <lrt/exit.h>
#include <lrt/io.h>

CObject(TreeTst) {
  CObjInterface(App) *ft;
};

/* static void *channel0 = (void *)0xd0000000; */
/* static void *channel1 = (void *)0xd0002000; */

EBBRC 
Treetst_start(AppRef _self, int argc, char **argv, 
		 char **environ)
{
  uint32_t rdr0;
  asm volatile (
		"mfdcrx %[val], %[dcrn]"
		: [val] "=r" (rdr0)
		: [dcrn] "r" (0xc00)
		);
  rdr0 = (rdr0 >> 16) & 0xffff;

  uint32_t rdr15;
  asm volatile (
		"mfdcrx %[val], %[dcrn]"
		: [val] "=r" (rdr15)
		: [dcrn] "r" (0xc07)
		);
  rdr15 = rdr15 & 0xffff;
  
  if (!((rdr0 & 0x7000) && (rdr0 & 0x0700) &&
	(rdr15 & 0x7000) && (rdr15 & 0x0700))) {
    //Leaf Node on some route
    uint8_t *tree[2] = {(uint8_t *)0xd0000000, (uint8_t *)0xd0002000};
    //FIXME: This is not event driven, we just sit here and clean the tree
    while (1) {
      for (int i = 0; i < 2; i++) {
	uint32_t status = *(volatile uint32_t *)(tree[i] + 0x40);
	uint8_t rcv_hdr = status & 0xf;
	while (rcv_hdr > 0) {
	  *(volatile uint32_t *)(tree[i] + 0x30); //read header
	  for (int j = 0; j < 256; j += 16) {
	    asm volatile (
			  "lfpdx 0, 0, %[addr]"
			  :
			  : [addr] "b" (tree[i] + 0x20)
			  ); //read payload
	  }
	  rcv_hdr--;
	}
      }
    }
  } else {
    //Non-leaf Node
    asm volatile (
		  "mfdcrx %[val], %[dcrn]"
		  : [val] "=r" (rdr0)
		  : [dcrn] "r" (0xc00)
		  );
    
    rdr0 &= ~0x30000;
    
    asm volatile (
		  "mtdcrx %[dcrn], %[val]"
		  :
		  : [dcrn] "r" (0xc00),
		    [val] "r" (rdr0)
		  );
    
    asm volatile (
		  "mfdcrx %[val], %[dcrn]"
		  : [val] "=r" (rdr15)
		  : [dcrn] "r" (0xc07)
		  );
    
    rdr15 &= ~0x3;
    asm volatile (
		  "mtdcrx %[dcrn], %[val]"
		  :
		  : [dcrn] "r" (0xc07),
		    [val] "r" (rdr15)
		  );
  }
 /*  uint32_t rdr[8]; */
 /*  for (int i = 0; i < 8; i++) { */
 /*    uint32_t dcr; */
 /*    asm volatile ( */
 /* 		  "mfdcrx %[val], %[dcrn]" */
 /* 		  : [val] "=r" (dcr) */
 /* 		  : [dcrn] "r" (0xc00 + i) */
 /* 		  ); */
 /*    rdr[i] = dcr; */
 /*    dcr &= ~0x30003; //turn off local client source/target */
 /*    if (!(dcr & 0x70000000)) { //no source set */
 /*      dcr &= ~0x07000000; //disable targets */
 /*    } */
 /*    if (!(dcr & 0x07000000)) { //no target set */
 /*      dcr &= ~0x70000000; //disable source */
 /*    } */
 /*    if (!(dcr & 0x7000)) { //no source set */
 /*      dcr &= ~0x0700; //disable targets */
 /*    } */
 /*    if (!(dcr & 0x0700)) { //no target set */
 /*      dcr &= ~0x7000; //disable source */
 /*    } */
 /*    asm volatile ( */
 /* 		  "mtdcrx %[dcrn], %[val]" */
 /* 		  :  */
 /* 		  : [dcrn] "r" (0xc00 + i), */
 /* 		    [val] "r" (dcr) */
 /* 		  ); */
 /*  } */
 /*  uint32_t rstat; */
 /* loop: */
 /*  for (int i = 0; i < 1000; i++) { */
 /*    asm volatile ( */
 /* 		  "mfdcrx %[val], %[dcrn]" */
 /* 		  : [val] "=r" (rstat) */
 /* 		  : [dcrn] "r" (0xc13) */
 /* 		  ); */
 /*    if (!(rstat & 0x00ff0000)) { */
 /*      goto loop; */
 /*    } */
 /*  } */
 /*  uint32_t xstat; */
 /*  asm volatile ( */
 /* 		"mfdcrx %[val], %[dcrn]" */
 /* 		: [val] "=r" (xstat) */
 /* 		: [dcrn] "r" (0xc1f) */
 /* 		);   */

 /*  uint32_t ch0rstat; */
 /*  asm volatile ( */
 /* 		"mfdcrx %[val], %[dcrn]" */
 /* 		: [val] "=r" (ch0rstat) */
 /* 		: [dcrn] "r" (0xc20) */
 /* 		);   */
 /*  uint32_t ch1rstat; */
 /*  asm volatile ( */
 /* 		"mfdcrx %[val], %[dcrn]" */
 /* 		: [val] "=r" (ch1rstat) */
 /* 		: [dcrn] "r" (0xc28) */
 /* 		);   */
 /*  uint32_t ch2rstat; */
 /*  asm volatile ( */
 /* 		"mfdcrx %[val], %[dcrn]" */
 /* 		: [val] "=r" (ch2rstat) */
 /* 		: [dcrn] "r" (0xc30) */
 /* 		);   */
 /*  uint32_t ch0sstat; */
 /*  asm volatile ( */
 /* 		"mfdcrx %[val], %[dcrn]" */
 /* 		: [val] "=r" (ch0sstat) */
 /* 		: [dcrn] "r" (0xc22) */
 /* 		);   */
 /*  uint32_t ch1sstat; */
 /*  asm volatile ( */
 /* 		"mfdcrx %[val], %[dcrn]" */
 /* 		: [val] "=r" (ch1sstat) */
 /* 		: [dcrn] "r" (0xc2a) */
 /* 		);   */
 /*  uint32_t ch2sstat; */
 /*  asm volatile ( */
 /* 		"mfdcrx %[val], %[dcrn]" */
 /* 		: [val] "=r" (ch2sstat) */
 /* 		: [dcrn] "r" (0xc32) */
 /* 		);   */
 /*  lrt_printf("RSTAT = %x, XSTAT = %x\n", rstat, xstat); */
 /*  lrt_printf("c0r: %x, c0s: %x, c1r: %x, c1s: %x, c2r %x, c2s %x\n", */
 /* 	     ch0rstat, ch0sstat, ch1rstat, ch1sstat, ch2rstat, ch2sstat); */
 /*  for (int i = 0; i < 8; i++) { */
 /*    uint32_t dcr; */
 /*    asm volatile ( */
 /* 		  "mfdcrx %[val], %[dcrn]" */
 /* 		  : [val] "=r" (dcr) */
 /* 		  : [dcrn] "r" (0xc00 + i) */
 /* 		  ); */
 /*    lrt_printf("RDR %d: Orig = %x, New = %x\n", i, rdr[i], dcr); */
 /*  } */
  lrt_exit(0);
  return EBBRC_OK;
}

CObjInterface(App) TreeTst_ftable = {
  .start = Treetst_start
};

APP(TreeTst);

