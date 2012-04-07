#ifndef L0_LRT_BARE_TRANS_H
#define L0_LRT_BARE_TRANS_H

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

#ifdef ARCH_AMD64
#include <l0/lrt/bare/arch/amd64/trans.h>
#elif ARCH_PPC32
#include <l0/lrt/bare/arch/ppc32/trans.h>
#elif ARCH_PPC64
#include <l0/lrt/bare/arch/ppc64/trans.h>
#endif

#include <lrt/assert.h>

//memory translation assertions

//FIXME DS: These assertions cannot be static because they are not constant

/* STATIC_ASSERT(lrt_trans_gt2id(lrt_trans_gmem()) == lrt_trans_idbase(), */
/* 	       "beginning of global memory does not match first id"); */

/* STATIC_ASSERT(lrt_trans_id2gt(lrt_trans_idbase()) == lrt_trans_gmem(), */
/* 	       "first id does not match beginning of global memory"); */

/* STATIC_ASSERT(lrt_trans_id2lt(lrt_trans_idbase()) ==  */
/* 	      lrt_trans_lmem(), */
/* 	       "first id does not match beginning of local mem"); */

/* STATIC_ASSERT(lrt_trans_lt2id((struct lrt_trans *)lrt_trans_lmem()) ==  */
/* 	      lrt_trans_idbase(), */
/* 	       "beginning of local mem does not match first id"); */

/* STATIC_ASSERT(lrt_trans_gt2lt((struct lrt_trans *)lrt_trans_gmem()) == */
/* 	       lrt_trans_lmem(), */
/* 	       "global mem to lmem mismatch"); */

/* STATIC_ASSERT(lrt_trans_lt2gt((struct lrt_trans *)lrt_trans_lmem()) == */
/* 	       lrt_trans_gmem(), */
/* 	       "local mem to global mem mismatch"); */

#endif
