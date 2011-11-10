#include <config.h>
#include <types.h>
#include <core/cobj/cobj.h>
#include <core/types.h>
#include <core/cobj/CObjEBB.h>
#include <core/cobj/CObjEBBRoot.h>
#include <core/cobj/CObjEBBRootMulti.h>
#include <core/MemMgr.h>
#include <core/MemMgrPrim.h>

struct RepListNode_s {
  void *rep;
  RepListNode *next;
};

uval
CObjEBBRootMulti_handleMiss(void *_self, void*obj, EBBLTrans *lt,
			    FuncNum fnum)
{
  CObjEBBRootMultiRef self = _self;

  void *rep = self->createRep(self);
  EBBCacheObj(lt, rep);
  *(void **)obj = rep;

  RepListNode *p;
  EBBPrimMalloc(sizeof(RepListNode), &p, EBB_MEM_DEFAULT);
  p->rep = rep;
  //FIXME: lock the list
  p->next = self->head;
  self->head = p;
  return EBBRC_OK;
}

void
CObjEBBRootMulti_init(void *_self, CreateRepFunc func)
{
  CObjEBBRootMultiRef self = _self;
  self->createRep = func;
  self->head = NULL;
}

RepListNode *
CObjEBBRootMulti_nextRep(void *_self, RepListNode *curr, void *rep)
{
  CObjEBBRootMultiRef self = _self;
  RepListNode *node;
  //FIXME: lock the list
  if(!curr) {
    node = self->head;
  } else {
    node = curr->next;
  }
  if (node) {
    *((void **)rep) = node->rep;
  }
  return node;
}


CObjInterface(CObjEBBRootMulti) CObjEBBRootMulti_ftable = {
  { .handleMiss = CObjEBBRootMulti_handleMiss },
  .init = CObjEBBRootMulti_init,
  .nextRep = CObjEBBRootMulti_nextRep
};
