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
  2. explore the interface to the driver
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
#include <l0/lrt/event.h>
#include <arpa/inet.h>

#define swapShort(x) \
    ((__uint16_t)((((__uint16_t)(x) & 0xff00) >> 8) | \
		  (((__uint16_t)(x) & 0x00ff) << 8)))
#define ntohs(x) swapShort(x)

#define NUM_RC_BUFS 8
#define NUM_TX_BUFS 8

// prototype for handler for ethernet packets
typedef EBBRC (* ethPacketHandler)(void * _self, void *buf);

CObject(EthMgrE1000E) {
  COBJ_EBBFuncTbl(EthMgrE1000E);
  
  EthMgrId myID;

  struct pci_info pi;
  uint32_t bar[6];		/* base address registers, i.e., base address
				   for memory mapped device registers */
  EventNo pollEvent;

  // ring of control information for transmission
  unsigned tx_tail;
  void *tx_ring_base_add, *rc_ring_base_add;
  struct le_e1ke_tx_desc *tx_ring_p;

  EventNo msiev;
  EventNo msixev[5];

  EBBId handlerId;
  EBBFuncNum handlerFuncNum;

  // ring of control information for reception
  unsigned rc_tail;
  unsigned rc_last_head;		/* next buf to read */
  struct le_e1ke_rc_desc *rc_ring_p;
};

#define ETH_ALEN 6

EBBRC 
EthMgrE1000E_defaultEthernetPacketHandler(void *self, void *buf)
{
  uint8_t *tmp;
  uint16_t protocol;

  tmp = buf;
  lrt_printf("defEthhandler packet:(");
  lrt_printf("d %02x:%02x:%02x:%02x:%02x:%02x, ", 
	     tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5]);
  tmp += ETH_ALEN;
  lrt_printf("s %02x:%02x:%02x:%02x:%02x:%02x, ", 
	     tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5]);

  tmp += ETH_ALEN;
  protocol = ntohs(*(uint16_t *)tmp);
  lrt_printf("t %x)\n", (short)protocol);

  return EBBRC_OK;
}

EBBRC 
EthMgrE1000E_bindPacketHandler(EthMgrE1000ERef self, EBBId handler, 
			       EBBFuncNum fn)
{
  self->handlerId = handler;
  self->handlerFuncNum = fn;
  return EBBRC_OK;
}


CObjInterface(EthMgrE1000E) {
  CObjImplements(EthMgr);

  // this is a cludge function to poll the network for now
  EBBRC (*readPoll)(EthMgrE1000ERef self);

  // interrupt routines for MSI-X interrupts
  EBBRC (*int0)(EthMgrE1000ERef self);
  EBBRC (*int1)(EthMgrE1000ERef self);
  EBBRC (*int2)(EthMgrE1000ERef self);
  EBBRC (*int3)(EthMgrE1000ERef self);
  EBBRC (*int4)(EthMgrE1000ERef self);
  // interrupt routines for MSI mode
  EBBRC (*intMSI)(EthMgrE1000ERef self);

  // default packet handler, supported by this device
  EBBRC (*defPacketHandler)(void *_self, void *buf);

  // bind a new packet handler
  EBBRC (*bindPacketHandler)(EthMgrE1000ERef self, EBBId handler, EBBFuncNum fn);
};


static inline void
dump_regs(EthMgrE1000ERef self)
{
  uint32_t tmp = rd_reg(self->bar[0], E1KE_CTRL);
  print_ctrl_reg(tmp);
  tmp = rd_reg(self->bar[0], E1KE_TCTL);
  print_tctl_reg(tmp);
  tmp = rd_reg(self->bar[0], E1KE_STATUS);
  lrt_printf("status register is %x\n", tmp);
  dump_phys(self->bar[0]);

  tmp = rd_reg(self->bar[0], 0x10);
  lrt_printf("eprom/flash control is %x\n", tmp);

  tmp = rd_reg(self->bar[0], E1KE_CTRL_EXT);
  lrt_printf("CTRL_EXT %x\n", tmp);
}

