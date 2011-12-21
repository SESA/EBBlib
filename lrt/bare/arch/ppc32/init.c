#include <arch/ppc32/bpcore/ppc450_core.h>
#include <arch/ppc32/bpcore/ppc450_inlines.h>
#include <arch/ppc32/bpcore/ppc450_tlb.h>

struct bgp_mailbox_desc {
	u16 offset;	// offset from SRAM base
	u16 size;	// size including header, 0=not present
} __attribute__((packed));

typedef struct bgp_mailbox {
	volatile u16 command;	// comand; upper bit=ack
	u16 len;		// length (does not include header)
	u16 result;		// return code from reader
	u16 crc;		// 0=no CRC
	char data[0];
} bgp_mailbox;

#define BGP_MAILBOX_DESC_OFFSET		(0x7fd0)
#define BGP_DCR_TEST(x)			(0x400 + (x))
#define BGP_DCR_GLOB_ATT_WRITE_SET	BGP_DCR_TEST(0x17)
#define BGP_DCR_GLOB_ATT_WRITE_CLEAR	BGP_DCR_TEST(0x18)
#define BGP_DCR_TEST_STATUS6		BGP_DCR_TEST(0x3a)
#define   BGP_TEST_STATUS6_IO_CHIP	(0x80000000U >> 3)

#define BGP_ALERT_OUT(core)	        (0x80000000U >> (24 + core))
#define BGP_ALERT_IN(core)	        (0x80000000U >> (28 + core))

__attribute__ ((section (".text"))) void 
init(int cpu)
{
  //sets our PID to 0
  _bgp_mtspr(SPRN_PID, 0);
  
  //sets MMUCR_STID to 0
  uint32_t mmucr = _bgp_mfspr(SPRN_MMUCR); //read MMUCR
  mmucr &= ~MMUCR_STID(-1); //mask all but STID
  _bgp_mtspr(SPRN_MMUCR, mmucr); //set MMUCR
  
  uint32_t curr_entry = _bgp_isMapped(&init);


  //FIXME: get the mbox info from the dtree
  
  //find the first entry in the TLB that we don't depend on
  int i;
  for (i = 0; (i != curr_entry) && (i < _BGP_PPC450_TLB_SLOTS); i++) {
    ;
  }

  //map SRAM into the TLB
  void *sram_mapping = 0xD0000000;
  _BGP_TLB_Word0 t0;
  t0.word = 0;
  t0.epn = ((uint32_t)sram_mapping) >> 10;
  t0.v = 1; //valid
  t0.ts = 0; 
  t0.size = 3; //64K mapping
  t1.word = 0;
  t1.rpn = 0xFFFF8000 >> 10;
  t1.erpn = 0x7;
  t2.word = 0;
  t2.i = 1; //inhibit caching
  t2.g = 1; //guarded access
  //read/write, no execute
  t2.sx = 0;
  t2.sw = 1;
  t2.sr = 1;
  _bgp_mttlb(i, t0, t1, t2);
  _bgp_isync();

  struct bgp_mailbox_desc* mb_desc = sram_mapping + BGP_MAILBOX_DESC_OFFSET;
  mb_desc++; //first descriptor is in, we want out
  bgp_mailbox mb = sram_mapping + mb_desc->offset;
  //size of data buffer in mailbox
  uint32_t size = mb_desc->size - sizeof(struct bgp_mailbox);

  mbox->data[0] = "H";
  mbox->len = 1;
  mbox->command = 2;
  _bgp_mbar();
  _bgp_mtdcrx(BGP_DCR_GLOB_ATT_WRITE_SET, BGP_ALERT_OUT(0));

  while(1);
#if 0
  //invalid all tlb entries except the one we are executing from
  _BGP_TLB_Word0 inv0;
  inv0.word = 0;
  _BGP_TLB_Word1 inv1;
  inv1.word = 0;
  _BGP_TLB_Word2 inv2;
  inv2.word = 0;
  for(int i = 0; i < _BGP_PPC450_TLB_SLOTS; i++) {
    if (i == curr_entry)
      continue;
    _bgp_mttlb(i, inv0, inv1, inv2);
  }

  mmucr = MMUCR_U2SWOAE;
  _bgp_mtspr(SPRN_MMUCR, mmucr); //set MMUCR
  
  // Find the biggest page size smaller than our kernel size
  uint32_t log2size;
  for (log2size = KERNEL_AREA_LOG2SIZE;
       (log2size != _BGP_VMM_SHIFT_1K) && (log2size != _BGP_VMM_SHIFT_4K) &&
	 (log2size != _BGP_VMM_SHIFT_16K) && (log2size != _BGP_VMM_SHIFT_64K) &&
	 (log2size != _BGP_VMM_SHIFT_256K) && (log2size != _BGP_VMM_SHIFT_1M) &&
	 (log2size != _BGP_VMM_SHIFT_4M) && (log2size != _BGP_VMM_SHIFT_16M) &&
	 (log2size != _BGP_VMM_SHIFT_64M) && (log2size != _BGP_VMM_SHIFT_256M) &&
	 (log2size != _BGP_VMM_SHIFT_1G);
       log2size--) {
    ;
  }
  
  //init kernel TLB entries
  _BGP_TLB_Word0 t0;
  t0.word = 0;
  // smallest page size is 1K so the epn doesn't store the least
  //  significant 10 bits
  t0.epn = KERNEL_OFFSET >> 10; 
  // stores size as 4KB^size so we shift off the lower 10 bits again
  //  divide by two so bits are in powers of 4 and not 2
  t0.size = (log2size - 10) / 2
  _BGP_TLB_Word0 t1;
  t1.word = 0;
  _BGP_TLB_Word0 t2;
  t2.word = 0;
  t2.m = 1; //requires memory coherency
  // supervisor can read/write/execute
  t2.sx = 1;
  t2.sw = 1;
  t2.sr = 1;

  while (i = _BGP_PPC450_TLB_SLOTS - 1;
	 (t0.epn << 10) < KERNEL_AREA_END;
	 t0.epn += (1 << log2size >> 10), 
	 t1.epn += (1 << log2size >> 10), i--) {
    if (i == curr_entry)
      i--;
    _bgp_mttlb(i, t0, t1, t2);
  }
  _bgp_isync();
#endif
}
