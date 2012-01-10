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

static volatile uint32_t * const APIC_BASE = (uint32_t *)0x0FEE0000;

//offsets
static const uint32_t APIC_ID_REGISTER = 0x20;
static const uint32_t APIC_VERSION_REGISTER = 0x30;
static const uint32_t APIC_ICR_LOW = 0x300;
static const uint32_t APIC_ICR_HIGH = 0x310;

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
} apic_icr_low;

_Static_assert(sizeof(apic_icr_low) == 4, "apic_icr_low packing issue");

typedef union {
  uint32_t raw;
  struct {
    uint32_t reserved :24;
    uint32_t destination :8;
  };
} apic_icr_high;

_Static_assert(sizeof(apic_icr_high) == 4, "apic_icr_high packing issue");

//small inline to just do the address computation
static inline volatile uint32_t *
apic_addr(uint32_t offset) {
  uintptr_t ptr = (uintptr_t)APIC_BASE;
  ptr += offset;
  return (volatile uint32_t *) ptr;
}

static inline void
enable_lapic(void)
{
  uint64_t apic_base;
  __asm__ volatile (
	 "rdmsr"
	 : "=A" (apic_base)
	 : "c" (MSR_APIC_BASE)
	 );

  apic_base |= MSR_APIC_BASE_GLOBAL_ENABLE;

  __asm__ volatile (
	 "wrmsr"
	 :
	 : "A" (apic_base), "c" (MSR_APIC_BASE)
	 );
}

//TODO DS: Make a bitfield that matches the registers
static inline uint32_t
get_apicid(void)
{
  return (*apic_addr(APIC_ID_REGISTER) >> 24) && 0xFF;
}


#endif