static inline unsigned 
get_transmit_tail(EthMgrE1000ERef self)
{
#if 0
  // this is expensive, but good debugging check
  LRT_Assert(self->tx_tail == rd_reg(self->bar[0], E1KE_TDT(0)));
#endif
  return self->tx_tail;
}

static inline void 
advance_transmit_tail(EthMgrE1000ERef self)
{
  self->tx_tail = (self->tx_tail+1)%NUM_TX_BUFS;
  wt_reg(self->bar[0], E1KE_TDT(0), self->tx_tail);
}

static inline unsigned 
get_receive_tail(EthMgrE1000ERef self)
{
#if 0
  // this is expensive
  LRT_Assert(self->rc_tail == rd_reg(self->bar[0], E1KE_RDT(0)));
#endif
  return self->rc_tail;
}

static inline void 
advance_receive_tail(EthMgrE1000ERef self)
{
  self->rc_tail = (self->rc_tail+1)%NUM_RC_BUFS;
  wt_reg(self->bar[0], E1KE_RDT(0), self->rc_tail );
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
  
  print_txdctl_reg(rd_reg(self->bar[0], E1KE_TXDCTL(0)));
  val = 1 << E1KE_TXDCTL_WTHRESH; /* set write back threashold to 1 */
  val |= (1 << E1KE_TXDCTL_GRAN); /* granularity in descriptors */
  wt_reg(self->bar[0], E1KE_TXDCTL(0), val);
		   
  lrt_printf("after initialize txdctl register\n");
  print_txdctl_reg(rd_reg(self->bar[0], E1KE_TXDCTL(0)));

  val = (0xF<<E1KE_TCTL_CT) | (0x3f <<E1KE_TCTL_COLD) | (1<<E1KE_TCTL_PSP) 
    | (1 <<E1KE_TCTL_EN);
  wt_reg(self->bar[0], E1KE_TCTL, val);

  wt_reg(self->bar[0], E1KE_TDBAL(0), ((uintptr_t)self->tx_ring_p) & 0xFFFFFFFF);
  wt_reg(self->bar[0], E1KE_TDBAH(0), ((uintptr_t)self->tx_ring_p) >> 32);
  wt_reg(self->bar[0], E1KE_TDLEN(0), sizeof(struct le_e1ke_tx_desc)*NUM_TX_BUFS);
}

static void inline
setup_next_receive_buffer(EthMgrE1000ERef self) 
{
  EBBRC rc;
  volatile struct le_e1ke_rc_desc *rc_desc;
  void *buf;

  rc_desc = &self->rc_ring_p[self->rc_tail];

  rc = EBBPrimMalloc(E1KE_BUFLEN, &buf, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  rc_desc->val = 0;
  rc_desc->buf_add = buf;
}

static inline void
EthMgrE1000E_handleReceives(EthMgrE1000ERef self) 
{
  uint32_t hd = 0;
  hd = rd_reg(self->bar[0], E1KE_RDH(0));
  uint32_t cur = self->rc_last_head;

  while (cur != hd) {
    lrt_trans_rep_ref ref = lrt_trans_id_dref(self->handlerId);
    ((ethPacketHandler)(ref->ft[self->handlerFuncNum]))
      (ref, self->rc_ring_p[cur].buf_add);

    // reinit this entry
    self->rc_ring_p[cur].val = 0;
    
    // advance rc_last_head
    cur = (cur+1)%NUM_RC_BUFS;

    // advance tail
    advance_receive_tail(self);

    // FIXME: get rid of this
    uint32_t tmp = rd_reg(self->bar[3], MSIXPBA);
    lrt_printf("MSIX PBA (pending bits) %x\n", tmp);
    e1000e_clear_all_interrupts(self->bar[0]);
    // FIXME: end get rid of this
  }
  self->rc_last_head = cur;
}

EBBRC
EthMgrE1000E_readPoll(EthMgrE1000ERef self)
{
  EBBRC rc;
  
  EthMgrE1000E_handleReceives(self);

  rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, self->pollEvent,
                    EVENT_LOC_SINGLE, MyEventLoc());
  LRT_RCAssert(rc);
  return EBBRC_OK;
}

