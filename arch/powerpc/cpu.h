#ifndef PPC64_ARCH_CPU_H
#define PPC64_ARCH_CPU_H

#include <stdint.h>

#include <arch/ppc64/regs.h>
#include <lrt/assert.h>

//From A2 Processor User's Manual
// Preliminary 1.03 page 294
typedef union {
  uint32_t val;
  struct {
    uint32_t cm :1; //computation mode
    uint32_t reserved0 :2;
    uint32_t gs :1; //guest state
    uint32_t reserved1 :1;
    uint32_t ucle :1; //user cache locking enable
    uint32_t spv :1; //vector available
    uint32_t reserved2 :7;
    uint32_t ce :1; //critical enable
    uint32_t reserved3 :1;
    uint32_t ee :1; //external enable
    uint32_t pr :1; //problem state
    uint32_t fp :1; //floating point avilable
    uint32_t me :1; //machine check enable
    uint32_t fe0 :1; //FP Exception Mode 0
    uint32_t reserved4 :1;
    uint32_t de :1; //debug interrupt enable
    uint32_t fe1 :1; //FP Exception Mode 1
    uint32_t reserved5 :2;
    uint32_t is :1; //instruction address space
    uint32_t ds :1; //data address space
    uint32_t reserved6 :4;
  };
} msr;

STATIC_ASSERT(sizeof(msr) == 4, "msr packing issue");

static inline msr
get_msr(void)
{
  msr msr;
  asm volatile (
		"mfmsr %[msr]"
		: [msr] "=r" (msr)
		);
  return msr;
}

static inline void
set_msr(msr msr)
{
  asm volatile (
		"mtmsr %[msr]"
		:
		: [msr] "r" (msr)
		);
}

static inline void
set_ivpr(void *addr)
{
  asm volatile (
		"mtivpr %[addr]"
		:
		: [addr] "r" (addr)
		);
}

#define xstr(s) #s
#define str(s) xstr(s)

#define get_spr(spr)						\
  ({								\
  uint64_t val;							\
  asm volatile (						\
		"mfspr %[val]," str(spr)			\
		: [val] "=r" (val));				\
  val;								\
  })

#define set_spr(spr, val)			\
  asm volatile (				\
		"mtspr " str(spr) ",%[v]"	\
		:				\
		: [v] "r" (val));		

//From A2 Processor User's Manual
// Preliminary 1.03 page 296
typedef union {
  uint32_t val;
  struct {
    /* extern input interrupt directed to guest state */
    uint32_t extgs :1;
    /* data TLB error interrupt directed to guest state */
    uint32_t dtlbgs :1;
    /* instruction TLB error interrupt directed to guest state */
    uint32_t itlbgs :1;
    /* data storage interrupt directed to guest state */
    uint32_t dsigs :1;
    /* instruction storage interrupt directed to guest state*/
    uint32_t isigs :1; 
    /* disable hypervisor debug */
    uint32_t duvd :1; 
    /* interrupt computation mode */
    uint32_t icm :1; 
    /* guest interrupt computation mode */
    uint32_t gicm :1;
    /* disable TLB guest management instructions */
    uint32_t dgtmi :1;
    /* disable MAS interrupt updates for hypervisor*/
    uint32_t dmiuh :1; 
    uint32_t reserved0 :22;
  } __attribute__ ((packed));
} epcr;

STATIC_ASSERT(sizeof(epcr) == 4, "epcr struct packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t en_dcr :1;
    uint32_t en_trace :1;
    uint32_t en_pc :1;
    uint32_t ifratsc :9;
    uint32_t ifrat :1;
    uint32_t dfratsc :9;
    uint32_t dfrat :1;
    uint32_t ucode_dis :1;
    uint32_t ap :4;
    uint32_t en_attn :1;
    uint32_t en_ditc :1;
    uint32_t en_icswx :1;
    uint32_t notlb :1;
  };
} ccr2;

STATIC_ASSERT(sizeof(ccr2) == 4, "ccr2 struct packing issue");

#endif
