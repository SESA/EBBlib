#ifndef ARCH_AMD64_PIC_H
#define ARCH_AMD64_PIC_H

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

#include <arch/amd64/sysio.h>

static const uint16_t PIC_MASTER_COMMAND = 0x20;
static const uint16_t PIC_MASTER_DATA = 0x21;
static const uint16_t PIC_SLAVE_COMMAND = 0xa0;
static const uint16_t PIC_SLAVE_DATA = 0xa1;

static const uint8_t ICW1_ICW4 = 0x01;
static const uint8_t ICW1_INIT = 0x10;

static const uint8_t ICW4_8086 = 0x01;

static inline void
disable_pic()
{
  sysOut8(PIC_MASTER_COMMAND, ICW1_ICW4 + ICW1_INIT);
  sysOut8(PIC_SLAVE_COMMAND, ICW1_ICW4 + ICW1_INIT);

  sysOut8(PIC_MASTER_DATA, 0x10);
  sysOut8(PIC_SLAVE_DATA, 0x18);

  sysOut8(PIC_MASTER_DATA, 1 << 2);
  sysOut8(PIC_SLAVE_DATA, 2);

  sysOut8(PIC_MASTER_DATA, ICW4_8086);
  sysOut8(PIC_SLAVE_DATA, ICW4_8086);

  //Disable the pic by masking all irqs
  //OCW 4 to a pic with a0 set (hence the +1 addr) will mask irqs
  //We set all bits to mask all the irqs
  sysOut8(PIC_MASTER_DATA, 0xff);
  sysOut8(PIC_SLAVE_DATA, 0xff);
}

#endif
