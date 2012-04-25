#ifndef ARCH_AMD64_APIC_H
#define ARCH_AMD64_APIC_H

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

#include <stdint.h>

#include <arch/amd64/cpu.h>
#include <lrt/assert.h>

typedef union {
  uint32_t raw;
  struct {
    uint32_t reserved :24;
    uint32_t lapicid :8;
  };
} lapic_id_register;

STATIC_ASSERT(sizeof(lapic_id_register) == 4, "lapic_id_register packing issue");

typedef union {
  uint32_t raw;
  struct {
    uint32_t version :8;
    uint32_t reserved0 :8;
    uint32_t max_lvt_entry :8;
    uint32_t eoi_broadcast_suppression :1;
    uint32_t reserved1 :7;
  };
} lapic_version_register;

STATIC_ASSERT(sizeof(lapic_version_register) == 4,
	       "lapic_version_register packing issue");

typedef union {
  uint32_t raw;
} lapic_eoi_register;

STATIC_ASSERT(sizeof(lapic_eoi_register) == 4,
	       "lapic_eoi_register packing issue");

typedef union {
  uint32_t raw;
  struct {
    uint32_t vector :8;
    uint32_t delivery_mode :3;
    uint32_t destination_mode :1;
    uint32_t delivery_status :1;    
    uint32_t reserved0 :1;
    uint32_t level :1;
    uint32_t trigger_mode :1;
    uint32_t reserved1 :2;
    uint32_t destination_shorthand :2;
    uint32_t reserved2 :12;
  };
} lapic_icr_low;

STATIC_ASSERT(sizeof(lapic_icr_low) == 4, "lapic_icr_low packing issue");

typedef union {
  uint32_t raw;
  struct {
    uint32_t reserved :24;
    uint32_t destination :8;
  };
} lapic_icr_high;

STATIC_ASSERT(sizeof(lapic_icr_high) == 4, "lapic_icr_high packing issue");

typedef union {
  uint32_t raw[256];
  struct {
    volatile uint32_t reserved0 __attribute__((aligned(16)));
    volatile uint32_t reserved1 __attribute__((aligned(16)));
    volatile lapic_id_register lir __attribute__((aligned(16)));
    volatile lapic_version_register lvr __attribute__((aligned(16)));
    volatile uint32_t reserved2 __attribute__((aligned(16)));
    volatile uint32_t reserved3 __attribute__((aligned(16)));
    volatile uint32_t reserved4 __attribute__((aligned(16)));
    volatile uint32_t reserved5 __attribute__((aligned(16)));
    volatile uint32_t tpr __attribute__((aligned(16)));
    volatile uint32_t apr __attribute__((aligned(16)));
    volatile uint32_t ppr __attribute__((aligned(16)));
    volatile lapic_eoi_register ler __attribute__((aligned(16)));
    volatile uint32_t rrd __attribute__((aligned(16)));
    volatile uint32_t ldr __attribute__((aligned(16)));
    volatile uint32_t dfr __attribute__((aligned(16)));
    volatile uint32_t sivr __attribute__((aligned(16)));
    volatile uint32_t isr_31_0 __attribute__((aligned(16)));
    volatile uint32_t isr_63_32 __attribute__((aligned(16)));
    volatile uint32_t isr_95_64 __attribute__((aligned(16)));
    volatile uint32_t isr_127_96 __attribute__((aligned(16)));
    volatile uint32_t isr_159_128 __attribute__((aligned(16)));
    volatile uint32_t isr_191_160 __attribute__((aligned(16)));
    volatile uint32_t isr_223_192 __attribute__((aligned(16)));
    volatile uint32_t isr_255_224 __attribute__((aligned(16)));
    volatile uint32_t tmr_31_0 __attribute__((aligned(16)));
    volatile uint32_t tmr_63_32 __attribute__((aligned(16)));
    volatile uint32_t tmr_95_64 __attribute__((aligned(16)));
    volatile uint32_t tmr_127_96 __attribute__((aligned(16)));
    volatile uint32_t tmr_159_128 __attribute__((aligned(16)));
    volatile uint32_t tmr_191_160 __attribute__((aligned(16)));
    volatile uint32_t tmr_223_192 __attribute__((aligned(16)));
    volatile uint32_t tmr_255_224 __attribute__((aligned(16)));
    volatile uint32_t irr_31_0 __attribute__((aligned(16)));
    volatile uint32_t irr_63_32 __attribute__((aligned(16)));
    volatile uint32_t irr_95_64 __attribute__((aligned(16)));
    volatile uint32_t irr_127_96 __attribute__((aligned(16)));
    volatile uint32_t irr_159_128 __attribute__((aligned(16)));
    volatile uint32_t irr_191_160 __attribute__((aligned(16)));
    volatile uint32_t irr_223_192 __attribute__((aligned(16)));
    volatile uint32_t irr_255_224 __attribute__((aligned(16)));
    volatile uint32_t esr __attribute__((aligned(16)));
    volatile uint32_t reserved6 __attribute__((aligned(16)));
    volatile uint32_t reserved7 __attribute__((aligned(16)));
    volatile uint32_t reserved8 __attribute__((aligned(16)));
    volatile uint32_t reserved9 __attribute__((aligned(16)));
    volatile uint32_t reserved10 __attribute__((aligned(16)));
    volatile uint32_t reserved11 __attribute__((aligned(16)));
    volatile uint32_t lvt_cmci __attribute__((aligned(16)));
    volatile lapic_icr_low lil __attribute__((aligned(16)));
    volatile lapic_icr_high lih __attribute__((aligned(16)));
    volatile uint32_t lvt_timer __attribute__((aligned(16)));
    volatile uint32_t lvt_thermal __attribute__((aligned(16)));
    volatile uint32_t lvt_pmc __attribute__((aligned(16)));
    volatile uint32_t lvt_lint0 __attribute__((aligned(16)));
    volatile uint32_t lvt_lint1 __attribute__((aligned(16)));
    volatile uint32_t lvt_error __attribute__((aligned(16)));
    volatile uint32_t init_count __attribute__((aligned(16)));
    volatile uint32_t current_count __attribute__((aligned(16)));
    volatile uint32_t reserved12 __attribute__((aligned(16)));
    volatile uint32_t reserved13 __attribute__((aligned(16)));
    volatile uint32_t reserved14 __attribute__((aligned(16)));
    volatile uint32_t reserved15 __attribute__((aligned(16)));
    volatile uint32_t dcr __attribute__((aligned(16)));
    volatile uint32_t reserved16 __attribute__((aligned(16)));
  };
} lapic;

