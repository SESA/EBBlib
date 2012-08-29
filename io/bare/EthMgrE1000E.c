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
#include <inttypes.h>

#define NUM_RC_BUFS 8
#define NUM_TX_BUFS 8

/*
 * these are the addresses in the memory mapped space of the E1000 (or
 * really e1000e) for various control registers
 */
#define E1KE_CTRL 0x0	                 /* device control register (RW) */
#define E1KE_STATUS 0x08		 /* status register */
#define E1KE_CTRL_EXT 0x018              /* extended device control reg (RW) */
#define E1KE_MDIC     0x020              /* Managemt Data Int(MDI) contr reg */
#define E1KE_IMC 0xd8		         /* interrupt mask clear (WO) */
#define E1KE_ICR 0xc0		         /* interrupt cause read (RW) */
#define E1KE_GCR 0x5b00		         /* 3GIO control register (RW) */
#define E1KE_TXDCTL(N) (0x03828+N*0x100) /* transmit descriptor control (RW) */
#define E1KE_TCTL 0x0400		 /* transmit control (RW) */
#define E1KE_RXDCTL(N) (0x02828+N*0x100) /* receive descriptor control (RW) */
#define E1KE_RCTL 0x0100		 /* receive control (RW) */

#define E1KE_TDBAL(N) (0x3800+N*0x100)
#define E1KE_TDBAH(N) (0x3804+N*0x100)
#define E1KE_TDLEN(N) (0x3808+N*0x100)
#define E1KE_RDBAL(N) (0x2800+N*0x100)
#define E1KE_RDBAH(N) (0x2804+N*0x100)
#define E1KE_RDLEN(N) (0x2808+N*0x100)

#define E1KE_TDH(N) (0x3810+N*0x100) // transmit descriptor head
#define E1KE_TDT(N) (0x3818+N*0x100) // transmit descriptor tail

#define E1KE_RDH(N) (0x2810+N*0x100) // receive descriptor head
#define E1KE_RDT(N) (0x2818+N*0x100) // receive descriptor tail

#define E1KE_RAL(N) (0x5400+N*8)     // receive address register low
#define E1KE_RAH(N) (0x5404+N*8)     // receive address register low

// BIT definitons in CTRL register 
#define E1KE_CTRL_FD_BIT  0    /* full duplex 1- half, 1- full */
#define E1KE_CTRL_ASDE_BIT 6    /* auto speed detection enabled */
#define E1KE_CTRL_SLU_BIT 5    /* set link up */
#define E1KE_CTRL_SPEED_BIT 8    /* set link up */
#define E1KE_CTRL_RST_BIT 26   /* reset */


static inline void
print_ctrl_reg(uint32_t ctrl)
{
  lrt_printf("control register %x\n", ctrl);
  if (ctrl & 1<<E1KE_CTRL_FD_BIT) 
    lrt_printf("\t - full duplex\n"); 
  else     
    lrt_printf("\t - half duplex\n"); 
  if (ctrl & 1<<2) lrt_printf("\t GIO Master Disabled\n");
  if (ctrl & 1<<5) 
    lrt_printf("\t auto speed detection enabled\n");
  else 
    lrt_printf("\t auto speed detection disabled\n");
  if (ctrl & 1<<E1KE_CTRL_SLU_BIT) 
    lrt_printf("\t Set link up set\n");
  else 
    lrt_printf("\t ERROR: Set link up not set\n");
  switch ((ctrl>>8) & 0x3) {
  case 0:
    lrt_printf("\t speed set to 10 Mb/s\n");
    break; 
  case 1:
    lrt_printf("\t speed set to 100 Mb/s\n");
    break; 
  case 2:
    lrt_printf("\t speed set to 1000 Mb/s\n");
    break; 
  case 3:
    lrt_printf("\t ERROR speed undefined\n");
    break; 
  }
  if (ctrl & 1<<11) lrt_printf("\t ERROR force speed\n");
  if (ctrl & 1<<12) lrt_printf("\t ERROR force duplex\n");
  if (ctrl & 1<<20) lrt_printf("\t ERROR D3Cold\n");
  if (ctrl & 1<<26) lrt_printf("\t Device Reset - only write\n");
  if (ctrl & 1<<27) 
    lrt_printf("\t receive flow control enabled\n");
  else
    lrt_printf("\t receive flow control disabled\n");
  if (ctrl & 1<<28) 
    lrt_printf("\t transmit flow control enabled\n");
  else
    lrt_printf("\t transmit flow control disabled\n");
  if (ctrl & 1<<30) 
    lrt_printf("\t VLAN mode enabled\n");
  else
    lrt_printf("\t VLAN mode disabled\n");
}

