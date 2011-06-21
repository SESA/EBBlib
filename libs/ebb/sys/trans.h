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

#ifndef __EBB_TRANS_H__
#define __EBB_TRANS_H__

#define EBB_MAX_FUNCS 256

typedef EBBRC (*EBBFunc) (uval arg);
typedef EBBFunc EBBFuncTable[];

extern EBBFuncTable EBBDefFT[EBB_MAXFUNCS];
extern EBBFuncTable EBBNullFT[EBB_MAXFUNCS];

typedef struct EBBCallDesc {
  EBBFuncTable * funcs;
  uval           extra;
} EBBCallDesc;

typedef struct EBBTrans {
  EBBCallDesc fdesc;
  uval transVal;
} EBBTrans;

typedef EBBTrans EBBLTrans;
typedef EBBTrans EBBGTrans;

typedef struct EBBTransLSysStruct {
  EBBGTrans *gTable;
  EBBLTrans *lTable;
  EBBGTrans *free;
  uval numAllocated;
} EBBTransLSys;

typedef EBBTrans *EBBId;

#define EBBIdNull 0

static inline EBBLTrans * EBBIdToLTrans(EBBTransLSys *sys, EBBid id)
{
}

static inline EBBGTrans * EBBLTransToGTrans(EBBTransLSys *sys, EBBLTrans *lt)
{
}

static inline EBBGTrans * EBBIdToGTrans(EBBTransLSys *sys, EBBid id)
{
}

static inline EBBid * EBBIdAlloc(EBBTransLSys *sys)
{
}

static inline EBBIdFree(EBBTransLSys *sys, EBBid id)
{
}

static inline EBBIdBind(EBBTransLSys *sys, EBBid id, uval v1, uval v2)
{
}

static inline EBBIdUnBind(EBBTransLSys *sys, EBBid id, uval *v1, uval *v2)
{
}

#define EBBId_CALL(id, f, ...) ((*id)->fdesc.funcs[f](&id->fdesc))  

#endif
