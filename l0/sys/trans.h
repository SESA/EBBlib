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

typedef EBBTrans EBBGTrans;

extern EBBFunc EBBDefFT[EBB_TRANS_MAX_FUNCS];

#ifdef __cplusplus
extern "C" {
#endif
extern void trans_init(void);
#ifdef __cplusplus
}
#endif

#if 0
extern uintptr_t myGTableSize(void);
extern EBBGTrans *myGTable(void);
extern uintptr_t myLTableSize(void);
extern EBBLTrans *myLTable(void);
extern void EBBInitGTrans(EBBGTrans *gt, EBBMissFunc mf, EBBMissArg arg);
extern void EBBInitLTrans(EBBLTrans *lt);
extern void initGTable(EBBMissFunc mf, EBBMissArg arg);
extern void initLTable(void);

extern EBBId TransEBBIdAlloc(void);
extern void TransEBBIdFree(EBBId id);
extern void TransEBBIdBind(EBBId id, EBBMissFunc mf, EBBMissArg arg);
extern void TransEBBIdUnBind(EBBId id, EBBMissFunc *mf, EBBMissArg *arg); 
#endif

#endif
