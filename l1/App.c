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
#include <inttypes.h>
#include <lrt/io.h>
#include <lrt/assert.h>
#include <l0/cobj/cobj.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EBBMgrPrim.h>
#include <l0/EventMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l1/App.h>

AppId theAppId=0;

EBBRC app_start(void)
{
  EBBRC rc;
  if ((app_start_model == APP_START_ONE) && (MyEL() != 0)) {
    lrt_printf("EBBOS: secondary processor %ld bailing out to event loop\n", 
	       MyEL());
    return EBBRC_OK;
  }

  lrt_printf("EBBOS: primary processor %ld continuing\n", MyEL());
  if (__sync_bool_compare_and_swap(&theAppId, (AppId)0,
				   (AppId)-1)) {  
    EBBId id;
    CObjEBBRootMultiImpRef appRoot;
    // create App instance and invoke its start
    rc = CObjEBBRootMultiImpCreate(&appRoot, App_createRep);
    LRT_RCAssert(rc);
    rc = EBBAllocPrimId(&id);
    LRT_RCAssert(rc);
    rc = CObjEBBBind(id, appRoot); 
    LRT_RCAssert(rc);
    theAppId = (AppId)id;
  } else {
    while (((volatile uintptr_t)theAppId)==-1);
  }

  // WE ARE NOW DONE WITH L1 INITIALIZATION : 
  //    We now  hand-over the start up msg to the appliation
  //    From this point on everything should be messages/events that are handled
  //    by appliation level Ebb's

  return COBJ_EBBCALL(theAppId, start);
}
