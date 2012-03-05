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

#include <arch/amd64/acpi.h>
#include <arch/amd64/apic.h>
#include <arch/amd64/ioapic.h>
#include <arch/amd64/acpica/include/acpi.h>
#include <l0/lrt/bare/arch/amd64/acpi.h>
#include <lrt/io.h>

#define ACPI_MAX_INIT_TABLES    16
static ACPI_TABLE_DESC TableArray[ACPI_MAX_INIT_TABLES];

void
acpi_init()
{
  ACPI_STATUS status;
  status = AcpiInitializeTables(TableArray, ACPI_MAX_INIT_TABLES, FALSE);
  EBBAssert(status == AE_OK);
  
  madt *madt_ptr;
  status = AcpiGetTable("APIC", 0, (ACPI_TABLE_HEADER **)&madt_ptr);
  EBBAssert(status == AE_OK);  

  uint32_t size = madt_ptr->header.Length - sizeof(madt);
  uint8_t *ptr = (uint8_t *)(madt_ptr + 1);
  do {
    if (*ptr == PROCESSOR_LOCAL_APIC) {
      lapic_structure *ls = (lapic_structure *)ptr;
      size -= ls->length;
      ptr += ls->length;
      //do nothing with the structure
    } else if (*ptr == IO_APIC) {
      ioapic_structure *is = (ioapic_structure *)ptr;
      size -= is->length;
      ptr += is->length;
      init_ioapic((uint32_t *)(uintptr_t)is->ioapic_address);
      EBB_LRT_printf("found ioapic table\n");
    } else if (*ptr == INTERRUPT_SOURCE_OVERRIDE) {
      iso_structure *is = (iso_structure *)ptr;
      size -= is->length;
      ptr += is->length;
      EBB_LRT_printf("IRQ %d is mapped to I/O APIC input %d\n", is->source, 
		     is->global_system_interrupt);
    } else {
      //No definitions for other structures yet!
      EBB_LRT_printf("Found MADT structed unimplimented: %d\n", *ptr);
      EBBAssert(0);
    }
  } while (size > 0);
}
