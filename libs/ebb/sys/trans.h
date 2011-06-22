/* Copyright 2011 Boston University. All rights reserved. */

/* Redistribution and use in source and binary forms, with or without modification, are */
/* permitted provided that the following conditions are met: */

/*    1. Redistributions of source code must retain the above copyright notice, this list of */
/*       conditions and the following disclaimer. */

/*    2. Redistributions in binary form must reproduce the above copyright notice, this list */
/*       of conditions and the following disclaimer in the documentation and/or other materials */
/*       provided with the distribution. */

/* THIS SOFTWARE IS PROVIDED BY BOSTON UNIVERSITY ``AS IS'' AND ANY EXPRESS OR IMPLIED */
/* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND */
/* FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BOSTON UNIVERSITY OR */
/* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR */
/* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR */
/* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF */
/* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

/* The views and conclusions contained in the software and documentation are those of the */
/* authors and should not be interpreted as representing official policies, either expressed */
/* or implied, of Boston University */

#ifndef __EBB_TRANS_H__
#define __EBB_TRANS_H__

#include "../../types.h"

static inline uval myEL() {
  return 0;
}

#define EBB_NUM_IDS 100
#define EBB_MAX_FUNCS 256

typedef EBBRC (*EBBFunc) (uval arg);
typedef EBBFunc EBBFuncTable[];

extern EBBFuncTable EBBDefFT[EBB_MAXFUNCS];
extern EBBFuncTable EBBNullFT[EBB_MAXFUNCS];

typedef struct EBBCallDesc {
  EBBFuncTable * funcs;
  uval           extra;
} EBBCallDesc;

typedef struct EBBTrans {
  EBBCallDesc fdesc;
  uval transVal;
} EBBTrans;

typedef EBBTrans EBBLTrans;
typedef EBBTrans EBBGTrans;

typedef struct EBBTransLSysStruct {
  EBBGTrans *gTable;
  EBBLTrans *lTable;
  EBBGTrans *free;
  uval numAllocated;
} EBBTransLSys;

typedef EBBTrans *EBBId;

#define EBBIdNull 0

static inline EBBLTrans * EBBIdToLTrans(EBBTransLSys *sys, EBBid id)
{
  return (EBBLTrans *)(id + myEl() * EBB_NUM_IDS);
}

static inline EBBid EBBLTransToId(EBBTransLSys *sys, EBBLTrans *lt)
{
  return (EBBid)(lt - myEl() * EBB_NUM_IDS);
}

static inline EBBGTrans * EBBLTransToGTrans(EBBTransLSys *sys, EBBLTrans *lt)
{
  return (EBBGTrans *)((uval)lt - (uval)sys->lTable + (uval)sys->gTable);
}

static inline EBBLTrans * EBBGTransToLTrans(EBBTransLSys *sys, EBBGTrans *gt)
{
  return (EBBLTrans *)((uval)gt - (uval)sys->gTable + (uval)sys->lTable);
}

static inline EBBGTrans * EBBIdToGTrans(EBBTransLSys *sys, EBBid id)
{
  return EBBLTransToGTrans(sys, EBBIdToLTrans(sys, id));
}

static inline EBBid EBBGTransToId(EBBTransLSys *sys, EBBGTrans *gt) {
  return EBBLTransToId(sys, EBBGTransToLTrans(sys, gt));
}

static inline EBBid * EBBIdAlloc(EBBTransLSys *sys)
{
  EBBGTrans *ret = sys->free;
  sys->free = (EBBGTrans *)sys->free->transVal;
  return EBBGTransToId(ret);
}

static inline void EBBIdFree(EBBTransLSys *sys, EBBid id)
{
  EBBGTrans *free = EBBIdToGTrans(sys, id);
  free->transVal = (uval)sys->free;
  sys->free = free;
}

static inline void EBBIdBind(EBBTransLSys *sys, EBBid id, uval v1, uval v2)
{
  EBBGTrans *gt = EBBIdToGTrans(sys, id);
  gt->fdesc.funcs = (EBBFuncTable *)v1;
  gt->extra = v2;
}

static inline void EBBIdUnBind(EBBTransLSys *sys, EBBid id, uval *v1, uval *v2)
{
  EBBGTrans *gt = EBBIdToGTrans(sys, id);
  *v1 = gt->fdesc.funcs;
  *v2 = gt->extra;
}

#define EBBId_DREF(id) *EBBIdToLTrans(id)
/* #define EBBId_CALL(id, f, ...) ((*id)->fdesc.funcs[f](&id->fdesc))   */

#endif
