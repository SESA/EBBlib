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

/*
 * The purpose of this file is to define function versions of the inlined 
 * routines that are common to all platforms for translation.  This basically gives
 * us a compile check that the prototypes are all the same, and makes clear what 
 * functions all the platform specific files should define
 */

#include <config.h>
#include <stdint.h>
#include <l0/lrt/trans.h>

struct lrt_trans *lrt_trans_id2lt_func(uintptr_t i)
{
  return lrt_trans_id2lt(i);
}
uintptr_t lrt_trans_lt2id_func(struct lrt_trans *t)
{
  return lrt_trans_lt2id(t);
}
struct lrt_trans *lrt_trans_id2gt_func(uintptr_t i)
{
  return lrt_trans_id2gt(i);
}
uintptr_t lrt_trans_gt2id_func(struct lrt_trans *t)
{
  return lrt_trans_gt2id(t);
}
struct lrt_trans *lrt_trans_gt2lt_func(struct lrt_trans *gt)
{
  return lrt_trans_gt2lt(gt);
}
struct lrt_trans *lrt_trans_lt2gt_func(struct lrt_trans *lt)
{
  return lrt_trans_lt2gt(lt);
}

