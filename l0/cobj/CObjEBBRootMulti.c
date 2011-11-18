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
#include <l0/cobj/cobj.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>

struct RepListNode_s {
  void *rep;
  RepListNode *next;
};

EBBRC
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
