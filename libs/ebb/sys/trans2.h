#ifndef __EBB_TRANS_H__
#define __EBB_TRANS_H__

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

#include "../../types.h"

#define EBB_TRANS_MAX_ELS (16)
#define EBB_TRANS_PAGE_SIZE (4096)
#define EBB_TRANS_NUM_PAGES (1024)
#define EBB_TRANS_MAX_FUNCS (256)


//FIXME: All Trans Mem is statically allocated
extern struct EBB_Trans_Mem {
  u8 GMem [EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES];
  u8 LMem [EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES *
	      EBB_TRANS_MAX_ELS];
  u8 *free;  // pointer to next available range of GMem
} EBB_Trans_Mem;

void EBB_Trans_Mem_Init(void) {
  EBB_Trans_Mem.free = EBB_Trans_Mem.GMem;
}  
// could also do initial mapping here if memory is
// is not actually static reservation

void EBB_Trans_Mem_Alloc_Pages(uval num_pages, u8 **pages) {
  if (&(EBB_Trans_Mem.free[EBB_TRANS_PAGE_SIZE * num_pages]) >
      &(EBB_Trans_Mem.GMem[EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES])) {
    *pages = NULL;
  } else {
    *pages = EBB_Trans_Mem.free;
    EBB_Trans_Mem.free = EBB_Trans_Mem.free + (EBB_TRANS_PAGE_SIZE * num_pages);
  }
}

typedef sval EBBRC;
typedef enum { EBBRC_FAILURE = -1, EBBRC_OK = 0 } EBBRC_STDVALS;
#define EBBRC_SUCCESS(rc) ( rc >= 0 )

typedef uval FuncNum;
typedef uval EBBMissArg;

typedef struct EBBTransStruct EBBTrans;
typedef EBBTrans EBBLTrans;
typedef EBBTrans EBBGTrans;

typedef EBBRC (*EBBFunc) (void);
typedef EBBRC (*EBBMissFunc) (EBBLTrans *, FuncNum, EBBMissArg);
typedef EBBFunc *EBBFuncTable;

extern EBBFunc EBBDefFT[EBB_TRANS_MAX_FUNCS];
extern EBBFunc EBBNullFT[EBB_TRANS_MAX_FUNCS];

extern EBBMissFunc theERRMF;

struct EBBTransStruct {
  union {
    uval v1;
    EBBFuncTable *obj;
    EBBMissFunc mf;
  };
  union {
    uval v2;
    EBBFuncTable ftable;
    EBBMissArg arg;
  };
  union {
    uval v3;
    EBBGTrans *next;
  };
};

typedef EBBTrans *EBBId;

static inline uval myEL() {
  return 0;
}

static inline uval myLTransIndex() {
  return myEL();
}

static inline EBBLTrans * EBBIdToLTrans(EBBId id) {
  return (EBBLTrans *)((uval)id + myLTransIndex() * 
		       EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES);
}

static inline EBBId EBBLTransToId(EBBLTrans *lt) {
  return (EBBId)((uval)lt - myLTransIndex() *
		 EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES);
}

static inline EBBGTrans * EBBIdToGTrans(EBBId id) {
  return (EBBGTrans *)((uval)id - (uval)EBB_Trans_Mem.LMem +
		       (uval)EBB_Trans_Mem.GMem);
}

static inline EBBId EBBGTransToId(EBBGTrans *gt) {
  return (EBBId)((uval)gt - (uval)EBB_Trans_Mem.GMem +
		 (uval)EBB_Trans_Mem.LMem);
}

static inline EBBGTrans * EBBLTransToGTrans(EBBLTrans *lt) {
  return EBBIdToGTrans(EBBLTransToId(lt));
}

static inline EBBLTrans * EBBGTransToLTrans(EBBGTrans *gt) {
  return EBBIdToLTrans(EBBGTransToId(gt));
}

typedef struct EBBTransLSysStruct {
  EBBGTrans *gTable;
  EBBLTrans *lTable;
  EBBGTrans *free;
  uval numAllocated;
  uval size; //number of EBBGTrans in our portion of the gTable
} EBBTransLSys;

static inline EBBId EBBIdAlloc(EBBTransLSys *sys) {
  EBBGTrans *ret = sys->free;
  if(ret != NULL) {
    ret->next = (EBBGTrans *)-1;
    sys->free = sys->free->next;
    return EBBGTransToId(ret);
  }
  int i;
  for (i = 0; i < sys->size; i++) {
    if((uval)sys->gTable[i].next != -1) {
      sys->gTable[i].next = (EBBGTrans *)-1;
      return EBBGTransToId(&sys->gTable[i]);
    }
  }
  return NULL;
}
    
static inline void EBBIdFree(EBBTransLSys *sys, EBBId id) {
  EBBGTrans *free = EBBIdToGTrans(id);
  free->next = sys->free;
  sys->free = free;
}

static inline void EBBCacheObj(EBBLTrans *lt, EBBFuncTable *obj) {
  lt->obj = obj;
}

static inline void EBBSetLTrans(EBBLTrans *lt,
				EBBFuncTable ftable) {
  EBBCacheObj(lt, &lt->ftable);
  lt->ftable = ftable;
}

static inline void EBBSetALLLTrans(EBBId id, EBBFuncTable ftable) {
  int i;
  EBBLTrans *lt;
  for (i = 0; i < EBB_TRANS_MAX_ELS; i++) {
    lt = (EBBLTrans *)(id + i * 
		       EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES);
    EBBSetLTrans(lt, ftable);
  }
}

static inline EBBIdBind(EBBId id, EBBMissFunc mf, 
			EBBMissArg arg) {
  EBBGTrans *gt = EBBIdToGTrans(id);
  gt->mf = mf;
  gt->arg = arg;
}

static inline EBBIdUnBind(EBBId id, EBBMissFunc *mf, EBBMissArg *arg) {
  EBBGTrans *gt = EBBIdToGTrans(id);
  *mf = gt->mf;
  *arg = gt->arg;
  //FIXME: this is how we reset the local tables after an unbind
  EBBSetALLLTrans(id, EBBDefFT);
  EBBIdBind(id, theERRMF, 0);
}

#define EBBId_DREF(id) (*EBBIdToLTrans(id))
#endif
