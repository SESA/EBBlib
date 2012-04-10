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

#include <l0/lrt/bare/arch/ppc32/bic.h>
#include <l0/lrt/bare/arch/ppc32/mem.h>
#include <l0/lrt/bare/arch/ppc32/trans.h>
#include <l0/sys/trans.h>
#include <lrt/io.h>

struct testObj;

struct testObjft {
  void (*foo)(struct testObj *self);
};

struct testObj {
  struct testObjft *ft;
};

EBBRC
testMissFunc (EBBRep **rep, EBBLTrans *lt, FuncNum fnum, EBBMissArg arg)
{
  EBB_LRT_printf("Got miss!\n");
  while(1) ;
}

extern EBBId TransEBBIdAlloc(void);
extern void TransEBBIdBind(EBBId id, EBBMissFunc mf, EBBMissArg arg);

void
lrt_start(void)
{
  EBB_LRT_printf("lrt_start called!\n");

  lrt_mem_init();
  lrt_trans_init();

  trans_init();

  EBB_LRT_printf("translation system initialized!\n");

  struct testObj **id = (struct testObj **)TransEBBIdAlloc();
  TransEBBIdBind((EBBId)id, testMissFunc, 0);

  EBBId_DREF(id)->ft->foo(EBBId_DREF(id));

  while(1) ;
}