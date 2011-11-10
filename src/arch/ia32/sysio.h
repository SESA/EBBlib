/* Copyright 2011 Boston University. All rights reserved. */

/* Redistribution and use in source and binary forms, with or without modification, are */
/* permitted provided that the following conditions are met: */

/*    1. Redistributions of source code must retain the above copyright notice, this list of */
/*       conditions and the following disclaimer. */

/*    2. Redistributions in binary form must reproduce the above copyright notice, this list */
/*       of conditions and the following disclaimer in the documentation and/or other materials */
/*       provided with the distribution. */

/* THIS SOFTWARE IS PROVIDED BY BOSTON UNIVERSITY ``AS IS'' AND ANY EXPRESS OR IMPLIED */
/* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND */
/* FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BOSTON UNIVERSITY OR */
/* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR */
/* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR */
/* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF */
/* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

/* The views and conclusions contained in the software and documentation are those of the */
/* authors and should not be interpreted as representing official policies, either expressed */
/* or implied, of Boston University */

#ifndef _SYSIO_H_
#define _SYSIO_H_

#include <l4hal/types.h>

static inline u32 sysIn32 (u16 port) {
  u32 ret;
  __asm__ volatile ("inl %w1,%0":"=a"(ret) : "Nd"(port));
  return ret;
}

static inline u16 sysIn16 (u16 port) {
  u16 ret;
  __asm__ volatile ("inw %w1,%w0":"=a"(ret) : "Nd"(port));
  return ret;
}

static inline u8 sysIn8 (u16 port) {
  u16 ret;
  __asm__ volatile ("inb %w1,%b0":"=a"(ret) : "Nd"(port));
  return ret;
}

static inline void sysOut32 (u16 port, u32 val) {
  __asm__ volatile ("outl %0,%w1"::"a"(val), "Nd" (port));
}

static inline void sysOut16 (u16 port, u16 val) {
  __asm__ volatile ("outw %w0,%w1"::"a"(val), "Nd" (port));
}

static inline void sysOut8 (u16 port, u8 val) {
  __asm__ volatile ("outb %b0,%w1"::"a"(val), "Nd" (port));
}

#endif
