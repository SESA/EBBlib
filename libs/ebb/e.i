# 1 "EBBMgrPrimitive.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "EBBMgrPrimitive.c"
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
# 2 "EBBMgrPrimitive.c" 2
# 1 "EBBConst.h" 1



# 1 "lrt/ulnx/EBBConst.h" 1
# 5 "EBBConst.h" 2
# 3 "EBBMgrPrimitive.c" 2
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
# 4 "EBBMgrPrimitive.c" 2
# 1 "EBBTypes.h" 1
# 30 "EBBTypes.h"
# 1 "lrt/ulnx/EBBTypes.h" 1



static inline EL LRTEBBMyEL(void) { return 0; }
# 31 "EBBTypes.h" 2

typedef uval EBBMethodNum;
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
# 72 "EBBTypes.h"
typedef sval EBBRC;
typedef enum { EBBRC_OK = 0 } EBBRC_STDVALS;


static inline EBBTrans * EBBIdToRepTrans(EBBId id)
{
  return ((EBBTrans *)id) + (EBBMyEL() * (16 * 128));
}

static inline EBBRepRef EBBIdToRep(EBBId id)
{
  return (EBBRepRef)EBBIdToRepTrans(id);
}
# 5 "EBBMgrPrimitive.c" 2
# 1 "EBBAssert.h" 1



# 1 "lrt/ulnx/EBBAssert.h" 1



# 1 "/usr/include/assert.h" 1 3 4
# 42 "/usr/include/assert.h" 3 4
# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 43 "/usr/include/assert.h" 2 3 4
# 75 "/usr/include/assert.h" 3 4

void __assert_rtn(const char *, const char *, int, const char *) __attribute__((__noreturn__));
void __eprintf(const char *, const char *, unsigned, const char *) __attribute__((__noreturn__));

# 5 "lrt/ulnx/EBBAssert.h" 2
# 5 "EBBAssert.h" 2
# 6 "EBBMgrPrimitive.c" 2
# 1 "EBBMgr.H" 1



typedef struct EBBMgrPrimitiveObj { struct EBBMgrPrimitiveVTable *vtable; struct EBBMgrPrimitiveData *data; } EBBMgrPrimitive; typedef EBBMgrPrimitive * EBBMgrPrimitiveId; typedef struct EBBMgrPrimitiveRootObj { struct EBBMgrPrimitiveRootVTable *vtable; struct EBBMgrPrimitiveRootData *data; } EBBMgrPrimitiveRoot;

struct EBBMgrPrimitiveVTable {
# 1 "EBBMgr.iface" 1


EBBRC (*init) (void);
EBBRC (*allocId) (EBBId *id);
EBBRC (*bind) (EBBId id, EBBObj o);
EBBRC (*destroy) (EBBId id);
# 8 "EBBMgr.H" 2
};

extern EBBMgrPrimitiveId theEBBMgrPrimitiveId;

static inline EBBMgrPrimitiveId TheEBBMgrPrimitiveId(void)
{
  return theEBBMgrPrimitiveId;
}

extern EBBRC EBBMgrInit(void);
# 7 "EBBMgrPrimitive.c" 2
# 17 "EBBMgrPrimitive.c"
typedef enum
{ NumPrimitiveEBBS = (16 * 128), NumELS=16 }
EBBMgrConst;

EBBTrans PrimitiveGTrnsTbl[NumPrimitiveEBBS];
EBBTrans PrimitiveLTrnsTbls[NumPrimitiveEBBS * NumELS];

EBBMgrPrimitiveId theEBBMgrPrimitiveId;





struct EBBMgrPrimitiveRootVTable {
# 1 "EBBRoot.iface" 1
EBBRC (*getRep) (ELNum el, EBBRepRef *rep);
EBBRC (*handleTransFault) (EBBTrans *t, EBBMethodNum mnum);
# 32 "EBBMgrPrimitive.c" 2
} theRootVTable;

struct EBBMgrPrimitiveRootData {
  EBBTrans *rootTable;
  EBBTrans *repTables;
  EBBTransSys transSys;

  struct EBBMgrPrimitiveRepData {
    struct EBBMgrPrimitiveRootData *myRoot;
    EBBTrans *myPrimitiveRootTable;
    EBBTrans *myPrimitiveRepTable;
  } repData[NumELS];
  EBBMgrPrimitive reps[NumELS];
} theRootData = { PrimitiveGTrnsTbl, PrimitiveLTrnsTbls,
    { PrimitiveGTrnsTbl, PrimitiveLTrnsTbls }};
typedef uval EBBIndex;

static inline EBBObj * TransToObj(EBBTrans *t)
{
  return (EBBObj *)t;
}

static inline EBBTrans * ObjToTrans(EBBObj *o)
{
  return (EBBTrans *)o;
}

static inline EBBIndex IdToIdx(EBBId id)
{
  return 0;
}

static inline EBBId IdxToId(EBBIndex idx)
{
  return (void *)(0);
}

static inline EBBTrans * IdToRootTrans(EBBId id)
{
  return theRootData.rootTable + IdToIdx(id);
}


static inline EBBRootRef IdToRootRef(EBBId id)
{
  return TransToObj(IdToRootTrans(id));
}

static inline installObj(EBBTrans *t, EBBObj *o)
{
  EBBObj *eo = TransToObj(t);
  *eo = *o;
}

static inline bindRoot(EBBId id, EBBRootRef r)
{
  EBBTrans *t = IdToRootTrans(id);
  installObj(t, r);
}

static inline bindRep(EBBId id, EBBRepRef r)
{
  EBBTrans *t = EBBIdToRepTrans(t);
  installObj(t, r);
}

static inline EBBRC bind(EBBId id, EBBInst inst)
{
  EBBRootRef root = inst;

  bindRoot(id, root);
# 114 "EBBMgrPrimitive.c"
}

static inline EBBRC allocId(EBBId *id) {
  *id = &(theRootData.repTables[0]);
  return EBBRC_OK;
}

static EBBRC
EBBMgrCreateInst(EBBInst *inst)
{
  static EBBObj theRoot = { &theRootVTable,
       &theRootData };
  *inst = &theRoot;
  return EBBRC_OK;
}

extern EBBRC
EBBMgrInit(void)
{
  EBBRC rc;
  EBBInst ebbmgr;



  (__builtin_expect(!(sizeof(EBBObj) == sizeof(EBBTrans)), 0) ? __assert_rtn(__func__, "EBBMgrPrimitive.c", 138, "sizeof(EBBObj) == sizeof(EBBTrans)") : (void)0);


  rc = EBBMgrCreateInst(&ebbmgr);
  (__builtin_expect(!(( rc >= 0 )), 0) ? __assert_rtn(__func__, "EBBMgrPrimitive.c", 142, "EBBRC_SUCCESS(rc)") : (void)0);


  rc = allocId((EBBId *)&theEBBMgrPrimitiveId);
  (__builtin_expect(!(( rc >= 0 )), 0) ? __assert_rtn(__func__, "EBBMgrPrimitive.c", 146, "EBBRC_SUCCESS(rc)") : (void)0);

  rc = bind(theEBBMgrPrimitiveId, ebbmgr);
  (__builtin_expect(!(( rc >= 0 )), 0) ? __assert_rtn(__func__, "EBBMgrPrimitive.c", 149, "EBBRC_SUCCESS(rc)") : (void)0);



  rc = ( ({ typeof((typeof(TheEBBMgrID()))EBBIdToRep(TheEBBMgrID())) _obj = ((typeof(TheEBBMgrID()))EBBIdToRep(TheEBBMgrID())); _obj->vtable->init(_obj->data); }); );

  return rc;
}
