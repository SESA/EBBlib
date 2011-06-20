#ifndef __EBB_EBB_TYPES_H__
#define __EBB_EBB_TYPES_H__

/**************************************************************/
/* An EBB is composed of two types of components:             */
/*    1) A single Root Component (then may change on          */
/*       multi-node systems)                                  */
/*    2) 0 or more Representatives                            */
/*                                                            */
/* These components are called EBBObjects (EBBObjs/objects)   */
/* An EBBObj is a data structure plus a calling convention    */
/* Various defintions for a EBBObj are possible but you must  */
/* make it compatiable with the EBB translation system        */
/*                                                            */
/* In particular in this file is a EBB "C" definition         */
/* Which defines an EBBObj to structure which has a pointer   */
/* to an EBBVtable and a pointer to an EBBData instance       */
/*                                                            */
/* A Root object is considered to be the EBBInstance (EBBInst)*/
/* To publish an instance in the EBB system you must allocate */
/* a EBBId and bind the instance to the EBBId                 */
/*                                                            */
/* A vtable is constructed from a set of interface definitions*/
/* which defines methods that take as their first paramter    */
/* a pointer to the object instance                           */
/* A primordial EBB called the EBBMgr provides an interface   */
/* for various methods such as allocId, bind, etc             */
/**************************************************************/

/***** EBB Stuff builds on object abstractions ******/

#include "lrt/ulnx/EBBTypes.h"

#define EBBMethod(method) EBBRC (*method)

typedef Obj     *EBBRootRef;
typedef Obj     *EBBRepRef;
typedef EBBRootRef  EBBInst;

// FIXME: JA some of these types might need to move around
typedef void * EBBId;
#define EBBNullId NULL

#define EBBCreateTypes(ebb)			\
  ObjCreateType(ebb);				\
  typedef ebb * ebb ## Id;                      \
  ObjCreateType(ebb ## Root)                           

typedef sval EBBRC;
typedef enum { EBBRC_OK = 0 } EBBRC_STDVALS;
#define EBBRC_SUCCESS(rc) ( rc >= 0 )

static inline EBBTrans * EBBIdToRepTrans(EBBId id)
{
  return ((EBBTrans *)id) + (EBBMyEL() * EBB_NUM_PRIMITIVE_EBBS);
}

static inline EBBRepRef EBBIdToRep(EBBId id)
{
  return (EBBRepRef)EBBIdToRepTrans(id);
}

#define INVK(ebbid, fun, ...)                          \
  ( OBJ_INVK((typeof(ebbid))EBBIdToRep(ebbid), fun, ##__VA_ARGS__); )


#endif
