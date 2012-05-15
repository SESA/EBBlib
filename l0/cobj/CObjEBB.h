#ifndef __COBJ_EBB_H__
#define __COBJ_EBB_H__
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

#include <l0/lrt/trans.h>
#include <l0/cobj/cobj.h>

extern EBBRC CObjEBBMissFunc(EBBRep **, EBBLTrans *, FuncNum, EBBMissArg);

#define COBJ_EBBCALL(id, method, ...) \
  (EBBId_DREF(id)->ft->method(EBBId_DREF(id), ##__VA_ARGS__))

#define COBJ_EBBCALL_FUNCNUM(Type, id, funcNum, ...)	\
  ( ((Type)COBJ_FUNC(EBBId_DREF(id), funcNum))(EBBId_DREF(id), ##__VA_ARGS__) )
 
// Introduces a new CObject Ebb type -> generates all associated cobject types
#define COBJ_EBBType(TYPE)			\
  CObject(TYPE);				\
  typedef TYPE ## Ref *TYPE ## Id;		\
  CObjectDefine(TYPE) {				\
    CObjInterface(TYPE) *ft;			\
  };						\
  CObjInterface(TYPE)

COBJ_EBBType(EBBBase) {};
 
// given a EBB Type will generate the appropriate cobject function table pointer
#define COBJ_EBBFuncTbl(TYPE) CObjInterface(TYPE) *ft;

/*
#define COBJ_EBBImp_START(TYPE,IMP)		\
  CObject(TYPE ## IMP) {			\
  CObjInterface(TYPE) *ft;			\
  struct 

#define COBJ_EBBImp_END ; }
*/
#endif
