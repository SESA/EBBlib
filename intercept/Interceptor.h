#ifndef INTERCEPT_INTERCEPTOR_H
#define INTERCEPT_INTERCEPTOR_H

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
#include <l0/EBBMgrPrim.h>

union func_ret {
  EBBFunc func;
  EBBRC ret;
};

COBJ_EBBType(Interceptor) {
  EBBRC (*PreCall) (InterceptorRef self, struct args *args,
                    EBBFuncNum fnum, union func_ret *fr);
  EBBRC (*PostCall) (InterceptorRef self, EBBRC rc);
};

COBJ_EBBType(InterceptorController) {
  EBBRC (*start) (InterceptorControllerRef self, EBBId target_id,
                  InterceptorId interceptor_id);
  EBBRC (*stop) (InterceptorControllerRef self);
  EBBRC (*destroy) (InterceptorControllerRef self);
};

extern EBBRC InterceptorControllerImp_Create(InterceptorControllerId id);
#endif
