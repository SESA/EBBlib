/*
 * Copyright (C) 2012 by Project SESA, Boston University
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
#include <stdlib.h>
#include <assert.h>
#include <l0/lrt/trans.h>

struct lrt_trans_mem_desc lrt_trans_mem;

// get the base address of a remote local memory translation table
static lrt_trans_ltrans *
lrt_trans_lmemr(lrt_event_loc el)
{
  ptrdiff_t index = el * LRT_TRANS_TBLSIZE / sizeof(lrt_trans_ltrans);
  return lrt_trans_mem.lmem + index;
}

// invalidate remote entry
void
lrt_trans_invalidate_rltrans(lrt_event_loc el, lrt_trans_id oid)
{
  lrt_trans_ltrans *lmem = lrt_trans_lmemr(el);
  ptrdiff_t index = oid - lrt_trans_idbase();
  lrt_trans_ltrans *lt = lmem + index;
  lt->ref = &lt->rep;
  lt->rep = lrt_trans_def_rep;
}

void
lrt_trans_specific_init()
{
}

void
lrt_trans_preinit(int cores)
{
  lrt_trans_mem.gmem = malloc(LRT_TRANS_TBLSIZE);
  assert(lrt_trans_mem.gmem);
  lrt_trans_mem.lmem = malloc(LRT_TRANS_TBLSIZE * cores);
  assert(lrt_trans_mem.lmem);
}
