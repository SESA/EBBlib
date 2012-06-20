#ifndef __EBB_EVENT_MGR_PRIM_EXP_H__
#define __EBB_EVENT_MGR_PRIM_EXP_H__
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

#include <l0/EventMgrPrim.h>

COBJ_EBBType(EventMgrPrimExp) {
  CObjImplements(EventMgrPrim);
  EBBRC (*enableBitvectorLocal) (EventMgrPrimExpRef self);
  EBBRC (*disableBitvectorLocal) (EventMgrPrimExpRef self);
  EBBRC (*enableBitvectorRemote) (EventMgrPrimExpRef self);
  EBBRC (*disableBitvectorRemote) (EventMgrPrimExpRef self);
  EBBRC (*enableBlock) (EventMgrPrimExpRef self);
  EBBRC (*enablePoll) (EventMgrPrimExpRef self);
};

extern EBBRC EventMgrPrimExpInit(void);
#endif
