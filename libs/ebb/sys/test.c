#include "../../types.h"
#include "trans.h"

EBBFunc EBBDefFT[EBB_MAX_FUNCS];
EBBFunc EBBNullFT[EBB_MAX_FUNCS];

EBBTransLSys myEBBTransLSys;
EBBGTrans theGTable[EBB_NUM_IDS];
EBBLTrans theLTable[EBB_NUM_ELS][EBB_NUM_IDS];

static EBBRC 
EBBNullFunc () {
  return EBBRC_FAILURE;
}

static EBBRC
EBBDefFunc () {
  return EBBRC_FAILURE;
}

void MyEBBIdBind () {
  

int main () {
  myEBBTransLSys.gTable = theGTable;
  myEBBTransLSys.lTable = theLTable[myEL()];
  myEBBTransLSys.free = theGTable;
  myEBBTransLSys.numAllocated = 0;
  int i, j;
  for (i = 0; i < EBB_MAX_FUNCS; i++) {
    EBBDefFT[i] = EBBDefFunc;
    EBBNullFT[i] = EBBNullFunc;
  }
  for (i = 0; i < EBB_NUM_IDS; i++) {
    theGTable[i].fdesc.funcs = 
      (EBBFuncTable *)&theGTable[i].fdesc.extra;
    theGTable[i].fdesc.extra = (uval)EBBNullFT;
    for (j = 0; j < EBB_NUM_ELS; j++) {
      theLTable[j][i].fdesc.funcs = 
	(EBBFuncTable *)&theLTable[j][i].fdesc.extra;
      theLTable[j][i].fdesc.extra = (uval)EBBDefFT;
    }
    if (i < (EBB_NUM_IDS-1)) {
      theGTable[i].transVal = (uval)&theGTable[i+1];
    } else {
      theGTable[i].transVal = (uval)NULL;
    }
  }

  return 0;
}
