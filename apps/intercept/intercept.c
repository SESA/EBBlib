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

#include <intercept/Interceptor.h>
#include <intercept/TestInterceptor.h>
#include <arch/args.h>
#include <arch/amd64/cpu.h>
#include <l0/EventMgrPrim.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l1/App.h>
#include <lrt/io.h>
#include <lrt/exit.h>

COBJ_EBBType(Target) {
  EBBRC (*func) (TargetRef);
};

static EBBRC
Target_func(TargetRef ref)
{
  return EBBRC_OK;
}

static CObjInterface(Target) Target_ftable = {
  .func = Target_func
};

CObject(Intercept) {
  CObjInterface(Intercept) *ft;
};

CObjInterface(Intercept) {
  CObjImplements(App);
  EBBRC (*work) (InterceptRef self);
};

static TargetId target;
static EventNo evnum;

EBBRC
Intercept_start(AppRef _self)
{
  InterceptRef self = (InterceptRef)_self;
  TargetRef targRef;
  EBBRC rc = EBBPrimMalloc(sizeof(Target), &targRef, EBB_MEM_GLOBAL);
  LRT_RCAssert(rc);
  targRef->ft = &Target_ftable;

  rc = EBBAllocPrimId((EBBId *)&target);
  LRT_RCAssert(rc);

  CObjEBBRootSharedRef targRootRef;
  rc = CObjEBBRootSharedCreate(&targRootRef, (EBBRepRef)targRef);
  LRT_RCAssert(rc);

  rc = CObjEBBBind((EBBId)target, targRootRef);
  LRT_RCAssert(rc);
  //Target initialized

  rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &evnum);
  LRT_RCAssert(rc);

  rc = COBJ_EBBCALL(theEventMgrPrimId, bindEvent, evnum,
                    (EBBId)theAppId, COBJ_FUNCNUM(self, work));
  LRT_RCAssert(rc);

  for(EventNo num = NextEventLoc(MyEventLoc());
      num != MyEventLoc();
      num = NextEventLoc(num)) {
      rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, evnum,
                        EVENT_LOC_SINGLE, num);
      LRT_RCAssert(rc);
  }

  //Ok now all cores are invoking the target repeatedly

  InterceptorId id0;
  rc = TestInterceptorCreate(&id0, "Interceptor 0");
  LRT_RCAssert(rc);

  InterceptorControllerId controllerId0;
  rc = EBBAllocPrimId((EBBId *)&controllerId0);
  LRT_RCAssert(rc);

  InterceptorControllerImp_Create(controllerId0);
  LRT_RCAssert(rc);

  rc = COBJ_EBBCALL(controllerId0, start, (EBBId)target, id0);
  LRT_RCAssert(rc);

  uint64_t time = rdtsc();
  while ((rdtsc() - time) < 1000000)
    ;

  rc = COBJ_EBBCALL(controllerId0, stop);
  LRT_RCAssert(rc);

  rc = COBJ_EBBCALL(controllerId0, start, (EBBId)target, id0);
  LRT_RCAssert(rc);

  time = rdtsc();
  while ((rdtsc() - time) < 1000000)
    ;

  InterceptorId id1;
  rc = TestInterceptorCreate(&id1, "Interceptor 1");
  LRT_RCAssert(rc);
  InterceptorControllerId controllerId1;
  rc = EBBAllocPrimId((EBBId *)&controllerId1);
  LRT_RCAssert(rc);

  InterceptorControllerImp_Create(controllerId1);
  LRT_RCAssert(rc);

  rc = COBJ_EBBCALL(controllerId1, start, (EBBId)target, id1);
  LRT_RCAssert(rc);

  time = rdtsc();
  while ((rdtsc() - time) < 1000000)
    ;

  rc = COBJ_EBBCALL(controllerId0, stop);
  LRT_RCAssert(rc);

  time = rdtsc();
  while ((rdtsc() - time) < 1000000)
    ;

  /* rc = COBJ_EBBCALL(controllerId0, destroy); */
  /* LRT_RCAssert(rc); */

  rc = COBJ_EBBCALL(controllerId1, stop);
  LRT_RCAssert(rc);

  /* rc = COBJ_EBBCALL(controllerId1, destroy); */
  /* LRT_RCAssert(rc); */

  time = rdtsc();
  while ((rdtsc() - time) < 1000000)
    ;

  lrt_exit(0);
  return EBBRC_OK;
}

EBBRC
Intercept_work(InterceptRef self)
{
  COBJ_EBBCALL(target, func);
  EBBRC rc = COBJ_EBBCALL(theEventMgrPrimId, triggerEvent, evnum,
                    EVENT_LOC_SINGLE, MyEventLoc());
  LRT_RCAssert(rc);
  return EBBRC_OK;
}

CObjInterface(Intercept) Intercept_ftable = {
  {.start = Intercept_start},
  .work = Intercept_work
};

APP_START_ONE(Intercept);
