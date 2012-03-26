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
#include <l0/lrt/bare/stdio.h>

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

#define BGP_DCR_TEST(x)			(0x400 + (x))
#define BGP_DCR_GLOB_ATT_WRITE_SET	BGP_DCR_TEST(0x17)
#define BGP_DCR_GLOB_ATT_WRITE_CLEAR	BGP_DCR_TEST(0x18)
#define BGP_DCR_TEST_STATUS6		BGP_DCR_TEST(0x3a)
#define   BGP_TEST_STATUS6_IO_CHIP	(0x80000000U >> 3)

#define BGP_ALERT_OUT(core)	        (0x80000000U >> (24 + core))
#define BGP_ALERT_IN(core)	        (0x80000000U >> (28 + core))

//FIXME: read from FDT, these are hardcoded
static bgp_mailbox * const bgp_mbox = (bgp_mailbox *)(0xfffff400);
static uintptr_t bgp_mbox_size = 0xf8;

static int
mailbox_putc(int c)
{
  bgp_mbox->data[bgp_mbox->len++] = c;
  
  if (bgp_mbox->len >= bgp_mbox_size || c == '\n') {
    bgp_mbox->command = 2;
    asm volatile (
		  "mbar;"
		  "mtdcrx %[dcrn], %[val];"
		  :
		  : [dcrn] "r" (BGP_DCR_GLOB_ATT_WRITE_SET),
		    [val] "r" (BGP_ALERT_OUT(0))
		  );
    do {
      asm volatile ("dcbi 0, %[addr]"
		    :
		    : [addr] "b" (&bgp_mbox->command)
		    );
      //MSBit is ack bit
    } while (!bgp_mbox->command & 0x8000);
  }
  return c;
}

static int
mailbox_write(uintptr_t cookie, const char *str, int len) 
{
  for (int i = 0; i < len; i++) {
    mailbox_putc((int)str[i]);
  }
  return 0;
}

FILE mailbox = {
  .cookie = 0,
  .write = mailbox_write
};

FILE *
mailbox_init()
{
  return &mailbox;
}
