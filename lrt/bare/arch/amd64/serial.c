
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

#include "sysio.h"

/* offsets from serial port address: */
/* when DLAB = 0 : */
static const uint16_t DATA_REG         = 0;   /* Data Register */
static const uint16_t INT_ENABLE       = 1;   /* Interrupt Enable Register */
/* when DLAB = 1 : */
static const uint16_t BAUD_DIV_LSB     = 0;   /* Least-signficant byte of the buad rate divsior */
static const uint16_t BAUD_DIV_MSB     = 1;   /* Most-signficant byte of the buad rate divsior */
/* Regardless of DLAB value : */
static const uint16_t INT_ID_FIFO      = 2;   /* Interrupt identification and fifo cntl registers */
static const uint16_t LINE_CNTL_REG    = 3;   /* Line control register (DLAB is the most signifcant bit) */
static const uint16_t MODEM_CNTL_REG   = 4;   /* Modem control register */
static const uint16_t LINE_STATUS_REG  = 5;   /* Line status register */
static const uint16_t MODEM_STATUS_REG = 6;   /* Modem status register */
static const uint16_t SCRATCH_REG      = 7;   /* Scratch register */


void lrt_serial_init(uint16_t out){
  uint8_t linectl;
  /* ensure interrupts are disabled */
  sysOut8(out+INT_ENABLE, 0);
  /* enable DLAB */
  sysOut8(out+LINE_CNTL_REG, (1<<7));
  /* set Divisor : */
  sysOut8(out+BAUD_DIV_LSB, 3);
  sysOut8(out+BAUD_DIV_MSB, 0);
  /* clear DLAB (Most sig bit = 0) */
  linectl = 0;
  /* 8 bit chars (2 least sig bits 1) : */
  linectl |= 3;
  /* for 1 stop bit, we leave bit #2 as 0 */
  /* leave bit #3 as 0 for no pairty */
  /* commit changes : */  
  sysOut8(out+LINE_CNTL_REG, linectl);
}

void lrt_serial_putch(uintptr_t out, uint8_t data){
  /* fifth bit of LINE_STATUS_REG, if set, indicates the line is ready. */
  while (!(sysIn8(out+LINE_STATUS_REG) & (1<<5)));
  sysOut8(out, data);
}