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
#include <lrt/assert.h>
#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/MemMgr.h> 
#include <l0/MemMgrPrim.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l1/App.h>

extern void trans_init(void);

/* 
 * Three main EBB's are EBBMgrPrim, EventMgrPrim EBBMemMgrPrim    
 * There creation and initialization are interdependent and requires 
 * fancy footwork 
 */
void
EBB_init(uintptr_t startInfo)
{
  lrt_printf("EBB_init called!\n");
  EBBRC rc;

  rc = EBBMemMgrPrimInit();
  LRT_RCAssert(rc);

  rc = EBBMgrPrimInit();
  LRT_RCAssert(rc);

  // At this point EBBMgr and Ebb Calls should be working
  //  NOMORE USE OF TRANS OR BOOT INTERFACES TO THESE THINGS
  //  AND MOST CODE SHOULD BE ON Ebb's

  rc = EventMgrPrimImpInit();
  LRT_RCAssert(rc);

  // JA: FIXME:  IS THIS FIRST REAL EBB CALL BELOW ... SHOULD BE EXPLICITLY MARKED
  //             AND THE FACTS THAT THAT DEPENDS ON CLEARLY STATED

  if ((app_start_model == APP_START_ALL) || (MyEventLoc() == 0)) {
    app_start();
  }
  // will fall through
}

void
l0_start(uintptr_t startInfo)
{
  trans_init();
  EBB_init(startInfo);
}

