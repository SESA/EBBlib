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


/* ******************************************************************
This is the first simple ethernet device driver for the E1000E NIC
from Intel.  
  See:
    http://www.intel.com/content/www/us/en/ethernet-controllers/82574l-gbe-controller-datasheet.html

  Goal of this first ethernet driver:
  ----------------------------------
  1. get basic networking going
  2. solidify the interface to the driver
  3. experiment with event model, wiring up ethernet

We are not at this point fully exploiting the device, e.g., offload,
scatter/gather, demux of incomming packets... that will be later.

For packet reception, a registered hander is invoked with the buffer
(list). The buffers are freed automatically after the event has
completed.

Rational:
o Naturally couples the data with the event.
o For now, we assume on read that most of the time buffer alignment,
  size or something else not optimal, so anything retained for a long
  time will probably be copied anyhow.
o If its retained for a long time, the cost of copying may not be
  important. This, of course, may be re-evaluated as get perforance
  data. In that context, we  can eventually return an argument to say
  don't free the buffer.

For writes, we will support initially a simple model, where the driver
copies the data into an internal buffer, and returns.  In the long
term, we should expore a model like fbufs, where the buffer is
immutable, with multiple reference counts, on last reference count
freed.  This will allow the same buffer to be repeatedly written to
the network with scatter gather to avoid copies.

****************************************************************** */

#include <config.h>
#include <inttypes.h>
#include <l0/EBBMgrPrim.h>
#include <lrt/io.h>
#include <io/bare/pci.h>
#include <io/EthMgr.h>
#include <io/bare/pci.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/MemMgrPrim.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <io/bare/e1000e.h>

#define NUM_RC_BUFS 8
#define NUM_TX_BUFS 8

CObject(EthMgrE1000E) {
  COBJ_EBBFuncTbl(EthMgr);
  
  struct pci_info pi;
  uint32_t bar;			/* base address register, i.e., base address
				   for memory mapped device registers */
  // ring of control information for transmission
  unsigned tx_tail;
  void *tx_ring_base_add, *rc_ring_base_add;
  struct le_e1ke_tx_desc *tx_ring_p;

  // ring of control information for reception
  unsigned rc_tail;
  unsigned rc_next;		/* next buf to read */
  struct le_e1ke_rc_desc *rc_ring_p;
};

static inline void
dump_regs(EthMgrE1000ERef self)
{
  uint32_t tmp = rd_reg(self->bar, E1KE_CTRL);
  print_ctrl_reg(tmp);
  tmp = rd_reg(self->bar, E1KE_TCTL);
  print_tctl_reg(tmp);
  tmp = rd_reg(self->bar, E1KE_STATUS);
  lrt_printf("status register is %x\n", tmp);
  dump_phys(self->bar);

  tmp = rd_reg(self->bar, 0x10);
  lrt_printf("eprom/flash control is %x\n", tmp);

  tmp = rd_reg(self->bar, E1KE_CTRL_EXT);
  lrt_printf("CTRL_EXT %x\n", tmp);
}

static inline unsigned 
get_transmit_tail(EthMgrE1000ERef self)
{
#if 0
  // this is expensive, but good debugging check
  LRT_Assert(self->tx_tail == rd_reg(self->bar, E1KE_TDT(0)));
#endif
  return self->tx_tail;
}

static inline void 
advance_transmit_tail(EthMgrE1000ERef self)
{
  self->tx_tail = (self->tx_tail+1)%NUM_TX_BUFS;
  wt_reg(self->bar, E1KE_TDT(0), self->tx_tail);
}

static inline unsigned 
get_receive_tail(EthMgrE1000ERef self)
{
#if 0
  // this is expensive
  LRT_Assert(self->rc_tail == rd_reg(self->bar, E1KE_RDT(0)));
#endif
  return self->rc_tail;
}

static inline void 
advance_receive_tail(EthMgrE1000ERef self)
{
  self->rc_tail = (self->rc_tail+1)%NUM_RC_BUFS;
  wt_reg(self->bar, E1KE_RDT(0), self->rc_tail );
}

static void
EthMgrE1000E_setup_transmit(EthMgrE1000ERef self)
{
  uint32_t val;
  int i;
  
  for(i=0;i<NUM_TX_BUFS;i++) {
    self->tx_ring_p[i].val = 0;	/* FIXME: set dd to 1 so can check done read */
    self->tx_ring_p[i].buf_add = 0;
  }
  
  print_txdctl_reg(rd_reg(self->bar, E1KE_TXDCTL(0)));
  val = 1 << E1KE_TXDCTL_WTHRESH; /* set write back threashold to 1 */
  val |= (1 << E1KE_TXDCTL_GRAN); /* granularity in descriptors */
  wt_reg(self->bar, E1KE_TXDCTL(0), val);
		   
  lrt_printf("after initialize txdctl register\n");
  print_txdctl_reg(rd_reg(self->bar, E1KE_TXDCTL(0)));

  val = (0xF<<E1KE_TCTL_CT) | (0x3f <<E1KE_TCTL_COLD) | (1<<E1KE_TCTL_PSP) 
    | (1 <<E1KE_TCTL_EN);
  wt_reg(self->bar, E1KE_TCTL, val);

  wt_reg(self->bar, E1KE_TDBAL(0), ((uintptr_t)self->tx_ring_p) & 0xFFFFFFFF);
  wt_reg(self->bar, E1KE_TDBAH(0), ((uintptr_t)self->tx_ring_p) >> 32);
  wt_reg(self->bar, E1KE_TDLEN(0), sizeof(struct le_e1ke_tx_desc)*NUM_TX_BUFS);
}

