#ifndef __EBBMGRPRIM_H__
#define __EBBMGRPRIM_H__
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

CObjInterface(EBBMgrPrim) {
  EBBRC (*AllocId) (void *_self, void **id);
  EBBRC (*FreeId) (void *_self, EBBId id);
  EBBRC (*BindId) (void *_self, EBBId id, EBBMissFunc mf, EBBMissArg arg);
  EBBRC (*UnBindId) (void *_self, EBBId id, EBBMissFunc *mf, EBBMissArg *arg);
};

CObject(EBBMgrPrim) {
  CObjInterface(EBBMgrPrim) *ft;
  //JA Hack
  void *myRoot;
  EBBTransLSys *lsys;
};


extern void *NULLId;
typedef EBBMgrPrimRef *EBBMgrPrimId;
extern EBBMgrPrimId theEBBMgrPrimId;
extern void EBBMgrPrimInit(void);

//FIXME: JA fix typing here on id
static inline EBBRC
EBBAllocPrimId(void *id)
{
  return EC(theEBBMgrPrimId)->AllocId(EB(theEBBMgrPrimId), id);
}

static inline EBBRC
EBBBindPrimId(void *id, EBBMissFunc mf, EBBMissArg arg)
{  
  return EC(theEBBMgrPrimId)->BindId(EB(theEBBMgrPrimId), 
				     (EBBId)id, mf, arg);
}


#endif
