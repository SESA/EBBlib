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

#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <lrt/io.h>
#include <l0/lrt/trans.h>
#include <lrt/exit.h>
#include <lrt/assert.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/App.h>
#include <l1/startinfo.h>
#include <sync/barrier.h>

CObject(HelloWorld_Parallel) {
  CObjInterface(App) *ft;
};

EBBRC
HelloWorld_Parallel_start(AppRef _self)
{
  static volatile int lock;
  while (!__sync_bool_compare_and_swap(&lock, 0, 1))
    ;
  lrt_printf("Hello world from event location %d\n",
             MyEventLoc());
  lock = 0;

  static volatile int barrier_init;
  static struct barrier_s bar;
  if (__sync_bool_compare_and_swap(&barrier_init, 0, 1)) {
    init_barrier(&bar, NumEventLoc());
    barrier_init = 2;
  } else {
    while (barrier_init == 1)
      ;
  }
  int sense = 0;
  barrier(&bar, &sense);

  if (lrt_my_event_loc() == 0) {
    lrt_exit(0);
  }
  return EBBRC_OK;
}

CObjInterface(App) HelloWorld_Parallel_ftable = {
  .start = HelloWorld_Parallel_start
};

APP_START_ALL(HelloWorld_Parallel);
