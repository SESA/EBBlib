/* Copyright 2011 Boston University. All rights reserved.                  */
/*                                                                         */  
/* Redistribution and use in source and binary forms, with or without      */ 
/* modification, are permitted provided that the following conditions are  */
/* met:                                                                    */
/*                                                                         */  
/*    1. Redistributions of source code must retain the above copyright    */
/*       notice, this list of conditions and the following disclaimer.     */
/*                                                                         */  
/*    2. Redistributions in binary form must reproduce the above copyright */ 
/*       notice, this list of conditions and the following disclaimer in   */
/*       the documentation and/or other materials  provided with the       */
/*       distribution.                                                     */
/*                                                                         */  
/*                                                                         */  
/* THIS SOFTWARE IS PROVIDED BY BOSTON UNIVERSITY ``AS IS'' AND ANY        */
/* EXPRESS OR IMPLIED  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      */
/* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BOSTON UNIVERSITY OR          */
/* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,   */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,     */
/* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR      */
/* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF  */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING    */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS      */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.            */
/*                                                                         */  
/* The views and conclusions contained in the software and documentation   */
/* are those of the  authors and should not be interpreted as representing */
/* official policies, either expressed or implied, of Boston University    */

#ifndef __EBB_TRANS_H__
#define __EBB_TRANS_H__

#include "../../base/types.h"
#include "../EBBTypes.h"
#include "../EBBConst.h"

//FIXME: All Trans Mem is statically allocated
extern struct EBB_Trans_Mem {
  uval8 GMem [EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES];
  uval8 LMem [EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES *
	      EBB_TRANS_MAX_ELS];
  uval8 *free;  // pointer to next available range of GMem
} EBB_Trans_Mem;

static void EBB_Trans_Mem_Init(void) {
  EBB_Trans_Mem.free = EBB_Trans_Mem.GMem;
}  
// could also do initial mapping here if memory is
// is not actually static reservation

static void EBB_Trans_Mem_Alloc_Pages(uval num_pages, uval8 **pages) {
  if (&(EBB_Trans_Mem.free[EBB_TRANS_PAGE_SIZE * num_pages]) >
      &(EBB_Trans_Mem.GMem[EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES])) {
    *pages = NULL;
  } else {
    *pages = EBB_Trans_Mem.free;
    EBB_Trans_Mem.free = EBB_Trans_Mem.free + (EBB_TRANS_PAGE_SIZE * num_pages);
  }
}

typedef EBBRC (*EBBFunc) (void *);
typedef EBBFunc *EBBFuncTable;

extern EBBFunc EBBDefFT[EBB_TRANS_MAX_FUNCS];
/* extern EBBFunc EBBNullFT[EBB_TRANS_MAX_FUNCS]; */

extern EBBMissFunc theERRMF;

struct EBBTransStruct {
  union {
    uval v1;
    EBBFuncTable *obj; //as a local entry
    EBBMissFunc mf; //as a global entry
  };
  union {
    uval v2;
    EBBFuncTable ftable; //as a local entry (by default)
    EBBMissArg arg; //as a global entry
  };
  //FIXME: used for a free list, probably should be separate
  union {
    uval v3;
    EBBGTrans *next; 
  };
  union {
    uval v4; //FIXME: padding - there was a bug without this
    EBBMissFunc globalMF;
  };
};

