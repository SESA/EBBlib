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

#include <types.h>
#include <core/types.h>
#include <core/const.h>

//FIXME: All Trans Mem is statically allocated
extern struct EBB_Trans_Mem {
  uval8 GMem [EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES];
  uval8 LMem [EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES *
	      EBB_TRANS_MAX_ELS];
  uval8 *free;  // pointer to next available range of GMem
} EBB_Trans_Mem;

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

struct EBBTransLSysStruct {
  EBBGTrans *gTable; //our portion of the gtable for allocating local ebbs
  EBBLTrans *lTable;
  EBBGTrans *free;
  uval numAllocated;
  uval size;
};

static inline EBBId EBBIdAlloc(EBBTransLSys *sys) {
  EBBGTrans *ret = sys->free;
  if(ret != NULL) {
    ret->next = (EBBGTrans *)-1;
    sys->free = sys->free->next;
    sys->numAllocated++;
    return EBBGTransToId(ret);
  }
  int i;
  for (i = 0; i < sys->size; i++) {
    if((uval)sys->gTable[i].next != -1) {
      sys->gTable[i].next = (EBBGTrans *)-1;
      sys->numAllocated++;
      return EBBGTransToId(&sys->gTable[i]);
    }
  }
  return NULL;
}    

static inline uval getLTransNodeId(EBBLTrans *lt) {
  uval val = (((uval)lt) - ((uval)EBB_Trans_Mem.LMem)) /
    (EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES / EBB_TRANS_MAX_NODES);
  return val;
}


static inline void EBBIdFree(EBBTransLSys *sys, EBBId id) {
  EBBGTrans *free = EBBIdToGTrans(id);
  free->next = sys->free;
  sys->free = free;
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

#endif
