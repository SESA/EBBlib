# 1 "test.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "test.c"
# 1 "../types.h" 1
# 33 "../types.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned long uval;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef long sval;
# 52 "../types.h"
typedef uval EL;
typedef uval ELNum;
# 2 "test.c" 2
# 1 "EBBConst.h" 1



# 1 "lrt/ulnx/EBBConst.h" 1
# 5 "EBBConst.h" 2
# 3 "test.c" 2
# 1 "sys/trans.h" 1
# 30 "sys/trans.h"
typedef struct EBBTransStruct {
  uval v1;
  uval v2;
} EBBTrans;

typedef struct EBBTransSysStruct {
  EBBTrans *gTable;
  EBBTrans *lTable;
} EBBTransSys;
# 4 "test.c" 2
# 1 "EBBTypes.h" 1
# 30 "EBBTypes.h"
# 1 "lrt/ulnx/EBBTypes.h" 1



static inline EL LRTEBBMyEL(void) { return 0; }
# 31 "EBBTypes.h" 2

typedef void * EBBVtable;
typedef void * EBBData;

typedef struct {
    EBBVtable vtable;
    EBBData data;
} EBBObj;

typedef EBBObj *EBBRootRef;
typedef EBBObj *EBBRepRef;
typedef EBBRootRef EBBInst;


typedef void * EBBId;
# 65 "EBBTypes.h"
typedef sval EBBRC;
typedef enum { EBBRC_OK = 0 } EBBRC_STDVALS;



static inline EL EBBMyEL(void) { return LRTEBBMyEL(); }

static inline EBBTrans * EBBIdToRepTrans(EBBId id)
{
  return ((EBBTrans *)id) + (EBBMyEL() * (16 * 128));
}

static inline EBBRepRef EBBIdToRep(EBBId id)
{
  return (EBBRepRef)EBBIdToRepTrans(id);
}
# 5 "test.c" 2
# 1 "EBBMgr.H" 1



extern EBBRC EBBMgrInit(void);

typedef struct EBBMgrPrimitiveObj { 
struct EBBMgrPrimitiveVTable *vtable; 
struct EBBMgrPrimitiveData *data; 
} EBBMgrPrimitive; 
typedef EBBMgrPrimitive * EBBMgrPrimitiveId; 
typedef struct EBBMgrPrimitiveRoot Obj 
{ struct EBBMgrPrimitiveRootVTable *vtable; struct EBBMgrPrimitiveRootData *data; } EBBMgrPrimitiveRoot;

extern EBBMgrPrimitiveId theEBBMgrPrimitiveId;

static inline EBBMgrPrimitiveId TheEBBMgrPrimitiveId(void)
{
  return theEBBMgrPrimitiveId;
}
# 6 "test.c" 2

int
main(int argc, char **argv)
{
  (void) EBBMgrInit();

  return 0;
}