static inline void
print_tctl_reg(uint32_t r)
{
  uint32_t tmp;

  printf("printing out transmit control register (TCTL)\n");

  lrt_printf("\t EN trasmitter: ");
  if (r& 1<<1) 
    lrt_printf("enabled\n");
  else 
    lrt_printf("DISABLED\n");

  lrt_printf("\t PSP pad short packets: ");
  if (r& 1<<3) 
    lrt_printf("enabled\n");
  else 
    lrt_printf("DISABLED\n");

  tmp = (r>>4)&0xff;
  lrt_printf("\t CT - retransmit on collision set to %d\n", tmp);  

  tmp = (r>>12)&0x3ff;
  lrt_printf("\t COLD - collision distance set to %d\n", tmp);  

  lrt_printf("\t SWXOFF: ");
  if (r& 1<<22) 
    lrt_printf("enabled\n");
  else 
    lrt_printf("DISABLED\n");

  LRT_Assert(!(r& 1<<23));	/* don't support packet bursting */

  tmp = (r>>26)&0x3;
  lrt_printf("\t TXDSCMT - transmit descriptor minimum threashold\n");  

  lrt_printf("\t MULR: ");
  if (r& 1<<28) 
    lrt_printf("enabled\n");
  else 
    lrt_printf("DISABLED\n");
}

// BIT definitons in CTRL_EXT register 
#define E1KE_CTRL_EXT_DRV_LOAD 28 /* driver loaded */

// BIT definitions in transmit descriptor control
#define E1KE_TXDCTL_WTHRESH 16 	        /* bits 16-21 */
#define E1KE_TXDCTL_GRAN 24		/* bit 24  */

static inline void
print_txdctl_reg(uint32_t r)
{
  lrt_printf("transmit descriptor control register: %x\n", r);
  lrt_printf("\t prefetch threashold is %d\n", (r & 0x3f));
  lrt_printf("\t host threashold is %d\n", ((r>>8) & 0x3f));
  lrt_printf("\t write back threashold is %d\n", ((r>>16) & 0x3f));
  if (r & (1<<24)) 
    lrt_printf("\t GRAN: granularity is in descriptors\n");
  else 
    lrt_printf("\t GRAN: granularity is in cache lines\n");
  lrt_printf("\t transmit desciptor low thershold is %d\n", ((r>>25) & 0x3f));
}

// bit defintions for transmit control register
#define E1KE_TCTL_EN 1	/* enable */
#define E1KE_TCTL_COLD 12	/* bits 12-21, collision distance */
#define E1KE_TCTL_CT 4	/* bits 4-11 collision threashold */
#define E1KE_TCTL_PSP 3  	/* pad short packets */

// bit definitions for receive control register
#define E1KE_RCTL_EN 1		/* enable reception */
#define E1KE_RCTL_SBP 2		/* store bad backets */
#define E1KE_RCTL_UPE 3		/* unicast promiscuous enable */
#define E1KE_RCTL_MPE 4		/* multicast promiscuous enable */
#define E1KE_RCTL_LPE 5		/* long package enable */
#define E1KE_RCTL_LBM 6		/* 7:6 00-normal, 01 - mac loop */
#define E1KE_RCTL_RDMTS 8 	/* 9:8 receive desc minimum threashold sz */
#define E1KE_RCTL_DTYP 10	/* 11:10 decriptor type, 00 legacy */
#define E1KE_RCTL_MO   12	/* multicast offset */
#define E1KE_RCTL_BAM  15	/* broadcast accept mode */
#define E1KE_RCTL_BSIZE 16 /* buf size 00 - 2048, 01 -1024, if not BSEX */

#define E1KE_MAX_PACKLEN 1514
#define E1KE_BUFLEN 2048

