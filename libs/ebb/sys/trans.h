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

#define EBB_NUM_ELS (16)
#define EBB_NUM_IDS (100)
#define EBB_MAX_FUNCS (256)

typedef sval EBBRC;
typedef enum { EBBRC_FAILURE = -1, EBBRC_OK = 0 } EBBRC_STDVALS;
#define EBBRC_SUCCESS(rc) ( rc >= 0 )

typedef EBBRC (*EBBFunc) (void);
typedef EBBRC (*EBBDefFunc) (EBBLTrans *, uval);
typedef EBBFunc EBBFuncTable[];

extern EBBFunc EBBDefFT[EBB_MAX_FUNCS];
extern EBBFunc EBBNullFT[EBB_MAX_FUNCS];

/* EBBTRANS: */
/* function table pointer: owned by the user, */
/*   overwritten on a bind to point to the correct object */
/* extra: owned by the EBBManager for whatever purpose it needs */
/* transVal: owned by the translation system for whatever purpose it needs */

typedef struct EBBCallDescStruct {
  EBBFuncTable * funcs;
  uval           extra;
} EBBCallDesc;

typedef struct EBBTransStruct {
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

static inline EBBLTrans * EBBIdToLTrans(EBBTransLSys *sys, EBBId id)
{
  return (EBBLTrans *)(id + myEl() * EBB_NUM_IDS);
}

static inline EBBId *EBBLTransToId(EBBTransLSys *sys, EBBLTrans *lt)
{
  return (EBBId *)(lt - myEl() * EBB_NUM_IDS);
}

static inline EBBGTrans * EBBLTransToGTrans(EBBTransLSys *sys, EBBLTrans *lt)
{
  return (EBBGTrans *)((uval)lt - (uval)sys->lTable + (uval)sys->gTable);
}

static inline EBBLTrans * EBBGTransToLTrans(EBBTransLSys *sys, EBBGTrans *gt)
{
  return (EBBLTrans *)((uval)gt - (uval)sys->gTable + (uval)sys->lTable);
}

static inline EBBGTrans * EBBIdToGTrans(EBBTransLSys *sys, EBBId id)
{
  return EBBLTransToGTrans(sys, EBBIdToLTrans(sys, id));
}

static inline EBBId * EBBGTransToId(EBBTransLSys *sys, EBBGTrans *gt) {
  return EBBLTransToId(sys, EBBGTransToLTrans(sys, gt));
}

static inline EBBId * EBBIdAlloc(EBBTransLSys *sys)
{
  EBBGTrans *ret = sys->free;
  if(ret == NULL) {
    return EBBIdNull;
  }
  sys->free = (EBBGTrans *)sys->free->transVal;
  sys->numAllocated++;
  return EBBGTransToId(sys, ret);
}

static inline void EBBIdFree(EBBTransLSys *sys, EBBId id)
{
  EBBGTrans *free = EBBIdToGTrans(sys, id);
  free->transVal = (uval)sys->free;
  sys->free = free;
}

static inline void EBBIdBind(EBBTransLSys *sys, EBBId id, uval v1, uval v2)
{
  EBBGTrans *gt = EBBIdToGTrans(sys, id);
  gt->fdesc.funcs = (EBBFuncTable *)v1;
  gt->fdesc.extra = v2;
}

static inline void EBBIdUnBind(EBBTransLSys *sys, EBBId id, uval *v1, uval *v2)
{
  EBBGTrans *gt = EBBIdToGTrans(sys, id);
  *v1 = (uval)(gt->fdesc.funcs);
  *v2 = gt->fdesc.extra;
}

#define EBBId_DREF(id) **EBBIdToLTrans(id)
/* #define EBBId_CALL(id, f, ...) ((*id)->fdesc.funcs[f](&id->fdesc))   */

#endif
