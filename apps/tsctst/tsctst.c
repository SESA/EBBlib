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
#include <l0/EventMgrPrim.h>
#include <l1/App.h>
#include <sync/barrier.h>

CObject(TscTest) {
  CObjInterface(App) *ft;
};

static inline uint64_t
rdtsc_diff(void)
{
  uint32_t t0_lo, t0_hi, t1_lo, t1_hi;
  asm volatile ("rdtsc\n\t"
                "mov %%eax, %[t0_lo]\n\t"
                "mov %%edx, %[t0_hi]\n\t"
                "rdtsc"
                : [t0_lo] "=r" (t0_lo),
                  [t0_hi] "=r" (t0_hi),
                  "=a" (t1_lo),
                  "=d" (t1_hi));
  uint64_t t0 = (uint64_t)t0_hi << 32 | t0_lo;
  uint64_t t1 = (uint64_t)t1_hi << 32 | t1_lo;
  return t1 - t0;
}

static inline uint64_t
rdtscp_diff(void)
{
  uint32_t t0_lo, t0_hi, t1_lo, t1_hi;
  asm volatile ("rdtscp\n\t"
                "mov %%eax, %[t0_lo]\n\t"
                "mov %%edx, %[t0_hi]\n\t"
                "cpuid\n\t"
                "rdtscp"
                : [t0_lo] "=r" (t0_lo),
                  [t0_hi] "=r" (t0_hi),
                  "=a" (t1_lo),
                  "=d" (t1_hi)
                : "a" (0x80000001)
                : "ebx", "ecx");
  uint64_t t0 = (uint64_t)t0_hi << 32 | t0_lo;
  uint64_t t1 = (uint64_t)t1_hi << 32 | t1_lo;
  return t1 - t0;
}

volatile int cores = 0;
struct barrier_s bar;

static EBBRC
TscTest_start(AppRef _self)
{
  uint32_t eax, ebx, ecx, edx;
  cpuid(0x80000001, &eax, &ebx, &ecx, &edx);
  LRT_Assert(edx & (1 << 27));
#define NUM_TIMESTAMPS (1000)
  uint64_t *tsc_table;
  EBBRC rc = EBBPrimMalloc(sizeof(uint64_t) * NUM_TIMESTAMPS,
                           &tsc_table, EBB_MEM_DEFAULT);
  LRT_RCAssert(rc);
  int sense = 0;			/* sense of thread for barrier */
  int master = 0;
  // Wait for a while so all the other cores are back to their halt point
  uint64_t time = rdtsc();
  while ((rdtsc() - time) < 1000000)
    ;

  if (MyEventLoc() == 0) {
    master = 1;
    //init_barrier(&bar, NumEventLoc());
    init_barrier(&bar, 1);

    // last thing; initialize cores will unblock everyone else
    //cores = NumEventLoc();
    cores = 1;
  } else {
    // everyone but first core blocks here
    while (*(volatile int *)&cores == 0){}
  }

  barrier(&bar, &sense);

  if (master) {
    __sync_synchronize();
    for (int i = 0; i < NUM_TIMESTAMPS; i++) {
      tsc_table[i] = rdtsc_diff();
    }
    __sync_synchronize();
  }

  barrier(&bar, &sense);

  if (master) {
    lrt_printf("rdtsc single core: start\n");
    for (int i = 0; i < NUM_TIMESTAMPS; i++) {
      lrt_printf("%ld ", tsc_table[i]);
      if (i % 10 == 9) {
        lrt_printf("\n");
      }
    }
    lrt_printf("rdtsc single core: end\n");
  }

  barrier(&bar, &sense);

  if (master) {
    __sync_synchronize();
    for (int i = 0; i < NUM_TIMESTAMPS; i++) {
      tsc_table[i] = rdtscp_diff();
    }
    __sync_synchronize();
  }

  barrier(&bar, &sense);

  if (master) {
    lrt_printf("rdtscp single core: start\n");
    for (int i = 0; i < NUM_TIMESTAMPS; i++) {
      lrt_printf("%ld ", tsc_table[i]);
      if (i % 10 == 9) {
        lrt_printf("\n");
      }
    }
  lrt_printf("rdtscp single core: end\n");
  }

  return EBBRC_OK;
}


CObjInterface(App) TscTest_ftable = {
  .start = TscTest_start
};

APP(TscTest, APP_START_ONE);
