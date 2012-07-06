#ifndef __PCI_H__
#define __PCI_H__
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

// vendors we know about
enum {
  PCI_VENDOR_BOGUS = 1,
  PCI_VENDOR_INTEL = 0x8086,
  PCI_VENDOR_VMWARE = 0x15ad,
  PCI_VENDOR_LSI = 0x1000,
  PCI_VENDOR_NVIDIA = 0x10de,
  PCI_VENDOR_AMD1 = 0x1002,
  PCI_VENDOR_AMD2 = 0x1022
};

// devices from particular vendors
enum {
  PCI_AMD_DEVID_SATACTLR = 0x4390
};

enum {
  PCI_INTEL_DEVID_HBRIDGE = 0x7190,
  PCI_INTEL_DEVID_PCIBRIDGE = 0x7191,
  PCI_INTEL_DEVID_ISABRIDGE = 0x7110, 
  PCI_INTEL_DEVID_ETHERNT = 0x100f,
  PCI_INTEL_DEVID_GBADPT = 0x10c9,
  PCI_INTEL_DEVID_E1000E = 0x10d3
};

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
  
// information that will describe device to pci buss,
// should be opaque
struct pci_info {
  uint8_t bus;
  uint8_t slot;
  uint16_t func;
};

extern uint32_t pci_config_read32(struct pci_info *pi, uint16_t offset);
extern void pci_enable_bus_master(struct pci_info *pi);
extern void pci_print_all();	/* prints on console everything we can find */
extern EBBRC pci_get_info(int vendor, int devid, struct pci_info *info);

// move this out to driver
extern void e1000e_init(uint8_t bus, uint8_t slot);
#endif