EBBRC
EthMgrE1000E_int0(EthMgrE1000ERef self)
{
  lrt_printf("--------------------EthMgrE1000E_int0----------------\n");
  return EBBRC_OK;
}
EBBRC
EthMgrE1000E_int1(EthMgrE1000ERef self)
{
  lrt_printf("--------------------EthMgrE1000E_int1----------------\n");
  return EBBRC_OK;
}
EBBRC
EthMgrE1000E_int2(EthMgrE1000ERef self)
{
  lrt_printf("--------------------EthMgrE1000E_int2----------------\n");
  return EBBRC_OK;
}
EBBRC
EthMgrE1000E_int3(EthMgrE1000ERef self)
{
  lrt_printf("--------------------EthMgrE1000E_int3----------------\n");
  return EBBRC_OK;
}
EBBRC
EthMgrE1000E_int4(EthMgrE1000ERef self)
{
  lrt_printf("--------------------EthMgrE1000E_int4----------------\n");
  return EBBRC_OK;
}
EBBRC
EthMgrE1000E_intMSI(EthMgrE1000ERef self)
{
  uint32_t i;
  i = e1000e_clear_all_interrupts(self->bar[0]);
  lrt_printf("got interrupt ICR-%x\n", i);
  return EBBRC_OK;
}

static inline void
bindReadPoll(EthMgrE1000ERef self)
{
  EBBRC rc;
  rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &self->pollEvent);
  LRT_RCAssert(rc);
  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, self->pollEvent, 
		    (EBBId)self->myID,
                    COBJ_FUNCNUM(self, readPoll));
  LRT_RCAssert(rc);
  rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, self->pollEvent,
                    EVENT_LOC_SINGLE, MyEventLoc());
  LRT_RCAssert(rc);

}

static inline void
EthMgrE1000E_setup_receive(EthMgrE1000ERef self)
{
  uint64_t mac;

  if (rd_reg(self->bar[0], E1KE_RAH(0)) & (1<<31)) {
    lrt_printf("mac address is valid\n");
    mac = (((uint64_t)rd_reg(self->bar[0], E1KE_RAH(0)))<<32) | 
      rd_reg(self->bar[0], E1KE_RAL(0));
    mac = mac & ~(0xffffLL<<48);	/* mask off upper bits of crap */
    unsigned char *c = (unsigned char *)&mac;
    lrt_printf("address is: %02x:%02x:%02x:%02x:%02x:%02x\n", 
	       c[0], c[1], c[2], c[3], c[4], c[5]);
  } else {
    LRT_Assert(0);		/* invalid mac address */
  }

  self->rc_tail = 0;
  // for now, initialize all the receive ring entries
  // with a receive buffer
  for (int i=0; i<NUM_RC_BUFS; i++) {
    setup_next_receive_buffer(self);
  }
  // need to have tail point to 1 less than the end
  for (int i=0; i<NUM_RC_BUFS-1; i++) {
    advance_receive_tail(self);
  }

  EthMgrE1000E_bindPacketHandler(
    self, (EBBId)self->myID, COBJ_FUNCNUM_FROM_TYPE(
      CObjInterface(EthMgrE1000E),defPacketHandler));

  // setup the receive buffer ring
  lrt_printf("address of rc ring is %lx\n", (long unsigned)&self->rc_ring_p[0]);
  wt_reg(self->bar[0], E1KE_RDBAL(0), ((uintptr_t)&self->rc_ring_p[0]) 
	 & 0xFFFFFFFF);
  wt_reg(self->bar[0], E1KE_RDBAH(0), ((uintptr_t)&self->rc_ring_p[0]) >> 32);
  wt_reg(self->bar[0], E1KE_RDLEN(0), 
	 sizeof(struct le_e1ke_rc_desc)*NUM_RC_BUFS);

  uint32_t val;
  lrt_printf("enabling interrupt, setting to promiscuous\n");
  // setup the read control register
  val = (1 << E1KE_RCTL_EN);	/* enable */
  val |= (1 << E1KE_RCTL_SBP); /* FIXME: store bad packets for now */
  val |= (1 << E1KE_RCTL_UPE); /* FIXME: lets be promiscuous */
  val |= (1 << E1KE_RCTL_LBM); /* FIXME: doing loopback */
  val |= (1 << E1KE_RCTL_BAM); /* FIXME: accept broadcasts */

  LRT_Assert(E1KE_BUFLEN == 2048); /* else, set BSIZE */
  wt_reg(self->bar[0], E1KE_RCTL, val);

  // FIXME: took out polling, we might want to re-enable with a timeout
  //bindReadPoll(self);
}

