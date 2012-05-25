/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2012, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights.  You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code.  No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision.  In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change.  Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee.  Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution.  In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE.  ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT,  ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES.  INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS.  INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.  THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government.  In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/


/*
 * These interfaces are required in order to compile the ASL compiler and the
 * various ACPICA tools under Linux or other Unix-like system.
 *
 * Note: Use #define __APPLE__ for OS X generation.
 */

#include <config.h>

#include <stdint.h>

#include <lrt/assert.h>
#include <lrt/io.h>

#include "acpi.h"

/******************************************************************************
 *
 * FUNCTION:    AcpiOsInitialize, AcpiOsTerminate
 *
 * PARAMETERS:  None
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Init and terminate. Nothing to do.
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsInitialize (void)
{
  LRT_Assert(0);
  return (AE_OK);
}


ACPI_STATUS
AcpiOsTerminate (void)
{
  LRT_Assert(0);
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsGetRootPointer
 *
 * PARAMETERS:  None
 *
 * RETURN:      RSDP physical address
 *
 * DESCRIPTION: Gets the ACPI root pointer (RSDP)
 *
 *****************************************************************************/

ACPI_PHYSICAL_ADDRESS
AcpiOsGetRootPointer (void)
{
  ACPI_STATUS status;
  ACPI_SIZE addr;
  
  status = AcpiFindRootPointer(&addr);

  LRT_Assert(status == AE_OK);
  
  return addr;
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsPredefinedOverride
 *
 * PARAMETERS:  InitVal             - Initial value of the predefined object
 *              NewVal              - The new value for the object
 *
 * RETURN:      Status, pointer to value. Null pointer returned if not
 *              overriding.
 *
 * DESCRIPTION: Allow the OS to override predefined names
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsPredefinedOverride (const ACPI_PREDEFINED_NAMES *InitVal, 
			  ACPI_STRING *NewVal)
{
  LRT_Assert(0);
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsTableOverride
 *
 * PARAMETERS:  ExistingTable       - Header of current table (probably
 *                                    firmware)
 *              NewTable            - Where an entire new table is returned.
 *
 * RETURN:      Status, pointer to new table. Null pointer returned if no
 *              table is available to override
 *
 * DESCRIPTION: Return a different version of a table if one is available
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsTableOverride (ACPI_TABLE_HEADER *ExistingTable,
		     ACPI_TABLE_HEADER **NewTable)
{
  *NewTable = NULL;
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsPhysicalTableOverride
 *
 * PARAMETERS:  ExistingTable       - Header of current table (probably firmware)
 *              NewAddress          - Where new table address is returned
 *                                    (Physical address)
 *              NewTableLength      - Where new table length is returned
 *
 * RETURN:      Status, address/length of new table. Null pointer returned
 *              if no table is available to override.
 *
 * DESCRIPTION: Returns AE_SUPPORT, function not used in user space.
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsPhysicalTableOverride (ACPI_TABLE_HEADER *ExistingTable,
			     ACPI_PHYSICAL_ADDRESS *NewAddress,
			     UINT32 *NewTableLength)
{
  NewAddress = NULL;
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsRedirectOutput
 *
 * PARAMETERS:  Destination         - An open file handle/pointer
 *
 * RETURN:      None
 *
 * DESCRIPTION: Causes redirect of AcpiOsPrintf and AcpiOsVprintf
 *
 *****************************************************************************/

void
AcpiOsRedirectOutput (void *Destination)
{
  LRT_Assert(0);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsPrintf
 *
 * PARAMETERS:  fmt, ...            - Standard printf format
 *
 * RETURN:      None
 *
 * DESCRIPTION: Formatted output
 *
 *****************************************************************************/

void ACPI_INTERNAL_VAR_XFACE
AcpiOsPrintf (const char *Fmt, ...)
{
  va_list args;

  va_start(args, Fmt);
  AcpiOsVprintf(Fmt, args);
  va_end(args);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsVprintf
 *
 * PARAMETERS:  fmt                 - Standard printf format
 *              args                - Argument list
 *
 * RETURN:      None
 *
 * DESCRIPTION: Formatted output with argument list pointer
 *
 *****************************************************************************/

void
AcpiOsVprintf (const char *Fmt, va_list Args)
{
  lrt_vprintf(Fmt, Args);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsGetLine
 *
 * PARAMETERS:  Buffer              - Where to return the command line
 *              BufferLength        - Maximum length of Buffer
 *              BytesRead           - Where the actual byte count is returned
 *
 * RETURN:      Status and actual bytes read
 *
 * DESCRIPTION: Formatted input with argument list pointer
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsGetLine (char *Buffer, UINT32 BufferLength, UINT32 *BytesRead)
{
  LRT_Assert(0);
  return (AE_OK);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsMapMemory
 *
 * PARAMETERS:  where               - Physical address of memory to be mapped
 *              length              - How much memory to map
 *
 * RETURN:      Pointer to mapped memory. Null on error.
 *
 * DESCRIPTION: Map physical memory into caller's address space
 *
 *****************************************************************************/

void *
AcpiOsMapMemory (ACPI_PHYSICAL_ADDRESS where, ACPI_SIZE length)
{
  //make sure that it is in the lower 1g of memory
  if (((uintptr_t)where) + ((uintptr_t)length) > 0x3FFFFFFF) {
    return NULL;
  }

  return (void *)where; //idempotent mapping
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsUnmapMemory
 *
 * PARAMETERS:  where               - Logical address of memory to be unmapped
 *              length              - How much memory to unmap
 *
 * RETURN:      None.
 *
 * DESCRIPTION: Delete a previously created mapping. Where and Length must
 *              correspond to a previous mapping exactly.
 *
 *****************************************************************************/

void
AcpiOsUnmapMemory (void *where, ACPI_SIZE length)
{
  return;
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsAllocate
 *
 * PARAMETERS:  Size                - Amount to allocate, in bytes
 *
 * RETURN:      Pointer to the new allocation. Null on error.
 *
 * DESCRIPTION: Allocate memory. Algorithm is dependent on the OS.
 *
 *****************************************************************************/

void *
AcpiOsAllocate (ACPI_SIZE size)
{
  LRT_Assert(0);
  return NULL;
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsFree
 *
 * PARAMETERS:  mem                 - Pointer to previously allocated memory
 *
 * RETURN:      None.
 *
 * DESCRIPTION: Free memory allocated via AcpiOsAllocate
 *
 *****************************************************************************/

void
AcpiOsFree (void *mem)
{
  LRT_Assert(0);
}


/******************************************************************************
 *
 * FUNCTION:    Semaphore stub functions
 *
 * DESCRIPTION: Stub functions used for single-thread applications that do
 *              not require semaphore synchronization. Full implementations
 *              of these functions appear after the stubs.
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsCreateSemaphore (UINT32 MaxUnits, UINT32 InitialUnits,
		       ACPI_HANDLE *OutHandle)
{
  LRT_Assert(0);
  return (AE_OK);
}

ACPI_STATUS
AcpiOsDeleteSemaphore (ACPI_HANDLE Handle)
{
  LRT_Assert(0);
  return (AE_OK);
}

ACPI_STATUS
AcpiOsWaitSemaphore (ACPI_HANDLE Handle, UINT32 Units, UINT16 Timeout)
{
  LRT_Assert(0);
  return (AE_OK);
}

ACPI_STATUS
AcpiOsSignalSemaphore (ACPI_HANDLE Handle, UINT32 Units)
{
  LRT_Assert(0);
  return (AE_OK);
}

/******************************************************************************
 *
 * FUNCTION:    Spinlock interfaces
 *
 * DESCRIPTION: Map these interfaces to semaphore interfaces
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsCreateLock (ACPI_SPINLOCK *OutHandle)
{
  LRT_Assert(0);
  return (AE_OK);
}

void
AcpiOsDeleteLock (ACPI_SPINLOCK Handle)
{
  LRT_Assert(0);
}

ACPI_CPU_FLAGS
AcpiOsAcquireLock (ACPI_HANDLE Handle)
{
  LRT_Assert(0);
  return (0);
}

void
AcpiOsReleaseLock (ACPI_SPINLOCK Handle, ACPI_CPU_FLAGS Flags)
{
  LRT_Assert(0);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsInstallInterruptHandler
 *
 * PARAMETERS:  InterruptNumber     - Level handler should respond to.
 *              Isr                 - Address of the ACPI interrupt handler
 *              ExceptPtr           - Where status is returned
 *
 * RETURN:      Handle to the newly installed handler.
 *
 * DESCRIPTION: Install an interrupt handler. Used to install the ACPI
 *              OS-independent handler.
 *
 *****************************************************************************/

UINT32
AcpiOsInstallInterruptHandler (UINT32 InterruptNumber, 
			       ACPI_OSD_HANDLER ServiceRoutine, void *Context)
{
  LRT_Assert(0);
  return (0);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsRemoveInterruptHandler
 *
 * PARAMETERS:  Handle              - Returned when handler was installed
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Uninstalls an interrupt handler.
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsRemoveInterruptHandler (UINT32 InterruptNumber, 
			      ACPI_OSD_HANDLER ServiceRoutine)
{
  LRT_Assert(0);
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsStall
 *
 * PARAMETERS:  microseconds        - Time to sleep
 *
 * RETURN:      Blocks until sleep is completed.
 *
 * DESCRIPTION: Sleep at microsecond granularity
 *
 *****************************************************************************/

void
AcpiOsStall (UINT32 microseconds)
{
  LRT_Assert(0);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsSleep
 *
 * PARAMETERS:  milliseconds        - Time to sleep
 *
 * RETURN:      Blocks until sleep is completed.
 *
 * DESCRIPTION: Sleep at millisecond granularity
 *
 *****************************************************************************/

void
AcpiOsSleep (UINT64 milliseconds)
{
  LRT_Assert(0);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsGetTimer
 *
 * PARAMETERS:  None
 *
 * RETURN:      Current time in 100 nanosecond units
 *
 * DESCRIPTION: Get the current system time
 *
 *****************************************************************************/

UINT64
AcpiOsGetTimer (void)
{
  LRT_Assert(0);
  return 0;
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsReadPciConfiguration
 *
 * PARAMETERS:  PciId               - Seg/Bus/Dev
 *              Register            - Device Register
 *              Value               - Buffer where value is placed
 *              Width               - Number of bits
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Read data from PCI configuration space
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsReadPciConfiguration (ACPI_PCI_ID *PciId, UINT32 Register,
			    UINT64 *Value, UINT32 Width)
{
  LRT_Assert(0);
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsWritePciConfiguration
 *
 * PARAMETERS:  PciId               - Seg/Bus/Dev
 *              Register            - Device Register
 *              Value               - Value to be written
 *              Width               - Number of bits
 *
 * RETURN:      Status.
 *
 * DESCRIPTION: Write data to PCI configuration space
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsWritePciConfiguration (ACPI_PCI_ID *PciId, UINT32 Register,
			     UINT64 Value, UINT32 Width)
{
  LRT_Assert(0);
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsReadPort
 *
 * PARAMETERS:  Address             - Address of I/O port/register to read
 *              Value               - Where value is placed
 *              Width               - Number of bits
 *
 * RETURN:      Value read from port
 *
 * DESCRIPTION: Read data from an I/O port or register
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsReadPort (ACPI_IO_ADDRESS Address, UINT32 *Value, UINT32 Width)
{
  LRT_Assert(0);
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsWritePort
 *
 * PARAMETERS:  Address             - Address of I/O port/register to write
 *              Value               - Value to write
 *              Width               - Number of bits
 *
 * RETURN:      None
 *
 * DESCRIPTION: Write data to an I/O port or register
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsWritePort (ACPI_IO_ADDRESS Address, UINT32 Value, UINT32 Width)
{
  LRT_Assert(0);
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsReadMemory
 *
 * PARAMETERS:  Address             - Physical Memory Address to read
 *              Value               - Where value is placed
 *              Width               - Number of bits (8,16,32, or 64)
 *
 * RETURN:      Value read from physical memory address. Always returned
 *              as a 64-bit integer, regardless of the read width.
 *
 * DESCRIPTION: Read data from a physical memory address
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsReadMemory (ACPI_PHYSICAL_ADDRESS Address, UINT64 *Value, UINT32 Width)
{
  LRT_Assert(0);
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsWriteMemory
 *
 * PARAMETERS:  Address             - Physical Memory Address to write
 *              Value               - Value to write
 *              Width               - Number of bits (8,16,32, or 64)
 *
 * RETURN:      None
 *
 * DESCRIPTION: Write data to a physical memory address
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsWriteMemory (ACPI_PHYSICAL_ADDRESS Address, UINT64 Value, UINT32 Width)
{
  LRT_Assert(0);
  return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsReadable
 *
 * PARAMETERS:  Pointer             - Area to be verified
 *              Length              - Size of area
 *
 * RETURN:      TRUE if readable for entire length
 *
 * DESCRIPTION: Verify that a pointer is valid for reading
 *
 *****************************************************************************/

BOOLEAN
AcpiOsReadable (void *Pointer, ACPI_SIZE Length)
{
  LRT_Assert(0);
  return (TRUE);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsWritable
 *
 * PARAMETERS:  Pointer             - Area to be verified
 *              Length              - Size of area
 *
 * RETURN:      TRUE if writable for entire length
 *
 * DESCRIPTION: Verify that a pointer is valid for writing
 *
 *****************************************************************************/

BOOLEAN
AcpiOsWritable (void *Pointer, ACPI_SIZE Length)
{
  LRT_Assert(0);
  return (TRUE);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsSignal
 *
 * PARAMETERS:  Function            - ACPI CA signal function code
 *              Info                - Pointer to function-dependent structure
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Miscellaneous functions. Example implementation only.
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsSignal (UINT32 Function, void *Info)
{
  LRT_Assert(0);
  return (AE_OK);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsGetThreadId
 *
 * PARAMETERS:  None
 *
 * RETURN:      Id of the running thread
 *
 * DESCRIPTION: Get the ID of the current (running) thread
 *
 *****************************************************************************/

ACPI_THREAD_ID
AcpiOsGetThreadId (void)
{
  LRT_Assert(0);
  return 0;
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsExecute
 *
 * PARAMETERS:  Type                - Type of execution
 *              Function            - Address of the function to execute
 *              Context             - Passed as a parameter to the function
 *
 * RETURN:      Status.
 *
 * DESCRIPTION: Execute a new thread
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsExecute (ACPI_EXECUTE_TYPE Type, ACPI_OSD_EXEC_CALLBACK Function, 
	       void *Context)
{
  LRT_Assert(0);
  return (AE_OK);
}