static void inline
setup_next_receive_buffer(EthMgrE1000ERef self) 
{
  EBBRC rc;
  volatile struct le_e1ke_rc_desc *rc_desc;
  void *buf;

  rc_desc = &self->rc_ring_p[self->rc_tail];

  lrt_printf("alloc [");
  rc = EBBPrimMalloc(E1KE_BUFLEN, &buf, EBB_MEM_DEFAULT);
  lrt_printf("alloc ]\n");
  LRT_RCAssert(rc);

  rc_desc->val = 0;
  rc_desc->buf_add = buf;

  advance_receive_tail(self);
  lrt_printf("after advance tail is %d, head is %d\n", 
	     rd_reg(self->bar, E1KE_RDT(0)), 
	     rd_reg(self->bar, E1KE_RDH(0)));
}


static inline void
EthMgrE1000E_setup_receive(EthMgrE1000ERef self)
{
  uint64_t mac;

  if (rd_reg(self->bar, E1KE_RAH(0)) & (1<<31)) {
    lrt_printf("mac address is valid\n");
    mac = (((uint64_t)rd_reg(self->bar, E1KE_RAH(0)))<<32) | 
      rd_reg(self->bar, E1KE_RAL(0));
    mac = mac & ~(0xffffLL<<48);	/* mask off upper bits of crap */
    unsigned char *c = (unsigned char *)&mac;
    lrt_printf("address is: %02x:%02x:%02x:%02x:%02x:%02x\n", 
	       c[0], c[1], c[2], c[3], c[4], c[5]);
  } else {
    LRT_Assert(0);		/* invalid mac address */
  }

  self->rc_tail = 0;
  // allocate one buffer less, so tail doesn't wrap around
  for (int i=0; i<NUM_RC_BUFS-1; i++) {
    lrt_printf("buf %d [", i);
    setup_next_receive_buffer(self);
    lrt_printf("]");
  }

  // setup the receive buffer ring
  lrt_printf("address of rc ring is %lx\n", (long unsigned)&self->rc_ring_p[0]);
  wt_reg(self->bar, E1KE_RDBAL(0), ((uintptr_t)&self->rc_ring_p[0]) 
	 & 0xFFFFFFFF);
  wt_reg(self->bar, E1KE_RDBAH(0), ((uintptr_t)&self->rc_ring_p[0]) >> 32);
  wt_reg(self->bar, E1KE_RDLEN(0), sizeof(struct le_e1ke_rc_desc)*NUM_RC_BUFS);

  uint32_t val;
  // setup the read control register
  val = (1 << E1KE_RCTL_EN);	/* enable */
  val |= (1 << E1KE_RCTL_SBP); /* FIXME: store bad packets for now */
  val |= (1 << E1KE_RCTL_UPE); /* FIXME: lets be promiscuous */
  val |= (1 << E1KE_RCTL_LBM); /* FIXME: doing loopback */
  val |= (1 << E1KE_RCTL_BAM); /* FIXME: accept broadcasts */

  LRT_Assert(E1KE_BUFLEN == 2048); /* else, set BSIZE */
  wt_reg(self->bar, E1KE_RCTL, val);
}

static void
EthMgrE1000E_dumpread(EthMgrE1000ERef self) 
{
  uint32_t hd = 0, lhd =0;
  // uint32_t tl = rd_reg(self->bar, E1KE_RDT(0));
  lrt_printf("dumread, tail is %d, head is %d\n", 
	     rd_reg(self->bar, E1KE_RDT(0)), 
	     rd_reg(self->bar, E1KE_RDH(0)));
  while (1) {
    while (hd == rd_reg(self->bar, E1KE_RDH(0))) {}
    hd = rd_reg(self->bar, E1KE_RDH(0));
    for (int i=lhd ; i<hd; i++) {
      lrt_printf("head changed tail is %d, head is %d\n", 
		 rd_reg(self->bar, E1KE_RDT(0)), 
		 rd_reg(self->bar, E1KE_RDH(0)));
      lrt_printf("val is %lx, status is %lx\n", 
		 self->rc_ring_p[i].val, 
		 (long)self->rc_ring_p[i].status);
      lrt_printf("packet contents %s\n", 
		 (char *)self->rc_ring_p[i].buf_add);
      lhd = hd;
    }
  }
}


