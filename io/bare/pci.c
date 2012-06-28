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
  PCI_VENDOR_LSI = 0x1000,
  PCI_VENDOR_NVIDIA = 0x10de,
  PCI_VENDOR_AMD1 = 0x1002,
  PCI_VENDOR_AMD2 = 0x1022
};

enum {
  PCI_AMD_DEVID_SATACTLR = 0x4390
};

enum {
  PCI_INTEL_DEVID_HBRIDGE = 0x7190,
  PCI_INTEL_DEVID_PCIBRIDGE = 0x7191,
  PCI_INTEL_DEVID_ISABRIDGE = 0x7110, 
  PCI_INTEL_DEVID_ETHERNT = 0x100f,
  PCI_INTEL_DEVID_GBADPT = 0x10c9,
  PCI_INTEL_DEVID_ETHERNT2 = 0x10d3
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
    case PCI_INTEL_DEVID_ETHERNT2:
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
  taboffset = pci_config_read32 (bus, slot, 0, ptr+4);
  pbaoffset = pci_config_read32 (bus, slot, 0, ptr+8);
  // if bottom bit of bar is 0, then its memory mapped the tables
  // if bit 1 is 0, then its a 32 bit address
  bar = pci_config_read32( bus, slot, 0, 0x10);
  lrt_printf("\t\t\t msix taboffset %x, pbaoffset %x bar %x\n", 
	     taboffset, pbaoffset, bar);

}

/*
 * these are the registeres in the memory mapped space of the E1000 (or
 * really e1000e) for various control registers
 */
#define E1KE_CTRL 0x0		/* device control register (RW) */
#define E1KE_STATUS 0x08	/* status register */
#define E1KE_CTRL_EXT 0x018   /* extended device control register (RW) */
#define E1KE_IMC 0xd8		/* interrupt mask clear (WO) */
#define E1KE_ICR 0xc0		/* interrupt cause read (RW) */
#define E1KE_GCR 0x5b00       /* 3GIO control register (RW) */
#define E1KE_TXDCTL 0x03828	/* transmit descriptor control (RW) */
#define E1KE_TCTL 0x0400	/* transmit control (RW) */

#define E1KE_TDBAL(N) (0x3800+N*0x100)
#define E1KE_TDBAH(N) (0x3804+N*0x100)
#define E1KE_TDLEN(N) (0x3808+N*0x100)
#define E1KE_TDH(N) (0x3810+N*0x100)
#define E1KE_TDT(N) (0x3818+N*0x100)

// BIT definitons in CTRL register 
#define E1KE_CTRL_RST_BIT 26  /* reset */
#define E1KE_CTRL_SLU_BIT 6   /* set link up */

// BIT definitons in CTRL_EXT register 
#define E1KE_CTRL_EXT_RST 16 /* reset for extended control */

// BIT definitions in transmit descriptor control
#define E1KE_TXDCTL_PTHRESH 0 	/* bits 0-5 */
#define E1KE_TXDCTL_GRAN 24		/* bit 24  */

// bit defintions for transmit control register
#define E1KE_TCTL_EN 1	/* enable */
#define E1KE_TCTL_COLD 12	/* bits 12-21, collision distance */
#define E1KE_TCTL_CT 4	/* bits 4-11 collision threashold */
#define E1KE_TCTL_PSP 3  	/* pad short packets */

// note, this is probably all broken on big endian machines
struct e1ke_tx_desc {
  uint64_t *buf_add;
  union {
    uint64_t val;
    struct {
      uint16_t len;
      uint8_t cso;
      union {
	uint8_t cmd;
	struct {
	  uint8_t eop:1;
	  uint8_t ifcs:1;
	  uint8_t ic:1;
	  uint8_t rs:1;
	  uint8_t rsvc:1;
	  uint8_t dext:1;
	  uint8_t vle:1;
	  uint8_t ide:1;
	};
      };
      union {
	uint8_t status:4;
	struct {
	  uint8_t dd:1;
	  uint8_t rsvs:3;
	};
      };
      uint8_t extcmd:4;
      uint8_t css;
      uint16_t vlan;
    };
  };
}__attribute__((packed));
      

// ring of control informaiton for transmission
struct e1ke_tx_desc tx_ring[8] __attribute__ ((aligned(16)));

static uint32_t 
rd_reg(uint32_t bar, uint32_t offset)
{
  volatile uint32_t *ptr = (uint32_t *)(uintptr_t)(bar + offset);
  return *ptr;
}

