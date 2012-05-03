#ifndef __LRT_EVENT_H__
#define __LRT_EVENT_H__
/*
 * Copyright (C) 2012 by Project SESA, Boston University
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

#define LRT_ULNX_PICFLAG_READ  (1<<0)  // READ:  BIT 0
#define LRT_ULNX_PICFLAG_WRITE (1<<1)  // WRITE: BIT 1
#define LRT_ULNX_PICFLAG_ERROR (1<<2)  // ERRORL BIT 2

struct IRQ_t {
  union {
    uint64_t raw;
    struct {
      uint32_t fd;
      uint32_t flags;
    } unix_pic_src;
  };
};


// FIXME: change to event interface, and put in platform specific includes
typedef void (*lrt_pic_handler)(void);

extern intptr_t lrt_pic_add_core();
extern intptr_t lrt_pic_init(lrt_pic_handler h);

#endif
