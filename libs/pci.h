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

#ifndef _PCI_H_
#define _PCI_H_

#include <l4hal/types.h>

//HARDCODED HACKS
/* #define E1000_BUS (2) */
/* #define E1000_DEVICE (0) */
/* #define E1000_FUNC (0) */

typedef struct {
  u8 bus;
  u8 slot;
  u8 func;
} PciConfig;

u32 pciConfigRead32(u8 bus, u8 slot, u16 func, u16 offset);
u16 pciConfigRead16(u8 bus, u8 slot, u16 func, u16 offset);
u8 pciConfigRead8(u8 bus, u8 slot, u16 func, u16 offset);
void pciConfigWrite32(u8 bus, u8 slot, u16 func, u16 offset, u32 val);
void pciConfigWrite16(u8 bus, u8 slot, u16 func, u16 offset, u16 val);
void pciConfigWrite8(u8 bus, u8 slot, u16 func, u16 offset, u8 val);

void pci_init(void);

#endif
