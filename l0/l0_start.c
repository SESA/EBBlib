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
#include <lrt/io.h>
#include <l0/lrt/pic.h>
#include <lrt/assert.h>
#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/MemMgr.h> 
#include <l0/MemMgrPrim.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>

extern void trans_init(void);

void
EBB_init()
{
  /* Three main EBB's are EBBMgrPrim, EventMgrPrim EBBMemMgrPrim    */
  /* There creation and initialization are interdependent and requires */
  /* fancy footwork */
  EBBRC rc;

  rc = EBBMemMgrPrimInit();
  EBBRCAssert(rc);

  EBBMgrPrimInit();

  rc = EventMgrPrimImpInit();
  EBBRCAssert(rc);

  // then invoke a method of BootInfo object on first message
  // this object should gather boot information (sysfacts and boot args)
  // and then get full blown primitive l0 EBBS up (perhaps by a hot swap)
  EBB_LRT_printf("%s: ADD REST OF INIT CODE HERE!\n", __func__);
  LRT_EBBAssert(0);
}

void
l0_start(void)
{
  EBB_LRT_printf("%s: started!\n", __func__);
  trans_init();
  EBB_init();
}

