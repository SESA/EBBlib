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

typedef uval FuncNum;
typedef uval EBBMissArg;

typedef EBBRC (*EBBFunc) (void);
typedef EBBRC (*EBBMissFunc) (EBBLTrans *, FuncNum, EBBMissArg);
typedef EBBFunc EBBFuncTable[];

extern EBBFunc EBBDefFT[EBB_MAX_FUNCS];
extern EBBFunc EBBNullFT[EBB_MAX_FUNCS];

extern EBBMissFunc theERRMF;

/* EBBTRANS: */
/* function table pointer: owned by the user, */
/*   overwritten on a bind to point to the correct object */
/* extra: owned by the EBBManager for whatever purpose it needs */
/* transVal: owned by the translation system for whatever purpose it needs */

typedef struct EBBTransStruct EBBTrans;
typedef EBBTrans EBBLTrans;
typedef EBBTrans EBBGTrans;

typedef struct EBBCallDescStruct {
  union {
    EBBFuncTable * funcs;
    EBBMissFunc mf;
  };
  union {
    uval           extra;
    EBBFuncTable ftable;
    EBBMissArg     arg;
  };
} EBBCallDesc;

typedef struct EBBTransStruct {
  EBBCallDesc fdesc;
  union {
    uval transVal;
    EBBGTrans *next;
  }
} EBBTrans;


typedef struct EBBTransLSys {
  EBBGTrans *gTable; //the global table
  EBBLTrans *lTable; //my local table
  EBBGTrans *free; 
  uval numAllocated;
  uval numIds;
} EBBTransLSys;

typedef EBBTrans *EBBId;
#define EBBIdNull 0

#if 0
// At some point we will need to support multiple trans system instances
// This code is template code is meant to document what we had in mind
// for this
typedef struct EBBTransGSys {
  uval numIds;
} EBBTransGSys;

typedef sval EBBTransSysId
#define EBB_TRANS_SYS_MAX (4) //num free bits for 32 bit aligned pointer

typedef struct EBBTransSystems {
  EBBTRansGSys transSystems[EBB_TRANS_SYS_MAX];
  uval free;
} EBBTransSystems;

extern EBBTransSystems theEBBTransSystems;

EBBTransSysId  EBBTransAllocSys() {
  uval sysid = theEBBTransSystems.free;

  theEBBTransSystems.free++;
  if (theEBBTransSystems.free > EBB_TRANS_SYS_MAX) return -1;
  return sysid;
}

EBBTransSysId EBBIdToSysId(EBBid id) { 
  return id & ((1 << EBB_TRANS_SYS_MAX) - 1);
}

void EBBIdSetSysId(EBBid *id, EBBTransSysId sid) {
  *id = *id | (sid & ((1 << EBB_TRANS_SYS_MAX) - 1));
}

static inline EBBNumIds(EBBId id) { 
  return EBBTransSystems[EBBIdToSysId(id)].numIds; 
}
#endif

// FIXME:  THIS IS HERE SO THAT WE CAN ADD A LEVEL OF INDIRECTION
//         TO SUPPORT MULTIPLE EBBTRANS SYS INSTANCES
//         (LIKE WE NEED ANOTHER F#%^#%^ LEVEL OF INDIRECTION :-)
static inline EBBNumIds(EBBId id) { return EBB_NUM_IDS; }

// We avoid using sys to compute your ltrans so that dref is
// efficient and usable
static inline EBBLTrans * EBBIdToLTrans(EBBId id)
{
  return (EBBLTrans *)(id + myEl() * EBBNumIds(id));
}

static inline uval EBBSysNumIds(EBBTransLSys *sys) {
  return sys->numIds;
}

static inline EBBId EBBLTransToId(EBBTransLSys *sys, EBBLTrans *lt)
{
  return (EBBId)(lt - myEl() * EBBSysNumIds(sys));
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

static inline EBBId EBBGTransToId(EBBTransLSys *sys, EBBGTrans *gt) {
  return EBBLTransToId(sys, EBBGTransToLTrans(sys, gt));
}

static inline EBBId EBBIdAlloc(EBBTransLSys *sys)
{
  EBBGTrans *ret = sys->free;
  if(ret == NULL) {
    return EBBIdNull;
  }
  sys->free = (EBBGTrans *)sys->free->next;
  sys->numAllocated++;
  return EBBGTransToId(sys, ret);
}

static inline void EBBIdFree(EBBTransLSys *sys, EBBId id)
{
  EBBGTrans *free = EBBIdToGTrans(sys, id);
  free->next = (uval)sys->free;
  sys->free = free;
}

// We expect this to be used by external code 
// such as miss handing functions to cache an local
// object for translation on future calls.
static inline void EBBCacheObj(EBBLTrans *lt,
				 EBBFuncTable *funcs) {
  lt->funcs = funcs;
}

static inline void EBBSetLTrans(EBBLTrans *lt,
				EBBFuncTable ftable) {
  EBBInstallObj(&lt->ftable);
  lt->ftable = ftable;
}

// JA: BIGFING KLUDGE 
static inline void EBBSetALLLTrans(EBBId id, EBBFuncTable ftable) {
  
}

static inline void EBBIdBind(EBBTransLSys *sys, EBBId id,
			     EBBMissFunc mf, EBBMissArg arg)
{
  EBBGTrans *gt = EBBIdToGTrans(sys, id);

  gt->fdesc.mf = mf;
  gt->fdesc.arg = arg;

}

static inline void EBBIdUnBind(EBBTransLSys *sys, EBBId id,
			       EBBMissFunc *mf, EBBMissArg *arg)
{
  EBBGTrans *gt = EBBIdToGTrans(sys, id);
  *mf = gt->fdesc.mf; 
  *arg = gt->fdesc.arg;
  
  EBBIdBind(sys, id, theERRMF, 0);
}

#define EBBId_DREF(id) **EBBIdToLTrans(id)
/* #define EBBId_CALL(id, f, ...) ((*id)->fdesc.funcs[f](&id->fdesc))   */

#endif
