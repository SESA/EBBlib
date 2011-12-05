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
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>

EBBMgrPrimId theEBBMgrPrimId;

CObject(EBBMgrPrimImp) {
  CObjInterface(EBBMgrPrim) *ft;
  CObjEBBRootMultiRef theRoot;
};

static EBBRC
AllocId (EBBMgrPrimRef _self, EBBId *id) {
  *id = EBBIdAlloc();
  return EBBRC_OK;
}

static EBBRC
FreeId (EBBMgrPrimRef _self, EBBId id) {
  EBBIdFree(id);
  return EBBRC_OK;
}

static EBBRC
BindId (EBBMgrPrimRef _self, EBBId id, EBBMissFunc mf, EBBMissArg arg) {
  EBBIdBind(id, mf, arg);
  return EBBRC_OK;
}

static EBBRC
UnBindId (EBBMgrPrimRef _self, EBBId id, EBBMissFunc *mf, EBBMissArg *arg) {
  EBBIdUnBind(id, mf, arg);
  return EBBRC_OK;
}

static CObjInterface(EBBMgrPrim) EBBMgrPrimImp_ftable = {
  .AllocId = AllocId, 
  .FreeId = FreeId, 
  .BindId = BindId, 
  .UnBindId = UnBindId, 
};

#if 0
static EBBRC 
EBBMgrPrimErrMF (EBBRep **_self, EBBLTrans *lt,
		 FuncNum fnum, EBBMissArg arg) {
  EBB_LRT_printf("%s: _self=%p: lt=%p fnum=%p arg=%p", __func__, 
		 _self, lt, (void *)fnum, (void *)arg);
  return EBBRC_GENERIC_FAILURE;
}
#endif

static EBBRep *
EBBMgrPrimImp_createRep(CObjEBBRootMultiRef _self) {
  EBBMgrPrimImpRef repRef;
  EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT);
  repRef->ft = &EBBMgrPrimImp_ftable;
  repRef->theRoot = _self;
  //  initGTable(EBBMgrPrimErrMF, 0);
  return (EBBRep *)repRef;
}

void EBBMgrPrimInit() {
  CObjEBBRootMultiImpRef rootRef;

  if (__sync_bool_compare_and_swap(&theEBBMgrPrimId, (EBBMgrPrimId)0,
				   (EBBMgrPrimId)-1)) {
    CObjEBBRootMultiImpCreate(&rootRef, EBBMgrPrimImp_createRep);
    
    theEBBMgrPrimId = (EBBMgrPrimId)EBBIdAlloc();
    EBBAssert(theEBBMgrPrimId != NULL);
    
    EBBIdBind((EBBId)theEBBMgrPrimId, CObjEBBMissFunc, (EBBMissArg) rootRef);
  } else {
    while (((volatile uintptr_t)theEBBMemMgrPrimId)==-1);
  }
}