//just one sanity check
STATIC_ASSERT(offsetof(lapic, lih) == 0x310, "lapic alignment issue");
STATIC_ASSERT(sizeof(lapic) == 1024, "lapic packing issue");

//FIXME: This should be set dynamically probably
static lapic *LAPIC_BASE = (lapic *)0xfee00000;

static inline bool
has_lapic(void)
{
  uint32_t features, dummy;

  cpuid(CPUID_FEATURES, &dummy, &dummy, &dummy, &features);

  return (features & CPUID_EDX_HAS_LAPIC);
}

static inline void
enable_lapic(void)
{
  uint64_t lapic_base;
  uint32_t low, high;
  __asm__ volatile (
	 "rdmsr"
	 : "=a" (low), "=d" (high)
	 : "c" (MSR_LAPIC_BASE)
	 );
  
  lapic_base = ((uint64_t)high << 32) | low;

  lapic_base |= MSR_LAPIC_BASE_GLOBAL_ENABLE;

  low = lapic_base & 0xFFFFFFFF;
  high = (lapic_base >> 32) & 0xFFFFFFFF;

  __asm__ volatile (
	 "wrmsr"
	 :
	 : "a" (low), "d" (high), "c" (MSR_LAPIC_BASE)
	 );
}

static inline uint32_t
get_lapic_id(void)
{
  lapic_id_register lir = LAPIC_BASE->lir;
  return lir.lapicid;
}

static inline uint32_t
get_lapic_version(void)
{
  lapic_version_register lvr = LAPIC_BASE->lvr;
  return lvr.version;
}

static inline uint32_t
get_lapic_max_lvt_entry(void)
{
  lapic_version_register lvr = LAPIC_BASE->lvr;
  return lvr.max_lvt_entry;
}

static inline uint32_t
get_lapic_eoi_broadcast_suppression(void)
{
  lapic_version_register lvr = LAPIC_BASE->lvr;
  return lvr.eoi_broadcast_suppression;
}

//TODO: There is a bit that we probably want to check that tells us if the ipi
// was actually sent to the lapic or if it is still pending
static inline void
send_ipi(lapic_icr_low icr_low, lapic_icr_high icr_high)
{
  //IPI is fired on the lowest dword being set so we set high first
  LAPIC_BASE->lih.raw = icr_high.raw;

  LAPIC_BASE->lil.raw = icr_low.raw;
}

//To send an eoi any write to the eoi register will do
static inline void
send_eoi(void)
{
  LAPIC_BASE->ler.raw = 0;
}

#endif
