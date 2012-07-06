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
#include <l0/EBBMgrPrim.h>
#include <lrt/io.h>
#include <io/bare/pci.h>
#include <io/EthMgr.h>
#include <io/bare/pci.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/MemMgrPrim.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/cobj/CObjEBBUtils.h>

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

CObject(EthMgrE1000E) {
  COBJ_EBBFuncTbl(EthMgr);
  
  struct pci_info pi;
  uint32_t bar;			/* base address register, i.e., base address
				   for memory mapped device registers */
  // ring of control information for transmission
  struct e1ke_tx_desc tx_ring[8] __attribute__ ((aligned(16)));
};

static void
EthMgrE1000E_setup_transmit(EthMgrE1000ERef self)
{
  uint32_t val;
  val = (1 << E1KE_TXDCTL_PTHRESH) | (1 << E1KE_TXDCTL_GRAN);
  wt_reg(self->bar, E1KE_TXDCTL, val);

  val = (0xF<<E1KE_TCTL_CT) | (0x3f <<E1KE_TCTL_COLD) | 
    (1<<E1KE_TCTL_PSP) | (1 <<E1KE_TCTL_EN);
  wt_reg(self->bar, E1KE_TCTL, val);

  wt_reg(self->bar, E1KE_TDBAL(0), ((uintptr_t)&self->tx_ring[0]) & 0xFFFFFFFF);
  wt_reg(self->bar, E1KE_TDBAH(0), ((uintptr_t)&self->tx_ring[0]) >> 32);
  wt_reg(self->bar, E1KE_TDLEN(0), sizeof(self->tx_ring));
}

static EBBRC
EthMgrE1000E_write(EthMgrE1000ERef self, void *buf, unsigned len) 
{
  unsigned i;
  volatile struct e1ke_tx_desc *tx_desc;

  if (len > 1514) {
    lrt_printf("Packet too long\n");
    return -1;
  }
  
  i = rd_reg(self->bar, E1KE_TDT(0));
  // read head BOZO to make sure not overriding crap

  tx_desc = &self->tx_ring[i];
  tx_desc->buf_add = buf;

  tx_desc->val = 0;
  tx_desc->len = len;
  tx_desc->cso = 0;
  tx_desc->ifcs = 1;
  tx_desc->rs = 1;
  tx_desc->eop = 1;

  /*FIXME change 8 to macro for number in ring */
  wt_reg(self->bar, E1KE_TDT(0), ((i+1)%8 ) );

  lrt_printf("we changed the tail\n");
  //block until we get confirmation that it was sent out
  while (!self->tx_ring[i].dd);
  lrt_printf("we think the write got out\n");
  return len;
}

static EBBRC
EthMgrE1000E_init(void *_self)
{
  EthMgrE1000ERef self = (EthMgrE1000ERef)_self;
  EBBRC rc;

  // set pci information in device
  rc = pci_get_info(PCI_VENDOR_INTEL, PCI_INTEL_DEVID_E1000E, &self->pi);
  LRT_RCAssert(rc);		/* later check EBBRC_NOTFOUND */

  lrt_printf("found dev, bus %d, slot %d initializing e1000e\n",
	     self->pi.bus, self->pi.slot);

  self->bar = pci_config_read32(&self->pi, 0x10);
  pci_enable_bus_master(&self->pi);

  uint32_t tmp = rd_reg(self->bar, E1KE_CTRL);
  lrt_printf("control register is %x\n", tmp);
  tmp = rd_reg(self->bar, E1KE_STATUS);
  lrt_printf("status register is %x\n", tmp);

  e1000e_disable_all_interrupts(self->bar);
  e1000e_reset_device(self->bar);
  e1000e_disable_all_interrupts(self->bar);
  e1000e_clear_all_interrupts(self->bar);

  tmp = rd_reg(self->bar, E1KE_CTRL);
  tmp |= 1<<E1KE_CTRL_SLU_BIT;
  wt_reg(self->bar, E1KE_CTRL, tmp);

  EthMgrE1000E_setup_transmit(self);

  tmp = rd_reg(self->bar, E1KE_CTRL);
  lrt_printf("control register is %x\n", tmp);
  tmp = rd_reg(self->bar, E1KE_STATUS);
  lrt_printf("status register is %x\n", tmp);

  lrt_printf("---- done initializing e1000e\n");
  EthMgrE1000E_write(self, "dan is a bozo", 14); 
  return EBBRC_OK;
}

static EBBRC
EthMgrE1000E_bind(void *_self, uint16_t type, EthTypeMgrId id)
{
  return EBBRC_GENERIC_FAILURE;
}
  
static EBBRC 
EthMgrE1000E_inEvent(EthMgrE1000ERef self)
{
  // wire up for input event for MSI-X
  return EBBRC_GENERIC_FAILURE;
}
  
CObjInterface(EthMgr) EthMgrE1000E_ftable = {
  // base functions of ethernet manager
  // OK: not entirely clear to me idea that JA had here, 
  // ignoring, and just implementing device driver for single
  // ethernet device
  .init = EthMgrE1000E_init,
  .bind = EthMgrE1000E_bind,
  
  .inEvent = (GenericEventFunc)EthMgrE1000E_inEvent
};

EBBRC
EthMgrE1000ECreate(EthMgrId *id)
{
  EBBRC rc;
  EthMgrE1000ERef repRef;
  CObjEBBRootSharedRef rootRef;
  
  rc = EBBPrimMalloc(sizeof(EthMgrE1000E), &repRef, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  repRef->ft = &EthMgrE1000E_ftable; 

  lrt_printf("found device, intializing\n");

  rc = EthMgrE1000E_init(repRef);
  // fixme, if failes should free storage and return
  LRT_RCAssert(rc);		/* later check EBBRC_NOTFOUND */

  rc = CObjEBBRootSharedCreate(&rootRef, (EBBRepRef)repRef);
  LRT_RCAssert(rc);

  rc = EBBAllocPrimId((EBBId *)id);
  LRT_RCAssert(rc);

  rc = CObjEBBBind((EBBId)*id, rootRef); 
  LRT_RCAssert(rc);


  // do a write operation here

  return EBBRC_OK;
}


#ifndef LRT_ULNX
/*
 * For now, doing something brain damged, telling 
 * pci bus to give us info about the one nic we know
 * about.  We should probably move this outside of this
 * file and ask for a list of all NICs...
 */
EBBRC
EthMgrCreate(EthMgrId *id) 
{
  pci_print_all();

  lrt_printf("calling EthMgrE1000ECreate\n");
  return EthMgrE1000ECreate(id);
}
#endif
