#ifndef PPC64_ARCH_CPU_H
#define PPC64_ARCH_CPU_H

#include <stdint.h>

#include <lrt/assert.h>

//From A2 Processor User's Manual
// Preliminary 1.03 page 294
typedef union {
  uint32_t val;
  struct {
    uint32_t reserved0 :4;
    uint32_t ds :1; //data address space
    uint32_t is :1; //instruction address space
    uint32_t reserved1 :2;
    uint32_t fe1 :1; //FP Exception Mode 1
    uint32_t de :1; //debug interrupt enable
    uint32_t reserved2 :1;
    uint32_t fe0 :1; //FP Exception Mode 0
    uint32_t me :1; //machine check enable
    uint32_t fp :1; //floating point avilable
    uint32_t pr :1; //problem state
    uint32_t ee :1; //external enable
    uint32_t reserved3 :1;
    uint32_t ce :1; //critical enable
    uint32_t reserved4 :7;
    uint32_t spv :1; //vector available
    uint32_t ucle :1; //user cache locking enable
    uint32_t reserved5 :1;
    uint32_t gs :1; //guest state
    uint32_t reserved6 :2;
    uint32_t cm :1; //computation mode
  } __attribute__ ((packed));
} msr;

STATIC_ASSERT(sizeof(msr) == 4, "msr packing issue");

#define SPRN_EPCR (0x133)

//From A2 Processor User's Manual
// Preliminary 1.03 page 296
typedef union {
  uint32_t val;
  struct {
    uint32_t reserved0 :22;
    /* disable MAS interrupt updates for hypervisor*/
    uint32_t dmiuh :1; 
    /* disable TLB guest management instructions */
    uint32_t dgtmi :1;
    /* guest interrupt computation mode */
    uint32_t gicm :1;
    /* interrupt computation mode */
    uint32_t icm :1; 
    /* disable hypervisor debug */
    uint32_t duvd :1; 
    /* instruction storage interrupt directed to guest state*/
    uint32_t isigs :1; 
    /* data storage interrupt directed to guest state */
    uint32_t dsigs :1;
    /* instruction TLB error interrupt directed to guest state */
    uint32_t itlbgs :1;
    /* data TLB error interrupt directed to guest state */
    uint32_t dtlbgs :1;
    /* extern input interrupt directed to guest state */
    uint32_t extgs :1;
  } __attribute__ ((packed));
} epcr;

STATIC_ASSERT(sizeof(epcr) == 4, "epcr struct packing issue");

#endif