static inline EBBRC
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
  // while (!tx_desc->dd) {lrt_printf("*");}; 
  lrt_printf("we think the write got out\n");
  return len;
}


static inline EBBRC
EthMgrE1000E_setup_MSIX(EthMgrE1000ERef self)
{
  EBBRC rc;
  // allocate the events for MSI-X interrupts
  for (int i; i<5; i++) {
    rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &self->msixev[i]);
    LRT_RCAssert(rc);
  }
  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, self->msixev[0],
		    (EBBId)self->myID, 
                    COBJ_FUNCNUM(self, int0));
  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, self->msixev[1],
		    (EBBId)self->myID, 
                    COBJ_FUNCNUM(self, int1));
  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, self->msixev[2],
		    (EBBId)self->myID, 
                    COBJ_FUNCNUM(self, int2));
  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, self->msixev[3],
		    (EBBId)self->myID, 
                    COBJ_FUNCNUM(self, int3));
  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, self->msixev[4],
		    (EBBId)self->myID, 
                    COBJ_FUNCNUM(self, int4));


  // set the address to my local core
  uint32_t destid, redHint = 0, destMode = 0;
  destid = lrt_event_loc2apicid(MyEventLoc());
  lrt_printf("directing interrupts to apic id %x\n", destid);
  uint32_t msiadd = 0xfee<<20 | destid<<12 | redHint<<3 | destMode <<2;

  for(int i=0;i<5;i++) {
    uint32_t msidata;

    // for intel, upper address always 0
    wt_reg(self->bar[3], MSIXTUADD(i), 0);

    // lower addres as constructed above
    wt_reg(self->bar[3], MSIXTADD(i), msiadd);

    // set the data to vector number, eveyrthing else 0 ok from intel manuel
    // pg 10.11.3 Message Data Register Format
    // yuck, we add 32 since Dan substracted 32 reserved vectors from 
    // the number returned for event number
    msidata = self->msixev[i]+32;
    msidata |= 1<<15; // make it level triggered
    msidata |= 1<<14; // make it level triggered
    wt_reg(self->bar[3], MSIXTMSG(i), msidata);

    // enable interrupts on this vector, 0 enables
    wt_reg(self->bar[3], MSIXTVCTRL(i), 0);
  }
  
  uint32_t ivar;
  for (int i=0;i<5;i++) {
    ivar = i<<(i*4); // map read receive queue i to vector i
    ivar |= 1<<(i*4+3); 		/* enable it */
  }
  wt_reg(self->bar[0], E1KE_IVAR, ivar);

  uint32_t ims;
  ims = E1KE_IMS_RXDMT0 | E1KE_IMS_RXQ0| E1KE_IMS_RXQ1| E1KE_IMS_TXQ0| 
    E1KE_IMS_TXQ1| E1KE_IMS_OTHER;
  wt_reg(self->bar[0], E1KE_IMS, ims);

  // This will enable autoclearning of the interrupt
  // after the message has been sent. FIXME: enable this
  // once we have interrupts working. 
  uint32_t eiac;
  eiac = 1<<20 | 1<<21  | 1<<22  | 1<<23  | 1<<24;
  wt_reg(self->bar[0], E1KE_EIAC, eiac);
  // now enable msix in the configuraiton space
  pci_enable_msix(&self->pi);

  return EBBRC_OK;
}

