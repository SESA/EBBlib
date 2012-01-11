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

//FIXME: I guess this could be remapped at some point in the future so it
// shouldn't be a const and perhaps the initial value should be read from
// the correct MSR when the lapic is enabled
static volatile uint32_t * const LAPIC_BASE = (uint32_t *)0xFEE00000;

//offsets
static const uint32_t LAPIC_ID_REGISTER = 0x20;
static const uint32_t LAPIC_VERSION_REGISTER = 0x30;
static const uint32_t LAPIC_EOI_REGISTER = 0xB0;
static const uint32_t LAPIC_ICR_LOW = 0x300;
static const uint32_t LAPIC_ICR_HIGH = 0x310;

typedef union {
  uint32_t raw;
  struct {
    uint32_t reserved :24;
    uint32_t lapicid :8;
  };
} lapic_id_register;

_Static_assert(sizeof(lapic_id_register) == 4, "lapic_id_register packing issue");

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

_Static_assert(sizeof(lapic_version_register) == 4,
	       "lapic_version_register packing issue");

typedef union {
  uint32_t raw;
} lapic_eoi_register;

_Static_assert(sizeof(lapic_eoi_register) == 4,
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

_Static_assert(sizeof(lapic_icr_low) == 4, "lapic_icr_low packing issue");

typedef union {
  uint32_t raw;
  struct {
    uint32_t reserved :24;
    uint32_t destination :8;
  };
} lapic_icr_high;

_Static_assert(sizeof(lapic_icr_high) == 4, "lapic_icr_high packing issue");

//small inline to just do the address computation
static inline volatile uint32_t *
lapic_addr(uint32_t offset) {
  uintptr_t ptr = (uintptr_t)LAPIC_BASE;
  ptr += offset;
  return (volatile uint32_t *) ptr;
}

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
  __asm__ volatile (
	 "rdmsr"
	 : "=A" (lapic_base)
	 : "c" (MSR_LAPIC_BASE)
	 );

  lapic_base |= MSR_LAPIC_BASE_GLOBAL_ENABLE;

  __asm__ volatile (
	 "wrmsr"
	 :
	 : "A" (lapic_base), "c" (MSR_LAPIC_BASE)
	 );
}

static inline uint32_t
get_lapic_id(void)
{
  lapic_id_register lir = (lapic_id_register)(*lapic_addr(LAPIC_ID_REGISTER));
  return lir.lapicid;
}

static inline uint32_t
get_lapic_version(void)
{
  lapic_version_register lvr = 
    (lapic_version_register)*lapic_addr(LAPIC_VERSION_REGISTER);
  return lvr.version;
}

static inline uint32_t
get_lapic_max_lvt_entry(void)
{
  lapic_version_register lvr = 
    (lapic_version_register)*lapic_addr(LAPIC_VERSION_REGISTER);
  return lvr.max_lvt_entry;
}

static inline uint32_t
get_lapic_eoi_broadcast_suppression(void)
{
  lapic_version_register lvr = 
    (lapic_version_register)*lapic_addr(LAPIC_VERSION_REGISTER);
  return lvr.eoi_broadcast_suppression;
}

//TODO: There is a bit that we probably want to check that tells us if the ipi
// was actually send to the lapic or if it is still pending
static inline void
send_ipi(lapic_icr_low icr_low, lapic_icr_high icr_high)
{
  //IPI is fired on the lowest dword being set so we set high first
  volatile lapic_icr_high *icrh = 
    (lapic_icr_high *)lapic_addr(LAPIC_ICR_HIGH);
  icrh->raw = icr_high.raw;

  volatile lapic_icr_low *icrl = 
    (lapic_icr_low *)lapic_addr(LAPIC_ICR_LOW);
  icrl->raw = icr_low.raw;
}

//To send an eoi any write to the eoi register will do
static inline void
send_eoi(void)
{
  volatile lapic_eoi_register *eoir =
    (lapic_eoi_register *)lapic_addr(LAPIC_EOI_REGISTER);
  eoir->raw = 0;
}

#endif
