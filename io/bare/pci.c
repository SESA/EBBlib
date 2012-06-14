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
  PCI_VENDOR_INTEL = 0x8086,
  PCI_VENDOR_VMWARE = 0x15ad,
  PCI_VENDOR_LSI = 0x1000
};

enum {
  PCI_INTEL_DEVID_HBRIDGE = 0x7190,
  PCI_INTEL_DEVID_PCIBRIDGE = 0x7191,
  PCI_INTEL_DEVID_ISABRIDGE = 0x7110, 
  PCI_INTEL_DEVID_ETHERNT = 0x100f
};

// credit to vm_device_version.h from vmware's 
enum {
  PCI_VMW_DEVID_SVGA2 = 0x0405,
  PCI_VMW_DEVID_SVGA = 0x0710,
  PCI_VMW_DEVID_NET = 0x0720,
  PCI_VMW_DEVID_SCSI = 0x0730,
  PCI_VMW_DEVID_VMCI = 0x0740,
  PCI_VMW_DEVID_CHIPSET = 0x1976,
  PCI_VMW_DEVID_82545EM = 0x0750, /* single port */
  PCI_VMW_DEVID_82546EB = 0x0760, /* dual port   */
  PCI_VMW_DEVID_EHCI = 0x0770,
  PCI_VMW_DEVID_1394 = 0x0780,
  PCI_VMW_DEVID_BRIDGE = 0x0790,
  PCI_VMW_DEVID_ROOTPORT = 0x07A0,
  PCI_VMW_DEVID_VMXNET3 = 0x07B0,
  PCI_VMW_DEVID_VMXWIFI = 0x07B8,
  PCI_VMW_DEVID_PVSCSI = 0x07C0
};
  
  

char *
vendor_name(int num)
{
  switch(num) {
  case PCI_VENDOR_INTEL:
    return "Intel";
    break;
  case PCI_VENDOR_VMWARE:
    return "VMware";
  case PCI_VENDOR_LSI:
    return "LSI";
  default:
    return "Unknown";
  }
}

