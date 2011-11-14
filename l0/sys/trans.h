#ifndef __EBB_TRANS_H__
#define __EBB_TRANS_H__
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

#include <stdint.h>
#include <l0/types.h>
#include <l0/const.h>

//FIXME: All Trans Mem is statically allocated
extern struct EBB_Trans_Mem {
  uint8_t GMem [EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES];
  uint8_t LMem [EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES *
	      EBB_TRANS_MAX_ELS];
  uint8_t *free;  // pointer to next available range of GMem
} EBB_Trans_Mem;

typedef EBBRC (*EBBFunc) (void *);
typedef EBBFunc *EBBFuncTable;

extern EBBFunc EBBDefFT[EBB_TRANS_MAX_FUNCS];
/* extern EBBFunc EBBNullFT[EBB_TRANS_MAX_FUNCS]; */

extern EBBMissFunc theERRMF;

struct EBBTransStruct {
  union {
    uintptr_t v1;
    EBBFuncTable *obj; //as a local entry
    EBBMissFunc mf; //as a global entry
  };
  union {
    uintptr_t v2;
    EBBFuncTable ftable; //as a local entry (by default)
    EBBMissArg arg; //as a global entry
  };
  //FIXME: used for a free list, probably should be separate
  union {
    uintptr_t v3;
    EBBGTrans *next; 
  };
};

static inline EBBId EBBLTransToId(EBBLTrans *lt) {
  return (EBBId)((uintptr_t)lt - EBBMyLTransIndex() *
		 EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES);
}

static inline EBBGTrans * EBBIdToGTrans(EBBId id) {
  return (EBBGTrans *)((uintptr_t)id - (uintptr_t)EBB_Trans_Mem.LMem +
		       (uintptr_t)EBB_Trans_Mem.GMem);
}

static inline EBBId EBBGTransToId(EBBGTrans *gt) {
  return (EBBId)((uintptr_t)gt - (uintptr_t)EBB_Trans_Mem.GMem +
		 (uintptr_t)EBB_Trans_Mem.LMem);
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
  uintptr_t numAllocated;
  uintptr_t size;
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
    if((uintptr_t)sys->gTable[i].next != -1) {
      sys->gTable[i].next = (EBBGTrans *)-1;
      sys->numAllocated++;
      return EBBGTransToId(&sys->gTable[i]);
    }
  }
  return NULL;
}    

static inline uintptr_t getLTransNodeId(EBBLTrans *lt) {
  uintptr_t val = (((uintptr_t)lt) - ((uintptr_t)EBB_Trans_Mem.LMem)) /
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
