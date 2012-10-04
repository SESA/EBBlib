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
#include <l0/EBBMgrPrim.h>
#include <io/bare/pci.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC
#define PCI_CAP_LIST_OFFSET 0x34

#define PCI_CAP_TYPE_PM  1	/* power management */
#define PCI_CAP_TYPE_AGP 2
#define PCI_CAP_TYPE_VPD 3
#define PCI_CAP_TYPE_SLI 4
#define PCI_CAP_TYPE_MSI 5
#define PCI_CAP_TYPE_HSW 6
#define PCI_CAP_TYPE_MSIX 0x11

static uint32_t 
_pci_config_read32 (uint8_t bus, uint8_t slot, uint16_t func, uint16_t offset) 
{
  sysOut32(PCI_CONFIG_ADDRESS, 0x80000000 | (bus << 16) | (slot << 11) | 
	   (func << 8) | offset);
  return sysIn32(PCI_CONFIG_DATA);
}

uint32_t 
pci_config_read32(struct pci_info *pi, uint16_t offset)
{
  return _pci_config_read32(pi->bus, pi->slot, pi->func, offset);
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
    case PCI_INTEL_DEVID_E1000E:
      return "Intel® 82574L Gigabit Ethernet Controller	";
    case PCI_INTEL_DEVID_GBADPT:
      return "82576 Gigabit ET Dual Port Server Adapter";
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
parse_msix_capability(uint8_t bus, uint8_t slot, uint8_t ptr)
{
  uint32_t taboffset, pbaoffset, bar;
  taboffset = _pci_config_read32 (bus, slot, 0, ptr+4);
  pbaoffset = _pci_config_read32 (bus, slot, 0, ptr+8);
  // if bottom bit of bar is 0, then its memory mapped the tables
  // if bit 1 is 0, then its a 32 bit address
  bar = _pci_config_read32( bus, slot, 0, 0x10);
  lrt_printf("\t\t\t msix taboffset %x, pbaoffset %x bar %x\n", 
	     taboffset, pbaoffset, bar);
}

static uint8_t 
find_capability(uint8_t bus, uint8_t slot, uint8_t type)
{
  uint8_t ptr;
  uint8_t id;
  uint8_t nxt;
  ptr = (pci_config_read8(bus, slot, 0, PCI_CAP_LIST_OFFSET) & ~0x3);
  while(ptr) {
    id = pci_config_read8(bus, slot, 0, ptr);
    nxt = (pci_config_read8(bus, slot, 0, ptr+1) & ~0x3);
    if (id == type) return ptr;
    ptr = nxt;
  };
  return 0;
}


// enable msix for this device
void 
pci_enable_msix(struct pci_info *pi)
{
  uint8_t ptr;
  uint16_t tmp;
  ptr = find_capability(pi->bus, pi->slot, PCI_CAP_TYPE_MSIX);
  // oops, couldn't find capability 

  LRT_Assert(ptr!=0);		
  lrt_printf("found msix capability, ptr is %x, enabling\n", ptr);
  lrt_printf("note for MSIX on ether should be A0\n");
  tmp = pci_config_read16(pi->bus, pi->slot, 0, ptr+2);
  tmp &= ~(1<<14);		/* turning off function mask */
  tmp |= (1<<15);		/* enable interrupts */
  pci_config_write16(pi->bus, pi->slot, 0, ptr+2, tmp);
  tmp = pci_config_read16(pi->bus, pi->slot, 0, ptr+2);
  lrt_printf("enabled msix on device\n");
} 

// enable msi for this device
void 
pci_enable_msi(struct pci_info *pi, uint32_t mal, uint32_t mau, 
	       uint16_t mad)
{
  uint8_t ptr;
  uint16_t tmp;
  ptr = find_capability(pi->bus, pi->slot, PCI_CAP_TYPE_MSI);
  // oops, couldn't find capability 
  LRT_Assert(ptr!=0);		
  lrt_printf("found msi capability, ptr is %x, enabling\n", ptr);
  lrt_printf("note for MSI on ether should be D0\n");

  lrt_printf("writting lower address to %x\n", ptr+4);
  pci_config_write32(pi->bus, pi->slot, 0, ptr+4, mal);
  lrt_printf("writting upper address to %x\n", ptr+8);
  pci_config_write32(pi->bus, pi->slot, 0, ptr+8, mau);
  lrt_printf("writting data to %x\n", ptr+12);
  pci_config_write16(pi->bus, pi->slot, 0, ptr+12, mad);
  

  tmp = pci_config_read16(pi->bus, pi->slot, 0, ptr+2);
  tmp |= (1);		/* enable interrupts */
  pci_config_write16(pi->bus, pi->slot, 0, ptr+2, tmp);
} 

// tell pci this device can be a bus master
void
pci_enable_bus_master(struct pci_info *pi)
{
  uint16_t command;

  command = pci_config_read16(pi->bus,pi->slot,pi->func,0x4);
  command |= 0x4;
  pci_config_write16(pi->bus,pi->slot,pi->func,0x4,command);
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
print_capability_list(uint8_t bus, uint8_t slot, int isabus)
{
  uint8_t ptr;
  uint8_t id;
  uint8_t nxt;
  ptr = (pci_config_read8(bus, slot, 0, PCI_CAP_LIST_OFFSET) & ~0x3);
  while(ptr) {
    id = pci_config_read8(bus, slot, 0, ptr);
    nxt = (pci_config_read8(bus, slot, 0, ptr+1) & ~0x3);
    lrt_printf("\t\t\t - cap id %x, nxt %x :", id, nxt);
    switch(id) {
    case PCI_CAP_TYPE_PM:
      lrt_printf(" - PCI power management\n");
      break;
    case PCI_CAP_TYPE_AGP:
      lrt_printf(" - AGP\n");
      break;
    case PCI_CAP_TYPE_VPD:
      lrt_printf(" - VPD\n");
      break;
    case PCI_CAP_TYPE_SLI:
      lrt_printf(" - slot identifiation\n");
      break;
    case PCI_CAP_TYPE_MSI:
      lrt_printf(" - MSI capable\n");
#if 0 // don't think we need to enable MSI on bridge
      if (isabus) {
	uint16_t tmp;
	lrt_printf("bad bad dan, enabling msi in reading this\n");
	tmp = pci_config_read16(bus, slot, 0, ptr+2);
	lrt_printf("msg control register %x\n", tmp);

	tmp |= (1<<0);		/* enable interrupts */
	pci_config_write16(bus, slot, 0, ptr+2, tmp);
      }
#endif      
      break;
    case PCI_CAP_TYPE_HSW:
      lrt_printf(" - hot swap\n");
      break;
    case PCI_CAP_TYPE_MSIX:
      lrt_printf(" - MSI-X capable\n");
      parse_msix_capability(bus, slot, ptr);
      break;
    default:
      lrt_printf(" - ?????\n");
    }
    ptr = nxt;
  }
}

#define VPRINT if(verbose) lrt_printf

static EBBRC
pci_get_info_bus(int bus, int targ_vend, int targ_devid, struct pci_info *info, 
		 int verbose) 
{
  uint16_t vendor;
  uint16_t device;
  uint16_t status;
  uint8_t header;
  uint8_t dev_class;
  uint8_t subclass;
  uint8_t  progif;
  int slot;
  EBBRC rc;

  VPRINT("Enumerating bus %d\n", bus);
 
  for (slot = 0; slot < 32; slot++) {
    vendor = pci_config_read16(bus,slot,0,0);
    device = pci_config_read16(bus,slot,0,0x2);
    if (vendor == 0xFFFF)
      continue;
    if ((vendor == targ_vend) && (device == targ_devid)) {
      // found it
      info->bus = bus;
      info->slot = slot;
      info->func = 0;
      return EBBRC_OK;
    }

    status = pci_config_read16(bus,slot,0,0x6);
    header = pci_config_read8(bus,slot,0,0xe);
    dev_class = pci_config_read8(bus,slot,0,11);
    subclass = pci_config_read8(bus,slot,0,10);
    progif = pci_config_read8(bus,slot,0,9);
    VPRINT("\tBus %d Slot %d header %x: ", bus, slot, header);
    if (header & 1<<7) {
      // its a multi-function device
      VPRINT("(*MFD*) ");
    }
    switch( (header & 0x3F) ) {
    case 0: // standard device layout
      if (verbose) {
	print_vendor_dev(vendor, device);
	VPRINT("\t\tClass code: %x, Subclass: %x, Prog IF: %x\n",
	       dev_class, subclass, progif);
	parse_status(status);
	if (status & (1<<4)) {
	  print_capability_list(bus, slot, 0);
	}
      }
      break;
    case 0x1: 
      {
	uint8_t sbus = pci_config_read8(bus,slot,0,25);
	if (verbose) {
	  VPRINT(" - PCI bridge, subbus is %d\n", sbus);
	  parse_status(status);
	  if (status & (1<<4)) {
	    print_capability_list(bus, slot, 1);
	  }
	}

	rc = pci_get_info_bus(sbus, targ_vend,  targ_devid, info, verbose);
	if (rc == EBBRC_OK) return rc;
	VPRINT("still searchhing, back to bus %d\n", bus);
      }
      break;
    case 0x2: 
      VPRINT(" -  Card Bus bridge\n");
      break;
    default:
      VPRINT("- Unknown header\n");
    }
  }
  return EBBRC_NOTFOUND;
}

void
pci_print_all()
{
  pci_get_info_bus(0, PCI_VENDOR_BOGUS, 0, NULL, 1);
}

EBBRC 
pci_get_info(int vendor, int devid, struct pci_info *info)
{
  return pci_get_info_bus(0, vendor, devid, info, 0);
}
