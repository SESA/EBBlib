#ifndef __EBB_TYPES_H__
#define __EBB_TYPES_H__

#include "EBBConst.h"
#include "lrt/ulnx/EBBTypes.h"

typedef struct EBBTransStruct EBBTrans;
typedef EBBTrans EBBLTrans;
typedef EBBTrans EBBGTrans;

typedef EBBTrans *EBBId;
#define EBBNullId NULL;

typedef struct EBBTransLSysStruct EBBTransLSys;

typedef sval EBBRC;
typedef enum { EBBRC_GENERIC_FAILURE = -1, EBBRC_OK = 0 } EBBRC_STDVALS;
#define EBBRC_SUCCESS(rc) ( rc >= 0 )

typedef uval FuncNum;
typedef uval EBBMissArg;

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

static inline uval EBBMyEL() {
  return LRTEBBMyEL();
}

static inline uval EBBMyLTransIndex() {
  return EBBMyEL();
}

static inline EBBLTrans * EBBIdToSpecificLTrans(EBBId id, uval i) {
  return (EBBLTrans *)((uval)id + i *
		       EBB_TRANS_PAGE_SIZE * EBB_TRANS_NUM_PAGES);
}

static inline EBBLTrans * EBBIdToLTrans(EBBId id) {
  return EBBIdToSpecificLTrans(id, EBBMyLTransIndex());
}

#define EBBId_DREF(id) ((typeof(*id))(*(void **)EBBIdToLTrans((EBBId)id)))

#endif
