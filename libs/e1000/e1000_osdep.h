/*******************************************************************************

  Intel PRO/1000 Linux driver
  Copyright(c) 1999 - 2010 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information:
  Linux NICS <linux.nics@intel.com>
  e1000-devel Mailing List <e1000-devel@lists.sourceforge.net>
  Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497

*******************************************************************************/


/* glue for the OS-dependent part of e1000
 * includes register access macros
 */

#ifndef _E1000_OSDEP_H_
#define _E1000_OSDEP_H_

/* #include <linux/pci.h> */
/* #include <linux/delay.h> */
/* #include <linux/interrupt.h> */
/* #include <linux/if_ether.h> */

/* #include "kcompat.h" */

#include <l4/types.h>
#include <l4/ipc.h>
#include <l4io.h>
#include <l4hal/types.h>
#include <arch/sysio.h>

#define dma_addr_t unsigned long
//FIXME: arch dependant types
#define __le16     unsigned short
#define __le32     unsigned int
#define __le64     unsigned long long
#define __iomem

typedef enum {
  FALSE = 0,
  TRUE = 1
} boolean_t;

#define usec_delay(x) do {int _i; for(_i = 0;_i < ((x)*1000); _i++);} while(0)
#define msec_delay(x)	L4_Sleep( L4_TimePeriod(((L4_Word64_t)x)*1000) )

/* Some workarounds require millisecond delays and are run during interrupt
 * context.  Most notably, when establishing link, the phy may need tweaking
 * but cannot process phy register reads/writes faster than millisecond
 * intervals...and we establish link due to a "link status change" interrupt.
 */
#define msec_delay_irq(x) msec_delay(x)

#define PCI_COMMAND_REGISTER   0x04
#define CMD_MEM_WRT_INVALIDATE 0x10
#define ETH_ADDR_LEN           6

#ifdef __BIG_ENDIAN
#define E1000_BIG_ENDIAN __BIG_ENDIAN
#endif

#define DBG 0
#if DBG
#define DEBUGOUT(S) printf(S "\n")
#define DEBUGOUT1(S, A...) printf(S "\n", A)
#else
#define DEBUGOUT(S)
#define DEBUGOUT1(S, A...)
#endif

#define DEBUGFUNC(F) DEBUGOUT(F "\n")
#define DEBUGOUT2 DEBUGOUT1
#define DEBUGOUT3 DEBUGOUT2
#define DEBUGOUT7 DEBUGOUT3

/* Mimic Linux io access stuff for the x86. */
#define __io_virt(x) ((void *)(x))
#define writel(b,addr) (*(volatile unsigned int *) __io_virt(addr) = (b))
#define readl(addr) (*(volatile unsigned int *) __io_virt(addr))

static inline void *memset(void *s, int c, unsigned int len) {
  unsigned char ch = (unsigned char)c;
  unsigned char *sch = (unsigned char *)s;
  while(len > 0) {
    *sch = ch;
    sch++;
    len--;
  }
  return s;
}

#define E1000_REGISTER(a, reg) (((a)->mac.type >= e1000_82543) \
                               ? reg                           \
                               : e1000_translate_register_82542(reg))

static inline u32 er32(u8 __iomem *reg)
{
        return readl(reg);
}

static inline void ew32(u8 __iomem *reg, u32 val)
{
        writel(val, reg);
}

#define E1000_WRITE_REG(a, reg, value) \
    ew32(((a)->hw_addr + E1000_REGISTER(a, reg)),value)

#define E1000_READ_REG(a, reg) (er32((a)->hw_addr + E1000_REGISTER(a, reg)))

#define E1000_WRITE_REG_ARRAY(a, reg, offset, value) ( \
    writel((value), ((a)->hw_addr + E1000_REGISTER(a, reg) + ((offset) << 2))))

#define E1000_READ_REG_ARRAY(a, reg, offset) ( \
    readl((a)->hw_addr + E1000_REGISTER(a, reg) + ((offset) << 2)))

#define E1000_READ_REG_ARRAY_DWORD E1000_READ_REG_ARRAY
#define E1000_WRITE_REG_ARRAY_DWORD E1000_WRITE_REG_ARRAY

#define E1000_WRITE_REG_ARRAY_WORD(a, reg, offset, value) ( \
    writew((value), ((a)->hw_addr + E1000_REGISTER(a, reg) + ((offset) << 1))))

#define E1000_READ_REG_ARRAY_WORD(a, reg, offset) ( \
    readw((a)->hw_addr + E1000_REGISTER(a, reg) + ((offset) << 1)))

#define E1000_WRITE_REG_ARRAY_BYTE(a, reg, offset, value) ( \
    writeb((value), ((a)->hw_addr + E1000_REGISTER(a, reg) + (offset))))

#define E1000_READ_REG_ARRAY_BYTE(a, reg, offset) ( \
    readb((a)->hw_addr + E1000_REGISTER(a, reg) + (offset)))

#define E1000_WRITE_REG_IO(a, reg, offset) do { \
    sysOut32(((a)->io_base), reg);					\
    sysOut32(((a)->io_base + 4), offset);      } while(0)

#define E1000_WRITE_FLUSH(a) E1000_READ_REG(a, E1000_STATUS)

#define E1000_WRITE_FLASH_REG(a, reg, value) ( \
    writel((value), ((a)->flash_address + reg)))

#define E1000_WRITE_FLASH_REG16(a, reg, value) ( \
    writew((value), ((a)->flash_address + reg)))

#define E1000_READ_FLASH_REG(a, reg) (readl((a)->flash_address + reg))

#define E1000_READ_FLASH_REG16(a, reg) (readw((a)->flash_address + reg))

#endif /* _E1000_OSDEP_H_ */
