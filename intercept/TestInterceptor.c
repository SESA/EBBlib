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

#include <config.h>

#include <arch/args.h>
#include <intercept/Interceptor.h>
#include <intercept/TestInterceptor.h>
#include <l0/EBBMgrPrim.h>
#include <l0/EventMgrPrim.h>
#include <l0/MemMgrPrim.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <lrt/io.h>

CObject(TestInterceptor) {
  CObjInterface(Interceptor) *ft;
  const char *str;
};

static EBBRC
TestInterceptor_PreCall(InterceptorRef _self, struct args *args,
                        EBBFuncNum fnum, union func_ret *fr)
{
  TestInterceptorRef self = (TestInterceptorRef)_self;
  lrt_printf("%s: on core %d, %s\n", __func__, MyEventLoc(), self->str);
  return EBBRC_OK;
}

static EBBRC
TestInterceptor_PostCall(InterceptorRef _self, EBBRC rc)
{
  TestInterceptorRef self = (TestInterceptorRef)_self;
  lrt_printf("%s: on core %d, %s\n", __func__, MyEventLoc(), self->str);
  return rc;
}

static CObjInterface(Interceptor) TestInterceptor_ftable = {
  .PreCall = TestInterceptor_PreCall,
  .PostCall = TestInterceptor_PostCall,
};

EBBRC
TestInterceptorCreate(InterceptorId *id, const char *str)
{
  TestInterceptorRef intRef;
  EBBRC rc = EBBPrimMalloc(sizeof(TestInterceptor), &intRef, EBB_MEM_GLOBAL);
  LRT_RCAssert(rc);
  intRef->ft = &TestInterceptor_ftable;
  intRef->str = str;

  CObjEBBRootSharedRef rootRef;
  rc = CObjEBBRootSharedCreate(&rootRef, (EBBRepRef)intRef);
  LRT_RCAssert(rc);

  rc = EBBAllocPrimId((EBBId *)id);
  LRT_RCAssert(rc);

  rc = CObjEBBBind(*(EBBId *)id, rootRef);
  LRT_RCAssert(rc);
  return EBBRC_OK;
}
