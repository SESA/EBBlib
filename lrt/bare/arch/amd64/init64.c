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
#include <arch/amd64/idt.h>
#include <arch/amd64/multiboot.h>
#include <lrt/bare/arch/amd64/serial.h>
#include <lrt/bare/arch/amd64/stdio.h>

idtdesc idt[256] __attribute__ ((aligned(8)));

FILE com1;

static inline void __attribute__ ((noreturn))
panic (void) {
  while(1)
    ;
}

void __attribute__ ((noreturn))
init64(multiboot_info_t *mbi) { 
  
  /* clear bss */
  extern uint8_t sbss[];
  extern uint8_t ebss[];
  for (uint8_t *i = sbss; i < ebss; i++) {
    *i = 0;
  }

  /* serial init */
  serial_init(COM1, &com1);
  stdout = &com1;
  printf("Hello World!\n");
  
  if (has_1gpages()) {
    printf("has 1g pages\n");
  }

  //setup idt
  for (int i = 0; i < 256; i++) {
    idt[i].raw[0] = 0;
    idt[i].raw[1] = 0;    
  }

  for (int i = 0; i < 256; i++) {
    //TODO DS: add symbol of generic handler instead of 0
    idt[i].offset_low = ((uint64_t)0 & 0xFFFF);
    idt[i].offset_high = ((uint64_t)0 >> 16);
    idt[i].selector = 0x8; //Our code segment
    idt[i].ist = 0;
    idt[i].type = 0xe;
    idt[i].p = 1;
  }
  
  load_idtr(idt, sizeof(idt));

  panic();
}
