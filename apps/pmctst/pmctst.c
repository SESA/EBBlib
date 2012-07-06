/*
 * Copyright (C) 2012 by Project SESA, Boston University
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
#include <arch/amd64/pmc.h>
#include <l0/EventMgrPrim.h>
#include <l1/App.h>
#include <sync/barrier.h>

CObject(PmcTest) {
  CObjInterface(App) *ft;
};

static inline
uint64_t rdpmc(int reg)
{
  uint32_t lo, hi;
  /* asm volatile ("xorl %%eax, %%eax\n\t" */
  /*               "cpuid" */
  /*               : */
  /*               : */
  /*               : "%rax", "%rbx", "%rcx", "%rdx"); */

  asm volatile ("rdpmc"
                : "=a" (lo),
                  "=d" (hi)
                : "c" (reg));

  /* asm volatile ("xorl %%eax, %%eax\n\t" */
  /*               "cpuid" */
  /*               : */
  /*               : */
  /*               : "%rax", "%rbx", "%rcx", "%rdx"); */

  return (uint64_t)hi << 32 | lo;
}

volatile int cores = 0;
struct barrier_s bar;

static EBBRC
PmcTest_start(AppRef _self)
{
#define NUM_TIMESTAMPS (1000)
  uint64_t *tsc_table;
  EBBRC rc = EBBPrimMalloc(sizeof(uint64_t) * NUM_TIMESTAMPS,
                           &tsc_table, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);
  int sense = 0;			/* sense of thread for barrier */
  int master = 0;

  uint64_t time = rdtsc();
  while ((rdtsc() - time) < 1000000)
    ;

  int num_cores = 1;
  //int num_cores = NumEventLoc();

  if (MyEventLoc() == 0) {
    master = 1;
    init_barrier(&bar, num_cores);

    // last thing; initialize cores will unblock everyone else
    cores = num_cores;
  } else {
    // everyone but first core blocks here
    while (*(volatile int *)&cores == 0){}
  }

  barrier(&bar, &sense);

  if (master) {
    perf_event_select event_sel;
    event_sel.val = 0;
    event_sel.eventselect_7_0 = PMC_CLOCKS_NOT_HALTED;
    event_sel.osusermode = 3; //count all events irrespective of cpl
    event_sel.en = 1;

    asm volatile ("wrmsr"
                  :
                  : "a" (event_sel.val & 0xFFFFFFFF),
                    "d" (event_sel.val >> 32),
                    "c" (0xC0010200));

    for (int i = 0; i < NUM_TIMESTAMPS; i++) {
      uint64_t t0, t1;
      t0 = rdpmc(0);
      for (int j = 0; j < 1000; j++) {
        asm volatile ("rdtsc"
                      :
                      :
                      : "%rax", "%rdx");
      }
      t1 = rdpmc(0);
      tsc_table[i] = t1 - t0;
    }
  }

  barrier(&bar, &sense);

  if (master) {
    for (int i = 0; i < NUM_TIMESTAMPS; i++) {
      lrt_printf("%ld ", tsc_table[i]);
      if (i % 10 == 9) {
        lrt_printf("\n");
      }
    }
  }

  return EBBRC_OK;
}


CObjInterface(App) PmcTest_ftable = {
  .start = PmcTest_start
};

APP(PmcTest, APP_START_ONE);
