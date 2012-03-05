#ifndef ARCH_AMD64_IOAPIC_H
#define ARCH_AMD64_IOAPIC_H

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

static volatile uint32_t *IOAPIC_BASE;

//IO APIC Registers
static volatile uint8_t IOAPIC_ID = 0;
static volatile uint8_t IOAPIC_VERSION = 1;
static volatile uint8_t IOAPIC_ARB = 2;
static volatile uint8_t IOAPIC_REDTBL_START = 0x10;
static volatile uint8_t IOAPIC_REDTBL_END = 0x3F;

typedef struct {
  volatile uint32_t IOREGSEL;
  uint8_t padding[12];
  volatile uint32_t IOWIN;
} ioapic;

STATIC_ASSERT(sizeof(ioapic) == 20, "ioapic packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t reserved :24;
    uint32_t addr :8;
  };
} ioregsel;

STATIC_ASSERT(sizeof(ioregsel) == 4, "ioregsel packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t reserved0 :4;
    uint32_t id :4;
    uint32_t reserved1 :24;
  };
} ioapicid;

STATIC_ASSERT(sizeof(ioapicid) == 4, "ioapicid packing issue");

typedef union {
  uint32_t val;
  struct {
    uint32_t reserved0 :8;
    uint32_t mde :8;
    uint32_t reserved1 :8;
    uint32_t version :8;
  };
} ioapicver;

STATIC_ASSERT(sizeof(ioapicver) == 4, "ioapicver packing issue");

typedef union {
  uint32_t val[2];
  struct {
    uint32_t destination :8;
    uint64_t reserved :39 __attribute__((packed));
    uint32_t mask :1;
    uint32_t trigger_mode :1;
    uint32_t remote_irr :1;
    uint32_t intpol :1;
    uint32_t delivs :1;
    uint32_t destmod :1;
    uint32_t delmod :3;
    uint32_t intvec :8;
  };
} ioredirect;

STATIC_ASSERT(sizeof(ioredirect) == 8, "ioredirect packing issue");

static inline void
init_ioapic(uint32_t *ioapic_addr) {
  IOAPIC_BASE = ioapic_addr;
  
}

#endif