static EBBRC
EthMgrE1000E_setup_MSI(EthMgrE1000ERef self)
{
  EBBRC rc;
  // allocate the events for MSI interrupts
  rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &self->msiev);
  LRT_RCAssert(rc);

  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, self->msiev,
		    (EBBId)self->myID, 
                    COBJ_FUNCNUM(self, intMSI));

  // set the address to my local core
  uint32_t destid, redHint = 0, destMode = 0;
  destid = lrt_event_loc2apicid(MyEventLoc());

  lrt_printf("directing interrupts to apic id %x\n", destid);
  uint32_t msiadd = 0xfee<<20 | destid<<12 | redHint<<3 | destMode <<2;
  uint16_t msidata;

  // set the data to vector number, eveyrthing else 0 ok from intel manuel
  // pg 10.11.3 Message Data Register Format
  // yuck, we add 32 since Dan substracted 32 reserved vectors from 
  // the number returned for event number
  msidata = self->msiev+32;
  msidata |= 1<<15; // make it level triggered
  msidata |= 1<<14; // make it level triggered

  uint32_t ims;
  ims = E1KE_IMS_RXDMT0 | E1KE_IMS_RXQ0| E1KE_IMS_RXQ1| E1KE_IMS_TXQ0| 
    E1KE_IMS_TXQ1| E1KE_IMS_OTHER;
  wt_reg(self->bar[0], E1KE_IMS, ims);

  pci_enable_msi(&self->pi, msiadd, 0, msidata);

  return EBBRC_OK;
}

static EBBRC
EthMgrE1000E_init(void *_self, EthMgrId id)
{
  EthMgrE1000ERef self = (EthMgrE1000ERef)_self;
  EBBRC rc;
  uint32_t tmp;
  
  self->myID = id;

  // needs to be 16 byte aligned
  rc = EBBPrimMalloc(((sizeof(struct le_e1ke_tx_desc)*NUM_TX_BUFS) + 16), 
		     &self->tx_ring_base_add, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);
  rc = EBBPrimMalloc(((sizeof(struct le_e1ke_rc_desc)*NUM_RC_BUFS) + 16), 
		     &self->rc_ring_base_add, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  self->tx_ring_p = (void *)(((uint64_t)self->tx_ring_base_add+15) & ~0xF);
  self->rc_ring_p = (void *)(((uint64_t)self->rc_ring_base_add+15) & ~0xF);

  lrt_printf(" wt ring buf %lx, ptr %lx, len %ld\n", 
	     (uint64_t)self->tx_ring_base_add, (uint64_t)self->tx_ring_p,
	     ((sizeof(struct le_e1ke_tx_desc)*NUM_TX_BUFS) + 16));
  lrt_printf(" rd ring buf %lx, ptr %lx\n", 
	     (uint64_t)self->rc_ring_base_add, (uint64_t)self->rc_ring_p);

  for (int i=0; i<6 ; i++) {
    self->bar[i] = pci_config_read32(&self->pi, 0x10+i*sizeof(uint32_t));
  }

  pci_enable_bus_master(&self->pi);

  // dump_regs(self);		/* print out all key registers */

  e1000e_disable_all_interrupts(self->bar[0]);
  e1000e_reset_device(self->bar[0]);
  e1000e_disable_all_interrupts(self->bar[0]);
  e1000e_clear_all_interrupts(self->bar[0]);

  tmp = rd_reg(self->bar[0], E1KE_CTRL);
  tmp |= 1<<E1KE_CTRL_SLU_BIT;
  wt_reg(self->bar[0], E1KE_CTRL, tmp);

  tmp = rd_reg(self->bar[0], E1KE_CTRL_EXT);
  tmp |= 1<<E1KE_CTRL_EXT_DRV_LOAD;
  tmp |= 1<<E1KE_CTRL_EXT_PBA_SUPP;
  tmp |= 1<<E1KE_CTRL_EXT_EIAME;
  wt_reg(self->bar[0], E1KE_CTRL_EXT, tmp);

  EthMgrE1000E_setup_receive(self);
  EthMgrE1000E_setup_transmit(self);
#if 0
  lrt_printf("***** setting up to use MSIX *******\n");
  EthMgrE1000E_setup_MSIX(self);
#else
  lrt_printf("***** setting up to use MSI *******\n");
  EthMgrE1000E_setup_MSI(self);
#endif
  // lrt_printf("***after initialization:\n");
  // dump_regs(self);		/* print out all key registers */

#if 0
  lrt_printf("---- done initializing e1000e\n");
  EthMgrE1000E_write(self, "dan is a bozo", 14); 
#endif

  // EthMgrE1000E_dumpread(self);  

#if 0
  // testing triggering of reads
  for(int i=0; i<5; i++)
    rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, self->msixev[i], 
		      LRT_EVENT_LOC_SINGLE, MyEventLoc());
#endif

  
  // generate explicit interrupt, to see if we get it
  // wt_reg(self->bar[0], E1KE_ICS, 1<<20|1<<21|1<<22|1<<23|1<<24);
  // wt_reg(self->bar[0], E1KE_ICS, 1<<21);

#if 0
  // FIXME: get rid of this
  uint32_t tp = rd_reg(self->bar[3], MSIXPBA);
  while (tp == 0) { 
    lrt_printf("*");
    tp = rd_reg(self->bar[3], MSIXPBA);
  }
  lrt_printf("MSIX PBA (pending bits) %x\n", tp);
#endif
#if 0
  e1000e_clear_all_interrupts(self->bar[0]);
  tp = rd_reg(self->bar[3], MSIXPBA);
  lrt_printf("MSIX PBA (pending bits) %x\n", tp);
#endif
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
  return EBBRC_GENERIC_FAILURE;
}
  