char *
device_name(int vendor, int dev)
{
  switch(vendor){
  case PCI_VENDOR_INTEL:
    switch(dev) {
    case PCI_INTEL_DEVID_HBRIDGE:
      return "440BX/ZX AGPset Host Bridge";
    case PCI_INTEL_DEVID_PCIBRIDGE:
      return "440BX/ZX AGPset PCI-to-PCI bridge";
    case PCI_INTEL_DEVID_ISABRIDGE:
      return "PIIX4/4E/4M ISA Bridge";
    case PCI_INTEL_DEVID_ETHERNT:
      return "Gigabit Ethernet Controller (copper)";
    default:
      return "Unknown";
    }
    break;
  case PCI_VENDOR_VMWARE:
    switch(dev) {
    case PCI_VMW_DEVID_SVGA2:
      return "SVGA2";
    case PCI_VMW_DEVID_SVGA:
      return "SVGA";
    case PCI_VMW_DEVID_NET:
      return "NET";
    case PCI_VMW_DEVID_SCSI:
      return "SCSI";
    case PCI_VMW_DEVID_VMCI:
      return "VMCI";
    case PCI_VMW_DEVID_CHIPSET:
      return "CHIPSET";
    case PCI_VMW_DEVID_82545EM:
      return "82545EM";
    case PCI_VMW_DEVID_82546EB:
      return "82546EB";
    case PCI_VMW_DEVID_EHCI:
      return "EHCI";
    case PCI_VMW_DEVID_1394:
      return "1394";
    case PCI_VMW_DEVID_BRIDGE:
      return "BRIDGE";
    case PCI_VMW_DEVID_ROOTPORT:
      return "ROOTPORT";
    case PCI_VMW_DEVID_VMXNET3:
      return "VMXNET3";
    case PCI_VMW_DEVID_VMXWIFI:
      return "VMXWIFI";
    case PCI_VMW_DEVID_PVSCSI:
      return "PVSCSI";
    default:
      return "Unknown";
    }
  case PCI_VENDOR_LSI:
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

static void
parse_status(uint16_t status) 
{
  unsigned tmp;
  lrt_printf("\t\t\tstatus is %x\n", status);
  if (status & 1<<4) {
    lrt_printf("\t\t\t - has capability list\n");
  }
  if (status & 1<<7) {
    lrt_printf("\t\t\t - fast back to back capable\n");
  }
  if (status & 1<<8) {
    lrt_printf("\t\t\t - parity error\n");
  }
  if (status & 1<<15) {
    lrt_printf("\t\t\t - detected parity error\n");
  }
  tmp = ((status>>9) & 0x3);
  switch(tmp) {
  case 0:
    lrt_printf("\t\t\t - timing fast\n");
    break;
  case 1:
    lrt_printf("\t\t\t - timing medium\n");
    break;
  case 2:
    lrt_printf("\t\t\t - timing slow\n");
    break;
  }
  
}

static void 
print_vendor_dev(uint16_t vendor, uint16_t device)
{
  lrt_printf("\n\t\tVendor: (%x)%s, "
	     "\n\t\tDevice: (%x)%s\n",
	     vendor, vendor_name(vendor), 
	     device, device_name(vendor, device)
	     );
}

static void
print_capability_list(uint8_t bus, uint8_t slot)
{
  uint8_t ptr;
  uint8_t id;
  uint8_t nxt;
  ptr = (pci_config_read8(bus, slot, 0, 0x34) & ~0x3);
  while(ptr) {
    id = pci_config_read8(bus, slot, 0, ptr);
    nxt = (pci_config_read8(bus, slot, 0, ptr+1) & ~0x3);
    lrt_printf("\t\t\t - cap id %x, nxt %x :", id, nxt);
    switch(id) {
    case 1:
      lrt_printf(" - PCI power management\n");
      break;
    case 2:
      lrt_printf(" - AGP\n");
      break;
    case 3:
      lrt_printf(" - VPD\n");
      break;
    case 4:
      lrt_printf(" - slot identifiation\n");
      break;
    case 5:
      lrt_printf(" - MSI capable\n");
      break;
    case 6:
      lrt_printf(" - hot swap\n");
      break;
    case 0x11:
      lrt_printf(" - MSI-X capable\n");
      break;
    default:
      lrt_printf(" - ?????\n");
    }
    ptr = nxt;
  }
}

static void 
enumerateDevices(int bus) 
{
  uint16_t vendor;
  uint16_t device;
  uint16_t status;
  uint8_t header;
  uint8_t dev_class;
  uint8_t subclass;
  uint8_t  progif;
  int slot;

  lrt_printf("Enumerating bus %d\n", bus);
 
  for (slot = 0; slot < 32; slot++) {
    vendor = pci_config_read16(bus,slot,0,0);
    device = pci_config_read16(bus,slot,0,0x2);
    if (vendor == 0xFFFF)
      continue;
    status = pci_config_read16(bus,slot,0,0x6);
    header = pci_config_read8(bus,slot,0,0xe);
    dev_class = pci_config_read8(bus,slot,0,11);
    subclass = pci_config_read8(bus,slot,0,10);
    progif = pci_config_read8(bus,slot,0,9);
    lrt_printf("\tBus %d Slot %d header %x: ", bus, slot, header);
    if (header & 1<<7) {
      // its a multi-function device
      lrt_printf("(*MFD*) ");
    }
    switch( (header & 0x3F) ) {
    case 0: // standard device layout
      print_vendor_dev(vendor, device);
      lrt_printf("\t\tClass code: %x, Subclass: %x, Prog IF: %x\n",
		 dev_class, subclass, progif);
      parse_status(status);
      if (status & (1<<4)) {
	print_capability_list(bus, slot);
      }
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
