#ifndef __EBB_TRANS_H__
#define __EBB_TRANS_H__

#include "../../types.h"
#include "trans.h"

EBBFunc EBBDefFT[EBB_MAX_FUNCS];

#define EBBDefFunc(i)						\
  static EBBRC							\
  EBBDefFunc_##i (EBBLTrans *self) {				\
    EBBGTrans *gt = EBBLTransToGTrans(myEbbTransLSys, self);	\
    EBBDefFunc f = (EBBDefFunc)gt->fdesc.funcs;			\
    return f(self, i);						\
  }								

#endif
