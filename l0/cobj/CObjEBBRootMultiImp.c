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
#include <config.h>

#include <stdint.h>
#include <stddef.h>

#include <lrt/assert.h>
#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EventMgrPrim.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>

struct RepListNode_s {
  EBBRep *rep;
  uintptr_t el;
  RepListNode *next;
};

static inline void
lockReps(CObjEBBRootMultiImpRef self)
{
  while (__sync_bool_compare_and_swap(&self->lock, 0, 1));
}

static inline void
unlockReps(CObjEBBRootMultiImpRef self)
{
  EBBAssert(__sync_bool_compare_and_swap(&self->lock, 1, 0)==1);
}

static EBBRep *
locked_FindRepOn(CObjEBBRootMultiImpRef self, uintptr_t el)
{
  EBBRep *rep=NULL;
  struct RepListNode_s *rd;

  EBBAssert(self->lock!=0);
  rd = self->head;
  while (rd && rd->el == el) rep=rd->rep;
  return rep;
}

static void 
locked_AddRepOn(CObjEBBRootMultiImpRef self, uintptr_t el, EBBRep *rep)
{
  struct RepListNode_s *rd;
  EBBRC rc;

  EBBAssert(self->lock!=0);
  rc = EBBPrimMalloc(sizeof(struct RepListNode_s), &rd,
		     EBB_MEM_GLOBAL);
  EBBRCAssert(rc);
  rd->rep = rep;
  rd->el = el;
  rd->next = self->head;
  self->head = rd;
}

static
void
CObjEBBRootMulti_addRepOn(CObjEBBRootMultiRef _self, uintptr_t el, EBBRep *rep)
{
  CObjEBBRootMultiImpRef self = (CObjEBBRootMultiImpRef)_self;
  lockReps(self);
  locked_AddRepOn(self, el, rep);
  unlockReps(self);
}

static
EBBRC
CObjEBBRootMulti_handleMiss(CObjEBBRootRef _self, EBBRep **obj, EBBLTrans *lt,
			    FuncNum fnum)
{
  CObjEBBRootMultiImpRef self = (CObjEBBRootMultiImpRef)_self;
  uintptr_t myel = myEL();
  EBBRep *rep;

  lockReps(self);
  rep = locked_FindRepOn(self, myel);
  if (rep==NULL) {
    rep = self->createRep((CObjEBBRootMultiRef)self);
    locked_AddRepOn(self, myel, rep);
  }
  unlockReps(self);
  EBBCacheObj(lt, rep);
  *obj = rep;
  return EBBRC_OK;
}


static
RepListNode *
CObjEBBRootMulti_nextRep(CObjEBBRootMultiRef _self, RepListNode *curr, 
			 EBBRep **rep)
{
  EBBRCAssert(0);
  return NULL;
}

static
CObjInterface(CObjEBBRootMulti) CObjEBBRootMulti_ftable = {
  { .handleMiss = CObjEBBRootMulti_handleMiss },
  .addRepOn = CObjEBBRootMulti_addRepOn,
  .nextRep = CObjEBBRootMulti_nextRep
};

static inline void
CObjEBBRootMultiSetFT(CObjEBBRootMultiImpRef o)
{
  o->ft = &CObjEBBRootMulti_ftable;
}

static void
CObjEBBRootMultiImpInit(CObjEBBRootMultiImpRef o, CreateRepFunc func)
{
  CObjEBBRootMultiSetFT(o);
  o->createRep = func;
  o->head = NULL;
  o->lock = 0;
}

EBBRC
CObjEBBRootMultiImpStaticInit(CObjEBBRootMultiImpRef o, CreateRepFunc func)
{
  CObjEBBRootMultiImpInit(o, func);
  return EBBRC_OK;
}

EBBRC
CObjEBBRootMultiImpCreate(CObjEBBRootMultiImpRef *o, CreateRepFunc func)
{
  EBBRC rc;
  rc = EBBPrimMalloc(sizeof(CObjEBBRootMultiImp), o, EBB_MEM_GLOBAL);
  if (EBBRC_SUCCESS(rc)) CObjEBBRootMultiImpInit(*o, func);
  return rc;
}
