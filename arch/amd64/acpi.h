#ifndef ARCH_AMD64_ACPI_H
#define ARCH_AMD64_ACPI_H

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

#include <arch/amd64/acpica/include/acpi.h>
#include <lrt/assert.h>

enum {
  PROCESSOR_LOCAL_APIC_N = 0,
  IO_APIC_N = 1,
  INTERRUPT_SOURCE_OVERRIDE_N = 2,
  NMI_N = 3,
  LOCAL_APIC_NMI_N = 4,
  LOCAL_APIC_ADDRESS_OVERRIDE_N = 5,
  IO_SAPIC_N = 6,
  LOCAL_SAPC_N = 7,
  PLATFORM_INTERRUPT_SOURCES_N = 8,
  PLATFORM_LOCAL_X2APIC_N = 9,
  LOCAL_X2APIC_NMI_N = 10,
  GIC_N = 11,
  GICD_N = 12
};
  

// APIC Structure Types
static const uint8_t PROCESSOR_LOCAL_APIC = PROCESSOR_LOCAL_APIC_N;
static const uint8_t IO_APIC = IO_APIC_N;
static const uint8_t INTERRUPT_SOURCE_OVERRIDE = INTERRUPT_SOURCE_OVERRIDE_N;
static const uint8_t NMI = NMI_N;
static const uint8_t LOCAL_APIC_NMI = LOCAL_APIC_NMI_N;
static const uint8_t LOCAL_APIC_ADDRESS_OVERRIDE = LOCAL_APIC_ADDRESS_OVERRIDE_N;
static const uint8_t IO_SAPIC = IO_SAPIC_N;
static const uint8_t LOCAL_SAPC = LOCAL_SAPC_N;
static const uint8_t PLATFORM_INTERRUPT_SOURCES = PLATFORM_INTERRUPT_SOURCES_N;
static const uint8_t PLATFORM_LOCAL_X2APIC = PLATFORM_LOCAL_X2APIC_N;
static const uint8_t LOCAL_X2APIC_NMI = LOCAL_X2APIC_NMI_N;
static const uint8_t GIC = GIC_N;
static const uint8_t GICD = GICD_N;
  
typedef struct {
  ACPI_TABLE_HEADER header;
  uint32_t local_interrupt_controller_address;
  uint32_t flags;
} madt;

STATIC_ASSERT(sizeof(madt) == 44, "madt packing issue");

typedef struct {
  uint8_t type;
  uint8_t length;
  uint8_t acpi_processor_id;
  uint8_t apic_id;
  uint32_t flags;
} lapic_structure;

STATIC_ASSERT(sizeof(lapic_structure) == 8, "lapic_structure packing issue");

typedef struct {
  uint8_t type;
  uint8_t length;
  uint8_t ioapic_id;
  uint8_t reserved;
  uint32_t ioapic_address;
  uint32_t global_system_interrupt_address;
} ioapic_structure;

STATIC_ASSERT(sizeof(ioapic_structure) == 12, "ioapic_structure packing issue");

typedef struct {
  uint8_t type;
  uint8_t length;
  uint8_t acpi_processor_id;
  uint16_t flags;
  uint8_t local_apic_lint;
} __attribute__((packed)) local_apic_nmi_structure;

STATIC_ASSERT(sizeof(local_apic_nmi_structure) == 6, "local_apic_nmi_structure packing issue");

typedef struct {
  uint8_t type;
  uint8_t length;
  uint8_t bus;
  uint8_t source;
  uint32_t global_system_interrupt;
  uint16_t flags;
} iso_structure;

#endif
