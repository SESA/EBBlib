#ifndef __CORE_TYPES_H__
#define __CORE_TYPES_H__
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

#include <l0/const.h>
#include <l0/lrt/types.h>

typedef struct EBBTransStruct EBBTrans;
typedef EBBTrans EBBLTrans;
typedef EBBTrans EBBGTrans;

typedef EBBTrans *EBBId;
#define EBBNullId NULL;

typedef struct EBBTransLSysStruct EBBTransLSys;

typedef intptr_t EBBRC;
typedef enum { 
  EBBRC_GENERIC_FAILURE = -1, 
  EBBRC_BADPARAMETER = -2,
  EBBRC_OUTOFRESOURCES = -3,
  EBBRC_OK = 0 
} EBBRC_STDVALS;
#define EBBRC_SUCCESS(rc) ( rc >= 0 )

typedef uintptr_t FuncNum;
typedef uintptr_t EBBMissArg;

//first arg is the address of the EBBRep that will be executed
//second arg is the local table pointer so that a rep can be installed
typedef EBBRC (*EBBMissFunc) (void *, EBBLTrans *, FuncNum,
			     EBBMissArg);

//FIXME: Not sure if the type of the 2nd arg should be something different
static inline void EBBCacheObj(EBBLTrans *lt, void *obj) {
  //assuming the first element of EBBLTrans 
  //is the object pointer
  *((void **)lt) = obj; 
}

static inline uintptr_t EBBMyEL() {
  return LRTEBBMyEL();
}

static inline uintptr_t EBBMyLTransIndex() {
  return EBBMyEL();
}

static inline EBBLTrans * EBBIdToSpecificLTrans(EBBId id, uintptr_t i) {
  return (EBBLTrans *)((uintptr_t)id + i *
		       EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES);
}

static inline EBBLTrans * EBBIdToLTrans(EBBId id) {
  return EBBIdToSpecificLTrans(id, EBBMyLTransIndex());
}

#define EBBId_DREF(id) ((typeof(*id))(*(void **)EBBIdToLTrans((EBBId)id)))

#endif