static inline EBBId EBBLTransToId(EBBLTrans *lt) {
  return (EBBId)((uval)lt - EBBMyLTransIndex() *
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

/* struct EBBTransLSysStruct { */
/*   EBBGTrans *gTable; */
/*   EBBLTrans *lTable; */
/*   EBBGTrans *free; */
/*   uval numAllocated; */
/*   uval size; //number of EBBGTrans in our portion of the gTable */
/* }; */

/* static inline EBBId EBBIdAlloc(EBBTransLSys *sys) { */
/*   EBBGTrans *ret = sys->free; */
/*   if(ret != NULL) { */
/*     ret->next = (EBBGTrans *)-1; */
/*     sys->free = sys->free->next; */
/*     return EBBGTransToId(ret); */
/*   } */
/*   int i; */
/*   for (i = 0; i < sys->size; i++) { */
/*     if((uval)sys->gTable[i].next != -1) { */
/*       sys->gTable[i].next = (EBBGTrans *)-1; */
/*       return EBBGTransToId(&sys->gTable[i]); */
/*     } */
/*   } */
/*   return NULL; */
/* } */
struct EBBTransLSysStruct {
  EBBGTrans *localGTable; //our portion of the gtable for allocating local ebbs
  EBBLTrans *localLTable;
  EBBGTrans *localFree;
  uval localNumAllocated;
  uval localSize;
  EBBGTrans *globalGTable; //our portion of the gtable for allocating global ebbs
  EBBLTrans *globalLTable;
  EBBGTrans *globalFree;
  uval globalNumAllocated;
  uval globalSize;
};

static inline uval isGlobalSetup(EBBTransLSys *sys) {
  return !!sys->globalGTable;
}

static inline void SetupGlobal(EBBTransLSys *sys, uval nodeId) {
  uval numGTransPerEL;
  numGTransPerEL = EBB_TRANS_NUM_PAGES * EBB_TRANS_PAGE_SIZE / 
    sizeof(EBBGTrans) / EBB_TRANS_MAX_ELS / EBB_TRANS_MAX_NODES;
  
  //FIXME: assuming gsys.pages = EBB_TRANS_NUM_PAGES
  //I can't possibly get this right
  sys->globalGTable = (EBBGTrans *)
    &EBB_Trans_Mem.GMem[nodeId * 
		       EBB_TRANS_PAGE_SIZE * 
		       EBB_TRANS_NUM_PAGES / EBB_TRANS_MAX_NODES +
		       numGTransPerEL * EBBMyEL() * sizeof(EBBGTrans)];
  sys->globalLTable = EBBGTransToLTrans(sys->globalGTable);
  sys->globalFree = NULL;
  sys->globalNumAllocated = 0;
  sys->globalSize = numGTransPerEL;
}
  
    
static inline EBBId EBBIdAllocGlobal(EBBTransLSys *sys) {
  EBBGTrans *ret = sys->globalFree;
  if(ret != NULL) {
    ret->next = (EBBGTrans *)-1;
    sys->globalFree = sys->globalFree->next;
    sys->globalNumAllocated++;
    return EBBGTransToId(ret);
  }
  int i;
  for (i = 0; i < sys->globalSize; i++) {
    if((uval)sys->globalGTable[i].next != -1) {
      sys->globalGTable[i].next = (EBBGTrans *)-1;
      sys->globalNumAllocated++;
      return EBBGTransToId(&sys->globalGTable[i]);
    }
  }
  return NULL;
}    

static inline EBBId EBBIdAllocLocal(EBBTransLSys *sys) {
  EBBGTrans *ret = sys->localFree;
  if(ret != NULL) {
    ret->next = (EBBGTrans *)-1;
    sys->localFree = sys->localFree->next;
    sys->localNumAllocated++;
    return EBBGTransToId(ret);
  }
  int i;
  for (i = 0; i < sys->localSize; i++) {
    if((uval)sys->localGTable[i].next != -1) {
      sys->localGTable[i].next = (EBBGTrans *)-1;
      sys->localNumAllocated++;
      return EBBGTransToId(&sys->localGTable[i]);
    }
  }
  return NULL;
}    

static inline uval getLTransNodeId(EBBLTrans *lt) {
  uval val = (((uval)lt) - ((uval)EBB_Trans_Mem.LMem)) /
    (EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES / EBB_TRANS_MAX_NODES);
  return val;
}

//FIXME: assuming gsys.pages = EBB_TRANS_NUM_PAGES
//this is probably wrong with some +/- 1 issue
static inline uval isLocalEBB(EBBGTrans *gt) {
  return gt < 
    &EBB_Trans_Mem.GMem[EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES /
			EBB_TRANS_MAX_NODES];
}



static inline void EBBIdFree(EBBTransLSys *sys, EBBId id) {
  EBBGTrans *free = EBBIdToGTrans(id);
  if(isLocalEBB(free)) {
    free->next = sys->localFree;
    sys->localFree = free;
  } else {
    free->next = sys->globalFree;
    sys->globalFree = free;
  }
}

static inline void EBBSetLTrans(EBBLTrans *lt,
				EBBFuncTable ftable) {
  EBBCacheObj(lt, &lt->ftable);
  lt->ftable = ftable;
}

static inline void EBBSetAllLTrans(EBBId id, EBBFuncTable ftable) {
  int i;
  EBBLTrans *lt;
  for (i = 0; i < EBB_TRANS_MAX_ELS; i++) {
    lt = EBBIdToSpecificLTrans(id, i);
    EBBSetLTrans(lt, ftable);
  }
}

static inline void EBBIdBind(EBBId id, EBBMissFunc mf, 
			     EBBMissArg arg) {
  EBBGTrans *gt = EBBIdToGTrans(id);
  gt->mf = mf;
  gt->arg = arg;
}

static inline void EBBIdBindGlobal(EBBId id, EBBMissFunc mf,
				   EBBMissArg arg, EBBMissFunc globalMF) {
  EBBGTrans *gt = EBBIdToGTrans(id);
  gt->mf = mf;
  gt->arg = arg;
  gt->globalMF = globalMF;
}

static inline void EBBIdUnBind(EBBId id, EBBMissFunc *mf,
			       EBBMissArg *arg) {
  EBBGTrans *gt = EBBIdToGTrans(id);
  if (mf)
    *mf = gt->mf;
  if (arg)
    *arg = gt->arg;
  //FIXME: this is how we reset the local tables after an unbind
  EBBSetAllLTrans(id, EBBDefFT);
  EBBIdBind(id, theERRMF, 0);
}

//initialize the portion of ltable from lt
// to the specified number of pages
static void initLTable(EBBLTrans *lt, uval pages) {
  EBBLTrans *iter;
  for (iter = lt; 
       iter < (&((char *)lt)[EBB_TRANS_PAGE_SIZE * pages]);
       iter++) {
    EBBSetLTrans(iter, EBBDefFT);
  }
}

//init all ltables from lt to the specified number of pages
//id must be the first id allocated
static void initAllLTables(EBBId id, uval pages) {
  int i;
  for (i = 0; i < EBB_TRANS_MAX_ELS; i++) {
    initLTable(EBBIdToSpecificLTrans(id, i), pages);
  }
}

// FIXME: JA think there is a bug here.  We should be calcing NUM explicity
static void initGTable(EBBGTrans *gt, uval pages) {
  EBBGTrans *iter;
  for (iter = gt;
       iter < (EBBGTrans *)((uval)gt + pages * EBB_TRANS_PAGE_SIZE);
       iter++) {
    EBBIdBind(EBBGTransToId(iter), theERRMF, 0);
  }
}
  
#endif
