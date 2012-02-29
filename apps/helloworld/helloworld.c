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
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <lrt/assert.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/lrt/pic.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/App.h>

CObject(HelloWorld) {
  CObjInterface(App) *ft;
};

EBBRC HelloWorld_start(void)
{
  EBB_LRT_printf("Hello world!\n");
  return EBBRC_OK;
}

CObjInterface(App) HelloWorld_ftable = {
  .start = HelloWorld_start;
};

EBBRC
AppInit(AppId *id)
{
  // JA: FIXME
  // This is concurrent non-ebb method code should we
  // expose this as the first thing an App developers writes ???
  // not sure I like it ... but on the other an EBB has to be created.
  // or maybe not we could pass in a EBBid that is bound to NULL and the view
  // start up as a hotswap to the developer defined instance.  More of a miss trigger
  // also want to consider this as a factor like startup ... developer defines
  // the App factory and we ask it to create and instance and then invoke start
  // which is what this is sort of like.... so the hybrid is 
  //   Alloc an id for the app factory (afid) that is bound to null
  //   Invoke load(afid); // this rebinds afid to developer specified app factory
  //                      // this implies that a loadable thing always rebinds the 
  //                      // passed in id ... races might be deferred to create        
  //   Invoke DREF(afid)->create(&appid);
  //   Invoke DREF(appid)->start(void);
  
  EBB_LRT_printf("%s: here we go....weee!\n");
  
  return EBBRC_OK;
}

