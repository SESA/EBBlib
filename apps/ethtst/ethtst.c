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
#include <net/EthTypeMgr.h>
#include <net/EthMgr.h>
#include <net/EthMgrPrim.h>
#include <l1/App.h>
#include <l1/startinfo.h>

EthMgrId theEthMgr;

CObject(EthTst) {
  CObjInterface(App) *ft;
};

EBBRC 
EthTst_start(AppRef _self)
{
  EBBRC rc;
  struct startinfo si;

  si_get_args(&si);

  if (si.argc <= 1) {
    lrt_printf("usage: ethtst nic\n");
    lrt_printf("  e.g. ethtst lo0, or ethtst eth1\n");
    LRT_RCAssert(-1);
  }

  lrt_printf("%s: START with device %s\n", __func__, si.argv[1]);
  //FIXME: check argument, pass in as first argument to run
  rc = EthMgrPrimCreate(&theEthMgr, si.argv[1]);
  LRT_RCAssert(rc);

  lrt_printf("%s: END\n", __func__);
  return EBBRC_OK;
}

CObjInterface(App) EthTst_ftable = {
  .start = EthTst_start
};

APP(EthTst, APP_START_ALL);
