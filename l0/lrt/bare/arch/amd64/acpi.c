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

static struct {
  int num_cores;
  int bsp_id;
  void *io_apic_addr;
} acpi_startup_info;

int
acpi_get_num_cores()
{
  return acpi_startup_info.num_cores;
} 

int
acpi_get_bsp()
{
  return acpi_startup_info.bsp_id;
}

ioapic *
acpi_get_ioapic_addr()
{
  return (ioapic *)acpi_startup_info.io_apic_addr;
}


void
acpi_init()
{
  ACPI_STATUS status;
  status = AcpiInitializeTables(TableArray, ACPI_MAX_INIT_TABLES, FALSE);
  LRT_Assert(status == AE_OK);
  acpi_startup_info.num_cores = 0;
  acpi_startup_info.bsp_id = 0;
  acpi_startup_info.io_apic_addr = 0;

  madt *madt_ptr;
  status = AcpiGetTable("APIC", 0, (ACPI_TABLE_HEADER **)&madt_ptr);
  LRT_Assert(status == AE_OK);

  uint32_t size = madt_ptr->header.Length - sizeof(madt);
  uint8_t *ptr = (uint8_t *)(madt_ptr + 1);
  uint8_t ioapics = 0;

  do {
    switch( *ptr ) {
    case PROCESSOR_LOCAL_APIC_N: 
      {
	lapic_structure *ls = (lapic_structure *)ptr;
	size -= ls->length;
	ptr += ls->length;
	if (acpi_startup_info.num_cores == 0) {
	  //record apic id of boot processor
	  acpi_startup_info.bsp_id = ls->apic_id;
	}
	if(ls->flags&1) acpi_startup_info.num_cores++;
      }
      break;
    case IO_APIC_N:
      {
	ioapic_structure *is = (ioapic_structure *)ptr;
	size -= is->length;
	ptr += is->length;
	acpi_startup_info.io_apic_addr = (void *)(uintptr_t)is->ioapic_address;
	ioapics++;
      }
      break;
    case INTERRUPT_SOURCE_OVERRIDE_N:
      {
	iso_structure *is = (iso_structure *)ptr;
	size -= is->length;
	ptr += is->length;
      }
      break;
    case NMI_N:
      lrt_printf("NMI MADT not yet implemented\n");
      LRT_Assert(0);
      break;
    case LOCAL_APIC_NMI_N:
      {
	local_apic_nmi_structure *s = (local_apic_nmi_structure *)ptr;
	size -= s->length;
	ptr += s->length;
      }
      break;
    case LOCAL_APIC_ADDRESS_OVERRIDE_N:
      lrt_printf("LOCAL_APIC_ADDRESS_OVERRIDE MADT not yet implemented\n");
      LRT_Assert(0);
      break;
    case IO_SAPIC_N:
      lrt_printf("IO_SAPIC MADT not yet implemented\n");
      LRT_Assert(0);
      break;
    case LOCAL_SAPC_N:
      lrt_printf("LOCAL_SAPC MADT not yet implemented\n");
      LRT_Assert(0);
      break;
    case PLATFORM_INTERRUPT_SOURCES_N:
      lrt_printf("PLATFORM_INTERRUPT_SOURCES MADT not yet implemented\n");
      LRT_Assert(0);
      break;
    case PLATFORM_LOCAL_X2APIC_N:
      lrt_printf("PLATFORM_LOCAL_X2APIC MADT not yet implemented\n");
      LRT_Assert(0);
      break;
    case LOCAL_X2APIC_NMI_N:
      lrt_printf("LOCAL_X2APIC_NMI MADT not yet implemented\n");
      LRT_Assert(0);
      break;
    case GIC_N:
      lrt_printf("GIC MADT not yet implemented\n");
      LRT_Assert(0);
      break;
    case GICD_N:
      lrt_printf("GICD MADT not yet implemented\n");
      LRT_Assert(0);
      break;
    default:
      //No definitions for other structures yet!
      lrt_printf("Found MADT struct unimplimented that we don't know about???:"
		 " %d\n", *ptr);
      LRT_Assert(0);
    }
  } while (size > 0);
  LRT_Assert(ioapics == 1);
}