CObjInterface(EthMgrE1000E) EthMgrE1000E_ftable = {
  // base functions of ethernet manager
  // OK: not entirely clear to me idea that JA had here, 
  // ignoring, and just implementing device driver for single
  // ethernet device
  .EthMgr_if = {
    .bind = EthMgrE1000E_bind,
    .inEvent = (GenericEventFunc)EthMgrE1000E_inEvent,
  },
  .readPoll = EthMgrE1000E_readPoll,
  .int0 = EthMgrE1000E_int0,
  .int1 = EthMgrE1000E_int1,
  .int2 = EthMgrE1000E_int2,
  .int3 = EthMgrE1000E_int3,
  .int4 = EthMgrE1000E_int4,
  .intMSI = EthMgrE1000E_intMSI,
  .defPacketHandler = EthMgrE1000E_defaultEthernetPacketHandler,
  .bindPacketHandler = EthMgrE1000E_bindPacketHandler
};

EBBRC
EthMgrE1000ECreate(EthMgrId *id)
{
  EBBRC rc;
  EthMgrE1000ERef repRef;
  CObjEBBRootSharedRef rootRef;
  
  rc = EBBPrimMalloc(sizeof(EthMgrE1000E), &repRef, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);

  // set pci information in device
  rc = pci_get_info(PCI_VENDOR_INTEL, PCI_INTEL_DEVID_E1000E, &repRef->pi);
  LRT_RCAssert(rc);		/* later check EBBRC_NOTFOUND */

  lrt_printf("found device, EthMgrE1000E: bus %d, slot %d\n",
	     repRef->pi.bus, repRef->pi.slot);


  repRef->ft = &EthMgrE1000E_ftable; 

  rc = EBBAllocPrimId((EBBId *)id);
  LRT_RCAssert(rc);

  rc = EthMgrE1000E_init(repRef, *id);
  // fixme, if fails should free storage and return
  LRT_RCAssert(rc);		/* later return EBBRC_NOTFOUND */

  rc = CObjEBBRootSharedCreate(&rootRef, (EBBRepRef)repRef);
  LRT_RCAssert(rc);

  rc = CObjEBBBind((EBBId)*id, rootRef); 
  LRT_RCAssert(rc);

  // do a write operation here

  return EBBRC_OK;
}

#ifndef LRT_ULNX
/*
 * For now, this driver function is called, probes the pci bus to see if it 
 * exists.  We need to debate this model, it assumes that the rigth device
 * drives, and just them, are built in. 
 */
EBBRC
EthMgrCreate(EthMgrId *id) 
{
  // pci_print_all();
  return EthMgrE1000ECreate(id);
}
#endif
