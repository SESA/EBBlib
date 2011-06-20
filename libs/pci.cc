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

#include <l4io.h>
#include <arch/sysio.h>
#include <l4hal/pci.h>
#include <l4hal/e1000.h>

u8 e1000_bus;
u8 e1000_slot;
u16 e1000_func;

u32 pciConfigRead32 (u8 bus, u8 slot, u16 func, u16 offset) {
  sysOut32(0xCF8, 0x80000000 | (bus << 16) | (slot << 11) | (func << 8) |
	   offset);
  return sysIn32(0xCFC);
}

u16 pciConfigRead16 (u8 bus, u8 slot, u16 func, u16 offset) {
  sysOut32(0xCF8, 0x80000000 | (bus << 16) | (slot << 11) | (func << 8) |
	   offset);
  return sysIn16(0xCFC + (offset & 2));
}

u8 pciConfigRead8 (u8 bus, u8 slot, u16 func, u16 offset) {
  sysOut32(0xCF8, 0x80000000 | (bus << 16) | (slot << 11) | (func << 8) |
	   offset);
  return sysIn8(0xCFC + (offset & 3));
}

void pciConfigWrite32 (u8 bus, u8 slot, u16 func, u16 offset,
			     u32 val) {
  sysOut32(0xCF8, 0x80000000 | (bus << 16) | (slot << 11) | (func << 8) |
	   offset);
  sysOut32(0xCFC, val);
}

void pciConfigWrite16 (u8 bus, u8 slot, u16 func, u16 offset,
			     u16 val) {
  sysOut32(0xCF8, 0x80000000 | (bus << 16) | (slot << 11) | (func << 8) |
	   offset);
  sysOut32(0xCFC + (offset & 2), val);
}

void pciConfigWrite8 (u8 bus, u8 slot, u16 func, u16 offset,
			     u8 val) {
  sysOut32(0xCF8, 0x80000000 | (bus << 16) | (slot << 11) | (func << 8) |
	   offset);
  sysOut32(0xCFC + (offset & 3), val);
}
  

static void enumerateDevices(int bus) {
  u16 vendor;
  u16 device;
  u8 header;
  u8 dev_class;
  u8 subclass;
  u8  progif;
  int i;
 
  for (i = 0; i < 32; i++) {
    vendor = pciConfigRead16(bus,i,0,0);
    if (vendor == 0xFFFF)
      continue;
    device = pciConfigRead16(bus,i,0,2);
    if(vendor == 0x8086 && device == 0x100f) {
      e1000_bus = bus;
      e1000_slot = i;
      e1000_func = 0;
    }
    header = pciConfigRead8(bus,i,0,14);
    dev_class = pciConfigRead8(bus,i,0,11);
    subclass = pciConfigRead8(bus,i,0,10);
    progif = pciConfigRead8(bus,i,0,9);
    printf("Bus #%d, Device #%d, Vendor: 0x%x, Device: 0x%x, Header: 0x%x\n",
	   bus, i, vendor, device, header);
    printf("  Class code: 0x%x, Subclass: 0x%x, Prog IF: 0x%x\n",
	   dev_class, subclass, progif);
    if(header & 1) {
      enumerateDevices(pciConfigRead8(bus,i,0,25));
    }
  }
}

void pci_init() {
  int i;
  u32 bar;
  u16 command;

  enumerateDevices(0);
  printf("Command: 0x%x, Status: 0x%x\n",
	 pciConfigRead16(e1000_bus,e1000_slot,e1000_func,4), 
	 pciConfigRead16(e1000_bus,e1000_slot,e1000_func,6));
  for(i = 0; i < 6; i++) {
    bar = pciConfigRead32(e1000_bus,e1000_slot,e1000_func,16+4*i);
    printf("bar %d: 0x%x\n", i, bar);
  }

  command = pciConfigRead16(e1000_bus,e1000_slot,e1000_func,0x4);
  command |= 0x4;
  pciConfigWrite16(e1000_bus,e1000_slot,e1000_func,0x4,command);
  command = pciConfigRead16(e1000_bus,e1000_slot,e1000_func,0x4);
  printf("command = 0x%X\n",command);

  e1000_init();
}