// little endian transmit descriptor
struct le_e1ke_tx_desc {
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

// little endian recieve descriptor
struct le_e1ke_rc_desc {
  uint64_t *buf_add;
  union {
    uint64_t val;
    struct {
      uint16_t len;
      uint16_t checksum;
      union {
	uint8_t status;
	struct {
	  uint8_t dd:1;
	  uint8_t eop:1;
	  uint8_t rsvd:1;
	  uint8_t vp:1;
	  uint8_t udpcs:1;
	  uint8_t tcpcs:1;
	  uint8_t ipcs:1;
	  uint8_t rsvd2:1;
	};
      };
      union {
	uint8_t error;
	struct {
	  uint8_t ce:1;
	  uint8_t se:1;
	  uint8_t seq:1;
	  uint8_t rsv:1;
	  uint8_t cxe:1;
	  uint8_t tcpe:1;
	  uint8_t ipe:1;
	  uint8_t rxe:1;
	};
      };
      uint16_t vlantag;
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

static inline uint32_t
rd_phys(uint32_t bar, uint32_t offset)
{
  uint32_t val;
  LRT_Assert(offset<32);
  val = offset << 16;
  val |= 1<<21;			/* gigabit phy */
  val |= 0x2<<26; 		/* op-code is a read */
  wt_reg(bar, E1KE_MDIC, val);
  do {
    val = rd_reg(bar, E1KE_MDIC);
  } while ((val & (1<<28)) == 0);
  return val & 0xffff;
}


static inline void
dump_phys_ctrl(uint32_t bar) 
{
  uint32_t r;
  uint32_t tmp;
  lrt_printf("\t control register\n");
  r = rd_phys(bar, 0);
  if (r & 1<<15) 
    lrt_printf("\t\t PHY reset reset\n");
  else 
    lrt_printf("\t\t PHY reset normal operation\n");

  
  //or in MSB and LSB of speed select
  tmp = (r>>13&1) | (r>>5&2);
  switch (tmp) {
  case 0:
    lrt_printf("\t\t speed set to 10 Mb/s\n");
    break; 
  case 1:
    lrt_printf("\t\t speed set to 100 Mb/s\n");
    break; 
  case 2:
    lrt_printf("\t\t speed set to 1000 Mb/s\n");
    break; 
  case 3:
    lrt_printf("\t\t ERROR speed undefined\n");
    break; 
  }

  if (r & 1<<12) 
    lrt_printf("\t\t auto negotiation enabled\n");
  else 
    lrt_printf("\t\t auto negotiation disabled\n");
}

static inline void
dump_phys_autoneg(uint32_t bar)
{
  uint32_t r;
  lrt_printf("\t auto negotiation register\n");
  r = rd_phys(bar, 4);
  if (r & 1<<10) {
    lrt_printf("\t\t pause enabled\n");
    LRT_Assert(0); // need to set flow control in ctrl register
  } else 
    lrt_printf("\t\t pause disabled\n");
  if (r & 1<<11) {
    lrt_printf("\t\t asymetric pause enabled\n");
    LRT_Assert(0); // need to set flow control in ctrl register
  } else 
    lrt_printf("\t\t asymetric pause disabled\n");
}

static inline void
dump_phys(uint32_t bar) 
{
  lrt_printf("---- dumping phys information for gigbit phy\n");
  dump_phys_ctrl(bar);
  dump_phys_autoneg(bar);
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

static struct le_e1ke_tx_desc tx_ring[NUM_TX_BUFS] __attribute__ ((aligned(16)));
static struct le_e1ke_rc_desc rc_ring[NUM_RC_BUFS] __attribute__ ((aligned(16)));

CObject(EthMgrE1000E) {
  COBJ_EBBFuncTbl(EthMgr);
  
  struct pci_info pi;
  uint32_t bar;			/* base address register, i.e., base address
				   for memory mapped device registers */
  // ring of control information for transmission
  unsigned tx_tail;
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
  // comment this out when everything is working, since
  // this is expensive
  LRT_Assert(self->tx_tail == rd_reg(self->bar, E1KE_TDT(0)));
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
  // comment this out when everything is working, since
  // this is expensive
  LRT_Assert(self->rc_tail == rd_reg(self->bar, E1KE_RDT(0)));

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

  rc = EBBPrimMalloc(E1KE_BUFLEN, &buf, EBB_MEM_DEFAULT);
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
    setup_next_receive_buffer(self);
  }

  // setup the receive buffer ring
  lrt_printf("address of rc ring is %lx\n", (long unsigned)&self->rc_ring_p[0]);
  wt_reg(self->bar, E1KE_RDBAL(0), ((uintptr_t)&self->rc_ring_p[0]) & 0xFFFFFFFF);
  wt_reg(self->bar, E1KE_RDBAH(0), ((uintptr_t)&self->rc_ring_p[0]) >> 32);
  wt_reg(self->bar, E1KE_RDLEN(0), sizeof(struct le_e1ke_rc_desc)*NUM_RC_BUFS);

  uint32_t val;
  // setup the read control register
  val = (1 << E1KE_RCTL_EN);	/* enable */
  val |= (1 << E1KE_RCTL_SBP); /* FIXME: store bad packets for now */
  val |= (1 << E1KE_RCTL_UPE); /* FIXME: lets be sexy */
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


static EBBRC
EthMgrE1000E_init(void *_self)
{
  EthMgrE1000ERef self = (EthMgrE1000ERef)_self;
  EBBRC rc;
  uint32_t tmp;


  self->tx_ring_p = &tx_ring[0];
  self->rc_ring_p = &rc_ring[0];
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
