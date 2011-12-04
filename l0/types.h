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

#ifndef NULL
#define NULL ((void *)0)
#endif /* ! NULL */

typedef union EBBTransStruct EBBTrans;
typedef EBBTrans EBBLTrans;

typedef EBBTrans *EBBId;

typedef intptr_t EBBRC;
typedef enum { 
  EBBRC_GENERIC_FAILURE = -1, 
  EBBRC_BADPARAMETER = -2,
  EBBRC_OUTOFRESOURCES = -3,
  EBBRC_OK = 0 
} EBBRC_STDVALS;
#define EBBRC_SUCCESS(rc) ( rc >= 0 )

//The argument type here has to be void * because c sucks
// and won't let me forward declare a typedef
// It should be of type EBBRep *
typedef EBBRC (*EBBFunc) (void *);
typedef EBBFunc *EBBRep;

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

//DS: Here we have to leak in the lrt trans header to do this 
// translation
static inline 
EBBLTrans * 
EBBIdToLTrans(EBBId id) {
  return (EBBLTrans *)lrt_trans_id2lt((lrt_transid)id);
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

#endif
