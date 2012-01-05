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
#include <arch/amd64/multiboot.h>
#include <lrt/bare/arch/amd64/serial.h>
#include <lrt/bare/arch/amd64/stdio.h>

FILE com1;

typedef struct {
  uint32_t reserved0;
  uint64_t rsp[3] __attribute__ ((packed));
  uint64_t reserved1;
  uint64_t ist[7] __attribute__ ((packed));
  uint64_t reserved2;
  uint16_t reserved3;
  uint16_t iopbm_offset;
} tss;

/* typedef struct { */
/*   uint64_t limit_low :16; */
/*   uint64_t base_low :24; */
/*   uint64_t type :4; */
/*   uint64_t  */

static inline void __attribute__ ((noreturn))
panic (void) {
  while(1)
    ;
}

void __attribute__ ((noreturn))
init64(multiboot_info_t *mbi) { 
  unsigned char *vram = (unsigned char *)0xb8000;
  vram[0] = 65;
  vram[1] = 0x07;
  serial_init(COM1, &com1);
  stdout = &com1;
  printf("Hello World!\n");
  
  //setup tss
  
  //setup idt


  panic();
}
