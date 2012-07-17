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
#include <l0/lrt/trans.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EBBMgrPrimBoot.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>


EBBMgrPrimId theEBBMgrPrimId=0;

CObject(EBBMgrPrimImp) {
  CObjInterface(EBBMgrPrim) *ft;
  CObjEBBRootMultiRef theRoot;
};

static EBBRC
AllocId (EBBMgrPrimRef _self, EBBId *id) {
  *id = lrt_trans_id_alloc();
  return EBBRC_OK;
}

static EBBRC
FreeId (EBBMgrPrimRef _self, EBBId id) {
  lrt_trans_id_free(id);
  return EBBRC_OK;
}

static EBBRC
BindId (EBBMgrPrimRef _self, EBBId id, EBBMissFunc *mf,
        EBBBindFunc *bf, EBBArg *arg, int force) {
  EBBMissFunc mftemp = *mf;
  EBBBindFunc bftemp = *bf;
  EBBArg argtemp = *arg;

  EBBBindFunc oldbf = (EBBBindFunc)lrt_trans_get_val(id);
  if (!force) {
    if (oldbf) {
      EBBArg oldarg = lrt_trans_get_arg(id);
      oldbf(&mftemp, &bftemp, &argtemp, oldarg);
    }
  }
  lrt_trans_id_bind(id, mf, arg);
  *bf = oldbf;
  lrt_trans_set_val(id, (uintptr_t)bftemp);
  return EBBRC_OK;
}

static EBBRC
UnBindId (EBBMgrPrimRef _self, EBBId id) {
  lrt_printf("%s: NYI: PLEASE FIXME!: NONE OF THIS WORKS\n",
             __func__);
  return EBBRC_OK;
}

static CObjInterface(EBBMgrPrim) EBBMgrPrimImp_ftable = {
  .AllocId = AllocId,
  .FreeId = FreeId,
  .BindId = BindId,
  .UnBindId = UnBindId,
};

static EBBRep *
EBBMgrPrimImp_createRep(CObjEBBRootMultiRef _self) {
  EBBMgrPrimImpRef repRef;
  EBBPrimMalloc(sizeof(EBBMgrPrimImp), &repRef, EBB_MEM_DEFAULT);
  repRef->ft = &EBBMgrPrimImp_ftable;
  repRef->theRoot = _self;
  return (EBBRep *)repRef;
}

EBBRC
EBBMgrPrimInit()
{
  EBBRC rc = EBBRC_OK;
  if (__sync_bool_compare_and_swap(&theEBBMgrPrimId, (EBBMgrPrimId)0,
                                   (EBBMgrPrimId)-1)) {
    EBBId id;
    CObjEBBRootMultiImpRef rootRef;
    rc = CObjEBBRootMultiImpCreate(&rootRef, EBBMgrPrimImp_createRep);
    LRT_RCAssert(rc);
    rc = EBBAllocPrimIdBoot(&id);
    LRT_RCAssert(rc);
    rc = EBBBindPrimIdBoot(id, CObjEBBMissFunc, (EBBArg)rootRef);
    LRT_RCAssert(rc);
    theEBBMgrPrimId = (EBBMgrPrimId)id;
  } else {
    while ((*(volatile uintptr_t *)&theEBBMgrPrimId)==-1);
  }
  return rc;
}

EBBRC EBBDestroyPrimId(EBBId id)
{
  EBBRC rc;

  // destroy needs to unbind id and trigger resource reclaimation
  //  eg. free unbound id, and
  //      call root's free method on all appropriate locations
  //
  // unbind should only leave id rebound to null
  // free id should allow id to be reused
  // destory unbinds and frees id along with invoke instances specific
  // free logic

  //FIXME: got rid of unbind from lower level, should always do a
  // bind to NULL

  // rc = COBJ_EBBCALL(theEBBMgrPrimId, UnBindId, id, NULL, NULL);

#if 0
  // this may have side effects so I am skipping it
  rc = COBJ_EBBCALL(theEBBMgrPrimId, FreeId, id);
#endif

  lrt_printf("%s: NYI: PLEASE FIXME!: NONE OF THIS WORKS\n",
             __func__);
  // FIMXE:
  rc = 0;
  return rc;
}
