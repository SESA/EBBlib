#ifndef __LRT_TRANS_H__
#define __LRT_TRANS_H__

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

#include <l0/lrt/event_loc.h>

//forward declaration for the lrt specific headers
struct lrt_trans;

// prototypes for the common set of functions all platforms must provide
static inline struct lrt_trans *lrt_trans_id2lt(uintptr_t i);
static inline uintptr_t lrt_trans_lt2id(struct lrt_trans *t);
static inline struct lrt_trans *lrt_trans_id2gt(uintptr_t i);
static inline uintptr_t lrt_trans_gt2id(struct lrt_trans *t);
static inline struct lrt_trans *lrt_trans_gt2lt(struct lrt_trans *gt);
static inline struct lrt_trans *lrt_trans_lt2gt(struct lrt_trans *lt);

// returns the pointer to a remote local translation entry for a object id
union EBBTransStruct;
typedef union EBBTransStruct EBBTrans;

typedef EBBTrans EBBGTrans;

typedef EBBTrans EBBLTrans;

typedef EBBTrans *EBBId;

typedef intptr_t EBBRC;
typedef enum { 
  EBBRC_GENERIC_FAILURE = -1, 
  EBBRC_BADPARAMETER = -2,
  EBBRC_OUTOFRESOURCES = -3,
  EBBRC_RETRY = -4,
  EBBRC_NOTFOUND = -5,
  EBBRC_OK = 0 
} EBBRC_STDVALS;

#define EBBRC_SUCCESS(rc) ( rc >= 0 )
//The argument type here has to be void * because c sucks
// and won't let me forward declare a typedef
// It should be of type EBBRep *
typedef EBBRC (*EBBFunc) (void *);
typedef EBBFunc *EBBRep;
typedef EBBRep *EBBRepRef;

typedef uintptr_t FuncNum;
typedef uintptr_t EBBMissArg;

//first arg is the address of the EBBRep that will be executed
//second arg is the local table pointer so that a rep can be installed
typedef EBBRC (*EBBMissFunc) (EBBRep **, EBBLTrans *, FuncNum,
                              EBBMissArg);
#ifdef __cplusplus
extern "C" {
#endif
  extern void EBBCacheObj(EBBLTrans *lt, EBBRep *obj);
#ifdef __cplusplus
}
#endif

static inline 
EBBLTrans * 
EBBIdToLTrans(EBBId id) {
  return (EBBLTrans *)lrt_trans_id2lt((uintptr_t)id);
}

static inline 
EBBRep *
EBBId_DREF_Inline(EBBId id) {
  EBBLTrans *lt;
  lt = EBBIdToLTrans(id);
  //Here we are using the fact that the first word of the ltrans
  // is a rep pointer
  return *(EBBRep **)lt;
}

#define EBBId_DREF(id) ((typeof(*id))EBBId_DREF_Inline((EBBId)id))

#define EBB_TRANS_MAX_FUNCS (256)

extern EBBFunc EBBDefFT[EBB_TRANS_MAX_FUNCS];

extern struct lrt_trans *lrt_trans_id2rlt(lrt_event_loc el, uintptr_t objid);
extern void lrt_trans_init(void);

// real implementations come from these files
#ifdef LRT_ULNX
#include <l0/lrt/ulnx/trans.h>
#elif LRT_BARE
#include <l0/lrt/bare/trans.h>
#endif

#include <lrt/assert.h>

// this has been sized for the future use
// doing multi-node translations
// eg.  some bits used as a key map to a node
//      some bits used as a key map to a 
//      lrt_trans pointer
struct lrt_trans {
  uint64_t vals[4];
};

#define LRT_TRANS_NUMIDS_PERPAGE \
  (LRT_TRANS_PAGESIZE / sizeof(struct lrt_trans))

STATIC_ASSERT(sizeof(struct lrt_trans) * LRT_TRANS_NUMIDS_PERPAGE == 
	       LRT_TRANS_PAGESIZE, "translation table size mismatch!");

#endif
