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

#include <config.h>

#include <stdint.h>

#include <arch/amd64/apic.h>
#include <arch/amd64/cpu.h>
#include <arch/amd64/multiboot.h>
#include <l0/lrt/event.h>
#include <l0/lrt/mem.h>
#include <l0/lrt/bare/stdio.h>
#include <l0/lrt/bare/arch/amd64/acpi.h>
#include <l0/lrt/bare/arch/amd64/lrt_start.h>
#include <l0/lrt/bare/arch/amd64/serial.h>
#include <l0/lrt/bare/arch/amd64/smp.h>

FILE com1;

static inline void
clear_bss(void)
{
  extern uint8_t sbss[];
  extern uint8_t ebss[];
  for (uint8_t *i = sbss; i < ebss; i++) {
    *i = 0;
  }
}

multiboot_info_t *bootinfo;

char *_smp_stack;

static void
init_smp(void)
{
  _smp_stack = lrt_mem_alloc(SMP_STACK_SIZE, 16, lrt_event_bsp_loc());
  extern char start_smp[];
  extern char end_smp[];

  for (int i = 0; i < (end_smp - start_smp); i++) {
    ((char *)SMP_START_ADDRESS)[i] = start_smp[i];
  }

  int cores = lrt_num_event_loc();
  for (int i = 0; i < cores; i++) {
    if (i == lrt_event_bsp_loc()) {
      continue;
    }

    while (!__sync_bool_compare_and_swap(&smp_lock, 0, 1))
      ;

    lapic_icr_low icr_low;
    icr_low.raw = 0;
    icr_low.delivery_mode = 0x5;

    lapic_icr_high icr_high;
    icr_high.raw = 0;
    icr_high.destination = i;

    send_ipi(icr_low, icr_high);

    uint64_t time = rdtsc();
    while ((rdtsc() - time) < 1000000)
      ;

    icr_low.vector = ((uintptr_t)SMP_START_ADDRESS) >> 12;
    icr_low.delivery_mode = 0x6;

    send_ipi(icr_low, icr_high);
  }
}

void __attribute__ ((noreturn))
init64(multiboot_info_t *mbi) {
  /* Zero out these segment selectors so we dont have issues later */
  __asm__ volatile (
                    "mov %w[zero], %%ds\n\t"
                    "mov %w[zero], %%es\n\t"
                    "mov %w[zero], %%ss\n\t"
                    "mov %w[zero], %%gs\n\t"
                    "mov %w[zero], %%fs\n\t"
                    :
                    :
                    [zero] "r" (0x0)
                    );

  clear_bss();

  bootinfo = mbi;

  //Initialize ctors
  extern char start_ctors[];
  extern char end_ctors[];
  for (void (*ctor) (void) = (void (*) (void))start_ctors;
       ctor < (void (*) (void))end_ctors;
       ctor++) {
    ctor();
  }

  /* serial init */
  serial_init(COM1, &com1);
  stdout = &com1;
  printf("Serial initialized\n");

  /* //get start args */
  acpi_init();
  int bsp = acpi_get_bsp();
  lrt_event_set_bsp(bsp);
  int cores = acpi_get_num_cores();
  lrt_printf("num cores = %d\n", cores);
  lrt_mem_preinit(cores);
  lrt_event_preinit(cores);
  lrt_trans_preinit(cores);

  init_smp();

  lrt_event_init(NULL); //unused parameter

  LRT_Assert(0);
}
