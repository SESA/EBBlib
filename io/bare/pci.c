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

#include <config.h>
#include <lrt/io.h>
#include <arch/amd64/sysio.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

uint32_t 
pci_config_read32 (uint8_t bus, uint8_t slot, uint16_t func, uint16_t offset) 
{
  sysOut32(PCI_CONFIG_ADDRESS, 0x80000000 | (bus << 16) | (slot << 11) | 
	   (func << 8) | offset);
  return sysIn32(PCI_CONFIG_DATA);
}

uint16_t 
pci_config_read16 (uint8_t bus, uint8_t slot, uint16_t func, uint16_t offset) 
{ 
  sysOut32(PCI_CONFIG_ADDRESS, 0x80000000 | (bus << 16) | (slot << 11) | 
	   (func << 8) | offset);
  return sysIn16(PCI_CONFIG_DATA + (offset & 2));
}

uint8_t 
pci_config_read8 (uint8_t bus, uint8_t slot, uint16_t func, uint16_t offset) 
{
  sysOut32(PCI_CONFIG_ADDRESS, 0x80000000 | (bus << 16) | (slot << 11) | 
	   (func << 8) | offset);
  return sysIn8(PCI_CONFIG_DATA + (offset & 3));
}

void 
pci_config_write32 (uint8_t bus, uint8_t slot, uint16_t func, uint16_t offset, 
		    uint32_t val) 
{
  sysOut32(PCI_CONFIG_ADDRESS, 0x80000000 | (bus << 16) | (slot << 11) | 
	   (func << 8) | offset);
  sysOut32(PCI_CONFIG_DATA, val);
}

void 
pci_config_write16 (uint8_t bus, uint8_t slot, uint16_t func, uint16_t offset, 
		    uint16_t val) 
{
  sysOut32(PCI_CONFIG_ADDRESS, 0x80000000 | (bus << 16) | (slot << 11) | 
	   (func << 8) | offset);
  sysOut32(PCI_CONFIG_DATA + (offset & 2), val);
}

void 
pci_config_write8 (uint8_t bus, uint8_t slot, uint16_t func, uint16_t offset, 
		   uint8_t val) 
{
  sysOut32(PCI_CONFIG_ADDRESS, 0x80000000 | (bus << 16) | (slot << 11) |
	   (func << 8) | offset);
  sysOut32(PCI_CONFIG_DATA + (offset & 3), val);
}

enum {
  VENDOR_INTEL = 0x8086,
  VENDOR_VMWARE = 0x15ad,
  VENDOR_LSI = 0x1000
};

char *
vendor_name(int num)
{
  switch(num) {
  case VENDOR_INTEL:
    return "Intel";
    break;
  case VENDOR_VMWARE:
    return "VMware";
  case VENDOR_LSI:
    return "LSI";
  default:
    return "Unknown";
  }
}

char *
device_name(int vendor, int dev)
{
  switch(vendor){
  case VENDOR_INTEL:
    switch(dev) {
    case 0x7190:
      return "440BX/ZX AGPset Host Bridge";
    case 0x7191:
      return "440BX/ZX AGPset PCI-to-PCI bridge";
    case 0x7110:
      return "PIIX4/4E/4M ISA Bridge";
    case 0x100f:
      return "Gigabit Ethernet Controller (copper)";
    default:
      return "Unknown";
    }
    break;
  case VENDOR_VMWARE:
    switch(dev) {
    case 0x405:
      return "NVIDIA";
    case 0x770:
      return "Standard Enhanced PCI to USB Host Controller";
    case 0x7B0:
      return "VMXNet 3";
    case 0x720:
      return "VMXNet";
    default:
      return "Unknown";
    }
  case VENDOR_LSI:
    switch(dev) {
    case 0x30:
      return "PCI-X SCSI Controller";
      break;
    default:
      return "Unknown";
    }
  default:
    return "Unknown";
  }
}

static void enumerateDevices(int bus) {
  uint16_t vendor;
  uint16_t device;
  uint8_t header;
  uint8_t dev_class;
  uint8_t subclass;
  uint8_t  progif;
  int slot;

  lrt_printf("Enumerating bus %d\n", bus);
 
  for (slot = 0; slot < 32; slot++) {
    vendor = pci_config_read16(bus,slot,0,0);
    if (vendor == 0xFFFF)
      continue;
    device = pci_config_read16(bus,slot,0,2);
    if(vendor == 0x8086 && device == 0x100f) {
      // e1000_bus = bus;
      //e1000_slot = i;
      // e1000_func = 0;
    }
    header = pci_config_read8(bus,slot,0,0xe);
    dev_class = pci_config_read8(bus,slot,0,11);
    subclass = pci_config_read8(bus,slot,0,10);
    progif = pci_config_read8(bus,slot,0,9);
    lrt_printf("\tBus %d Slot %d header %x: ", bus, slot, header);
    switch( header ) {
    case 0: // standard device layout
      lrt_printf("\n\t\tVendor: (%x)%s, "
		 "\n\t\tDevice: (%x)%s\n",
		 vendor, vendor_name(vendor), 
		 device, device_name(vendor, device)
		 );
      lrt_printf("\t\tClass code: %x, Subclass: %x, Prog IF: %x\n",
		 dev_class, subclass, progif);
      break;
    case 0x1: 
      {
	uint8_t sbus = pci_config_read8(bus,slot,0,25);
	lrt_printf(" - PCI bridge, subbus is %d\n", sbus);
	enumerateDevices(sbus);
	lrt_printf("Back to bus %d\n", bus);
      }
      break;
    case 0x2: 
      lrt_printf(" -  Card Bus bridge\n");
      break;
    default:
      lrt_printf("- Unknown header\n");
    }
  }

}

void pci_init() 
{
  enumerateDevices(0);
#if 0
  int i;
  uint32_t bar;
  uint16_t command;
  lrt_printf("Command: %x, Status: %x\n",
	 pci_config_read16(e1000_bus,e1000_slot,e1000_func,4), 
	 pci_config_read16(e1000_bus,e1000_slot,e1000_func,6));
  for(i = 0; i < 6; i++) {
    bar = pci_config_read32(e1000_bus,e1000_slot,e1000_func,16+4*i);
    printf("bar %d: 0x%x\n", i, bar);
  }

  command = pci_config_read16(e1000_bus,e1000_slot,e1000_func,0x4);
  command |= 0x4;
  pciConfigWrite16(e1000_bus,e1000_slot,e1000_func,0x4,command);
  command = pci_config_read16(e1000_bus,e1000_slot,e1000_func,0x4);
  lrt_printf("command = %X\n",command);

  e1000_init();
#endif
}
