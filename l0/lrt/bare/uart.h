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

#include <lrt/assert.h>

/* UART register offsets */
/* Receiver Buffer */
static const uint8_t UART_RBR = 0;
/* Transmitter holding */
static const uint8_t UART_THR = 0;
/* Interrupt Enable */
static const uint8_t UART_IER = 1;
/* Interrupt Identification */
static const uint8_t UART_IIR = 2;
/* FIFO Control */
static const uint8_t UART_FCR = 2;
/* Line Control */
static const uint8_t UART_LCR = 3;
/* Modem Control */
static const uint8_t UART_MCR = 4;
/* Line Status */
static const uint8_t UART_LSR = 5;
/* Modem Status */
static const uint8_t UART_MSR = 6;
/* Scratch */
static const uint8_t UART_SCR = 7;
/* With DLAB = 1 */
/* Divisor Latch LSB */
static const uint8_t UART_DLL = 0;
/* Divisor Latch MSB */
static const uint8_t UART_DLM = 1;


//FIXME: Bit field ordering is borked

/* typedef union { */
/*   uint8_t val; */
/*   struct { */
/*     /\* Received data available *\/ */
/*     uint8_t rda :1; */
/*     /\* Transmitter holding register empty *\/ */
/*     uint8_t thre :1; */
/*     /\* Receiver line status register change *\/ */
/*     uint8_t rlsrc :1; */
/*     /\* Modem status register change *\/ */
/*     uint8_t msrc :1; */
/*     /\* Sleep mode *\/ */
/*     uint8_t sm :1; */
/*     /\* Low power mode *\/ */
/*     uint8_t lpm :1; */
/*     uint8_t reserved :2; */
/*   }; */
/* } uart_ier; */

/* STATIC_ASSERT(sizeof(ier) == 1, "ier packing issue"); */

/* typedef union { */
/*   uint8_t val; */
/*   struct { */
/*     /\* Interrupt pending *\/ */
/*     uint8_t ip :1; */
/*     /\* Status *\/ */
/*     uint8_t s :3; */
/*     uint8_t reserved :1; */
/*     /\* 64 byte fifo enabled (16750 only)*\/ */
/*     uint8_t bfe :1; */
/*     /\* FIFO status *\/ */
/*     uint8_t fs :2; */
/*   }; */
/* } uart_iir; */

/* STATIC_ASSERT(sizeof(iir) == 1, "iir packing issue"); */

/* typedef union { */
/*   uint8_t val; */
/*   struct { */
/*     /\* Enable/disable *\/ */
/*     uint8_t ed :1; */
/*     /\* Clear receive *\/ */
/*     uint8_t cr :1; */
/*     /\* Clear transmit *\/ */
/*     uint8_t tr :1; */
/*     /\* DMA mode *\/ */
/*     uint8_t dm :1; */
/*     uint8_t reserved :1; */
/*     /\* enable 64 byte fifo (16750 only)*\/ */
/*     uint8_t ebf :1; */
/*     /\* Receive FIFO interrupt trigger level *\/ */
/*     uint8_t rfitl :2; */
/*   }; */
/* } uart_fcr; */

/* STATIC_ASSERT(sizeof(fcr) == 1, "fcr packing issue"); */

/* typedef union { */
/*   uint8_t val; */
/*   struct { */
/*     /\* Data word length *\/ */
/*     uint8_t dwl :2; */
/*     /\* Stop bits *\/ */
/*     uint8_t sb :1; */
/*     /\* Parity *\/ */
/*     uint8_t p :3; */
/*     /\* Break signal *\/ */
/*     uint8_t bs :1; */
/*     /\* DLAB accesible *\/ */
/*     uint8_t da :1; */
/*   }; */
/* } uart_lcr; */

/* STATIC_ASSERT(sizeof(lcr) == 1, "lcr packing issue"); */

/* typedef union { */
/*   uint8_t val; */
/*   struct { */
/*     /\* Data terminal ready *\/ */
/*     uint8_t dtr :1; */
/*     /\* Request to send *\/ */
/*     uint8_t rts :1; */
/*     /\* Auxiliary output 1 *\/ */
/*     uint8_t ao1 :1; */
/*     /\* Auxiliary output 2 *\/ */
/*     uint8_t ao2 :1; */
/*     /\* Loopback mode *\/ */
/*     uint8_t lm :1; */
/*     /\* Autoflow control (16750 only) *\/ */
/*     uint8_t ac :1; */
/*     uint8_t reserved :2; */
/*   }; */
/* } uart_mcr; */

/* STATIC_ASSERT(sizeof(mcr) == 1, "mcr packing issue"); */

typedef union {
  uint8_t val;
  struct {
    /* Erroneous data in FIFO */
    uint8_t edif :1;
    /* THR is empty, line is idle */
    uint8_t teli :1;
    /* THR is empty */
    uint8_t te :1;
    /* Break signal received */
    uint8_t bsr :1;
    /* Framing error */
    uint8_t fe :1;
    /* Parity error */
    uint8_t pe :1;
    /* Overrun error */
    uint8_t oe :1;
    /* Data available */
    uint8_t da :1;
  };
} uart_lsr;

STATIC_ASSERT(sizeof(lsr) == 1, "lsr packing issue");

/* typedef union { */
/*   uint8_t val; */
/*   struct { */
/*     /\* Change in Clear to send *\/ */
/*     uint8_t cicts :1; */
/*     /\* Change in Data set ready *\/ */
/*     uint8_t cidsr :1; */
/*     /\* Trailing edge Ring indicator *\/ */
/*     uint8_t teri :1; */
/*     /\* Change in Carrier detect *\/ */
/*     uint8_t cicd :1; */
/*     /\* Clear to send *\/ */
/*     uint8_t cts :1; */
/*     /\* Data set ready *\/ */
/*     uint8_t dsr :1; */
/*     /\* Ring indicator *\/ */
/*     uint8_t ri :1; */
/*     /\* Carrier detect *\/ */
/*     uint8_t cd :1; */
/*   }; */
/* } uart_msr; */

/* STATIC_ASSERT(sizeof(msr) == 1, "msr packing issue"); */

void uart_write(char *addr, const char *str, int len);