static void
wt_reg(uint32_t bar, uint32_t offset, uint32_t val)
{
  volatile uint32_t *ptr = (uint32_t *)(uintptr_t)(bar + offset);
  *ptr = val;
}

static inline void
e1000e_disable_all_interrupts(uint32_t bar) 
{
  wt_reg(bar, E1KE_IMC, 0xffffffff);
}

static inline void
e1000e_reset_device(uint32_t bar) 
{
  uint32_t tmp = rd_reg(bar, E1KE_CTRL);
  tmp |= 1<<E1KE_CTRL_RST_BIT;
  wt_reg(bar, E1KE_CTRL, tmp);
  
  tmp = rd_reg(bar, E1KE_GCR);
  tmp |= 1<<22;
  wt_reg(bar, E1KE_GCR, tmp);
}

static inline void
e1000e_clear_all_interrupts(uint32_t bar)
{
  rd_reg(bar, E1KE_ICR);
}

int
e1ke_write(uint32_t bar, void *buf, unsigned len) 
{
  unsigned i;
  volatile struct e1ke_tx_desc *tx_desc;

  if (len > 1514) {
    lrt_printf("Packet too long\n");
    return -1;
  }
  
  i = rd_reg(bar, E1KE_TDT(0));
  // read head BOZO to make sure not overriding crap

  tx_desc = &tx_ring[i];
  tx_desc->buf_add = buf;

  tx_desc->val = 0;
  tx_desc->len = len;
  tx_desc->cso = 0;
  tx_desc->ifcs = 1;
  tx_desc->rs = 1;
  tx_desc->eop = 1;

  /*FIXME change 8 to macro for number in ring */
  wt_reg(bar, E1KE_TDT(0), ((i+1)%8 ) );

  lrt_printf("we changed the tail\n");
  //block until we get confirmation that it was sent out
  while (!tx_ring[i].dd);
  lrt_printf("we think the write got out\n");
  return len;
}

static void
e1000e_setup_transmit(uint32_t bar)
{
  uint32_t val;
  val = (1 << E1KE_TXDCTL_PTHRESH) | (1 << E1KE_TXDCTL_GRAN);
  wt_reg(bar, E1KE_TXDCTL, val);

  val = (0xF<<E1KE_TCTL_CT) | (0x3f <<E1KE_TCTL_COLD) | 
    (1<<E1KE_TCTL_PSP) | (1 <<E1KE_TCTL_EN);
  wt_reg(bar, E1KE_TCTL, val);

  wt_reg(bar, E1KE_TDBAL(0), ((uintptr_t)&tx_ring[0]) & 0xFFFFFFFF);
  wt_reg(bar, E1KE_TDBAH(0), ((uintptr_t)&tx_ring[0]) >> 32);
  wt_reg(bar, E1KE_TDLEN(0), sizeof(tx_ring));

  
}

static void
enable_bus_master(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint16_t command;

  command = pci_config_read16(bus,slot,func,0x4);
  command |= 0x4;
  pci_config_write16(bus,slot,func,0x4,command);
}

static void
initialize_e1000e(uint8_t bus, uint8_t slot)
{
  lrt_printf("---- initializing e1000e\n");
  uint32_t bar;
  bar = pci_config_read32( bus, slot, 0, 0x10);
  enable_bus_master(bus, slot, 0);

  uint32_t tmp = rd_reg(bar, E1KE_CTRL);
  lrt_printf("control register is %x\n", tmp);
  tmp = rd_reg(bar, E1KE_STATUS);
  lrt_printf("status register is %x\n", tmp);

  e1000e_disable_all_interrupts(bar);
  e1000e_reset_device(bar);
  e1000e_disable_all_interrupts(bar);
  e1000e_clear_all_interrupts(bar);

  tmp = rd_reg(bar, E1KE_CTRL);
  tmp |= 1<<E1KE_CTRL_SLU_BIT;
  wt_reg(bar, E1KE_CTRL, tmp);

  e1000e_setup_transmit(bar);

  tmp = rd_reg(bar, E1KE_CTRL);
  lrt_printf("control register is %x\n", tmp);
  tmp = rd_reg(bar, E1KE_STATUS);
  lrt_printf("status register is %x\n", tmp);

  lrt_printf("---- done initializing e1000e\n");
  e1ke_write(bar, "dan is a bozo", 14); 
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
      parse_msix_capability(bus, slot, ptr);
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
      if ((vendor == PCI_VENDOR_INTEL) && (device == PCI_INTEL_DEVID_ETHERNT2)) {
	initialize_e1000e(bus, slot);
      };
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