static EBBRC
EthMgrE1000E_write(EthMgrE1000ERef self, void *buf, unsigned len) 
{
  unsigned i;
  volatile struct le_e1ke_tx_desc *tx_desc;

  if (len > E1KE_MAX_PACKLEN) {
    lrt_printf("Packet too long\n");
    return -1;
  }
  
  i = get_transmit_tail(self);
  lrt_printf("writting to tail %d\n", i);
  
  tx_desc = &self->tx_ring_p[i];
#if 0				/* fixme, init to 1 later */
  if (!tx_desc->dd) {
    // device has not yet finished with descriptor
    lrt_printf("not finished, retry\n");
    return EBBRC_RETRY;
  }
#endif

  tx_desc->buf_add = buf;

  tx_desc->val = 0;
  tx_desc->len = 1500;
  // lrt_printf("set length to %d\n", len);
  tx_desc->cso = 0;
  tx_desc->ifcs = 1;
  tx_desc->rs = 1;
  tx_desc->eop = 1;

  // note put memory fence here, before tail, then
  // get rid of volatile on above structure

  lrt_printf("tx desc %" PRIxPTR" %" PRIxPTR"\n", 
	     (uintptr_t)tx_desc->buf_add, (uintptr_t)tx_desc->val);
  advance_transmit_tail(self);


  /* //block until we get confirmation that it was sent out */
  lrt_printf("blocking until write gets out\n");
  while (!tx_desc->dd) {lrt_printf("*");}; 
  lrt_printf("we think the write got out\n");
  return len;
}


static struct le_e1ke_tx_desc tx_ring[NUM_TX_BUFS] __attribute__ ((aligned(16)));
static struct le_e1ke_rc_desc rc_ring[NUM_RC_BUFS] __attribute__ ((aligned(16)));

static EBBRC
EthMgrE1000E_init(void *_self)
{
  EthMgrE1000ERef self = (EthMgrE1000ERef)_self;
  EBBRC rc;
  uint32_t tmp;

#if 0
  // needs to be 16 byte aligned
  rc = EBBPrimMalloc(((sizeof(struct le_e1ke_tx_desc)*NUM_TX_BUFS) + 16), 
		     &self->tx_ring_base_add, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);
  rc = EBBPrimMalloc(sizeof(struct le_e1ke_rc_desc)*NUM_RC_BUFS + 16, 
		     &self->rc_ring_base_add, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  self->tx_ring_p = (void *)((uint64_t)self->tx_ring_base_add & ~0xF);
  self->rc_ring_p = (void *)((uint64_t)self->rc_ring_base_add & ~0xF);
#else
  self->tx_ring_p = &tx_ring[0];
  self->rc_ring_p = &rc_ring[0];
#endif
  lrt_printf(" wt ring buf %lx, ptr %lx, len %ld\n", 
	     (uint64_t)self->tx_ring_base_add, (uint64_t)self->tx_ring_p,
	     ((sizeof(struct le_e1ke_tx_desc)*NUM_TX_BUFS) + 16));
  lrt_printf(" rd ring buf %lx, ptr %lx\n", 
	     (uint64_t)self->rc_ring_base_add, (uint64_t)self->rc_ring_p);

  // set pci information in device
  rc = pci_get_info(PCI_VENDOR_INTEL, PCI_INTEL_DEVID_E1000E, &self->pi);
  LRT_RCAssert(rc);		/* later check EBBRC_NOTFOUND */

  lrt_printf("found dev, bus %d, slot %d initializing e1000e\n",
	     self->pi.bus, self->pi.slot);

  self->bar = pci_config_read32(&self->pi, 0x10);

  pci_enable_bus_master(&self->pi);

  dump_regs(self);		/* print out all key registers */

  e1000e_disable_all_interrupts(self->bar);
  e1000e_reset_device(self->bar);
  e1000e_disable_all_interrupts(self->bar);
  e1000e_clear_all_interrupts(self->bar);

  tmp = rd_reg(self->bar, E1KE_CTRL);
  tmp |= 1<<E1KE_CTRL_SLU_BIT;
  wt_reg(self->bar, E1KE_CTRL, tmp);

  tmp = rd_reg(self->bar, E1KE_CTRL_EXT);
  tmp |= 1<<E1KE_CTRL_EXT_DRV_LOAD;
  wt_reg(self->bar, E1KE_CTRL_EXT, tmp);

  EthMgrE1000E_setup_receive(self);
  EthMgrE1000E_setup_transmit(self);

  lrt_printf("***after initialization:\n");
  dump_regs(self);		/* print out all key registers */

  lrt_printf("---- done initializing e1000e\n");
  EthMgrE1000E_write(self, "dan is a bozo", 14); 

  EthMgrE1000E_dumpread(self);  

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
  // fixme, if fails should free storage and return
  LRT_RCAssert(rc);		/* later return EBBRC_NOTFOUND */

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
