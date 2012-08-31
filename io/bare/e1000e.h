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
