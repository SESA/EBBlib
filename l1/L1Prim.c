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
#include <stddef.h>

#include <lrt/assert.h>
#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/EBBMgrPrimBoot.h>
#include <l0/EventMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l0/lrt/mem.h>
#include <l1/L1.h>
#include <l1/L1Prim.h>

CObject(L1Prim) {
  CObjInterface(L1) *ft;
  CObjEBBRootMultiRef myRoot;
  char *si;
  intptr_t sisize;
};

EBBRC
L1Prim_startMsg(void *_self, uintptr_t startinfo)
{
  self = _self;
  self->si = malloc(lrt_startinfo_size());
  memcpy(...);
  
  rc = AppInit(&id);
  
  COBJ_EBBCALL(id, start);
}

EBBRC
L1Prim_start(void *_self, uintptr_t startinfo)
{
  EBBRC rc;
  MsgHandlerId startMsgHdlr;

  // initialize the message handler, this will take over the
  // IPI on this core. 
  rc = MsgMgrPrim_Init();
  EBBRCAssert(rc);

  // continue startup for this EL as a message to myself here (on this EL)
  COBJ_EBBCALL(theMsgMgr, myEL(), theL1Id, startMsg, startinfo);

  EBBRCAssert(rc);
}

CObjInterface(L1) L1Prim_ftable = {
  .start = L1Prim_start
};
