#ifndef __EBBMGRPRIM_H__
#define __EBBMGRPRIM_H__
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

#include <l0/lrt/trans.h>
#include <l0/cobj/CObjEBB.h>

typedef enum {
  EBBRC_GENERIC_FAILURE = -1,
  EBBRC_BADPARAMETER = -2,
  EBBRC_OUTOFRESOURCES = -3,
  EBBRC_RETRY = -4,
  EBBRC_NOTFOUND = -5,
  EBBRC_OK = 0
} EBBRC_STDVALS;

typedef lrt_trans_rc EBBRC;
#define EBBRC_SUCCESS(rc) LRT_TRANS_RC_SUCCESS(rc)

typedef lrt_trans_func EBBFunc;
typedef lrt_trans_rep EBBRep;
typedef lrt_trans_rep_ref EBBRepRef;
typedef lrt_trans_func_num EBBFuncNum;
typedef lrt_trans_arg EBBArg;
typedef lrt_trans_ltrans EBBLTrans;
typedef lrt_trans_id EBBId;
typedef EBBRC (*EBBMissFunc) (EBBRepRef *,
                              EBBLTrans *,
                              EBBFuncNum,
                              EBBArg);
#ifndef __cplusplus
STATIC_ASSERT(__builtin_types_compatible_p(EBBMissFunc, lrt_trans_miss_func),
              "EBBMissFunc and lrt_trans_miss_func are not compatible types");
#endif

typedef void (*EBBBindFunc) (EBBMissFunc *mf, void *bf, EBBArg *arg,
                             EBBArg oldarg);

static inline void
EBBCacheObj(EBBLTrans *lt, EBBRepRef ref) {
  lrt_trans_cache_obj(lt, ref);
}

#define EBBId_DREF(id) ((typeof(*id))lrt_trans_id_dref((EBBId)id))

COBJ_EBBType(EBBMgrPrim) {
  EBBRC (*AllocId) (EBBMgrPrimRef _self, EBBId *id);
  EBBRC (*FreeId) (EBBMgrPrimRef _self, EBBId id);
  EBBRC (*BindId) (EBBMgrPrimRef _self, EBBId id, EBBMissFunc *mf,
                   EBBBindFunc *bf, EBBArg *arg, int force);
  // note, this will just result in a bind of the id to null
  EBBRC (*UnBindId) (EBBMgrPrimRef _self, EBBId id);
};
extern EBBMgrPrimId theEBBMgrPrimId;

extern EBBRC EBBMgrPrimInit(void);


static inline EBBRC
EBBAllocPrimId(EBBId *id)
{
  return COBJ_EBBCALL(theEBBMgrPrimId, AllocId, id);
}

static inline EBBRC
EBBBindPrimId(EBBId id, EBBMissFunc mf, EBBArg arg)
{
  EBBMissFunc mftemp = mf;
  EBBBindFunc bftemp = NULL;
  EBBArg argtemp = arg;
  return COBJ_EBBCALL(theEBBMgrPrimId, BindId, id, &mftemp, &bftemp, &argtemp, 0);
}

extern EBBRC EBBDestroyPrimId(EBBId id);

#endif
