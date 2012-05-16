#ifndef L0_SYS_TRANS_DEF_H
#define L0_SYS_TRANS_DEF_H

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

#include <stdint.h>

#include <l0/lrt/trans.h>

union EBBTransStruct {
  struct {
    union {
      uintptr_t v1;
      EBBRep *obj; //as a local entry
      EBBMissFunc mf; //as a global entry
    };
    union {
      uintptr_t v2;
      EBBFunc *ftable; //as a local entry (by default)
      EBBMissArg arg; //as a global entry
    };
    union {
      uintptr_t v3;
      EBBGTrans *free;
    };
    union {
      uintptr_t v4;
      uint64_t corebv;	// bitvector of cores where object is cached in translation table
			// note, object may have been accessed in larger set of nodes and
			// tranlated id may be on stack in other nodes
    };
  };
  struct lrt_trans padding;
};

#endif
