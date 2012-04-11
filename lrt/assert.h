#ifndef __EBB_ASSERT_H__
#define __EBB_ASSERT_H__
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

#ifdef LRT_ULNX
#include <lrt/ulnx/assert.h>
#elif LRT_BARE
#include <lrt/bare/assert.h>
#endif

#define EBBAssert LRT_EBBAssert
#define EBBWAssert(exp)  \
  ((void) ((exp) ? 0 : printf ("%s:%u: warning assert `%s'\n", __FILE__, __LINE__, #exp)))
#define EBBRCAssert(rc) EBBAssert(EBBRC_SUCCESS(rc))

#ifdef __cplusplus
  #if HAS_CXX_STATIC_ASSERT
    #define STATIC_ASSERT static_assert
  #else
    #define STATIC_ASSERT(b,s)
  #endif
#else //C
  #if HAS_C_STATIC_ASSERT
    #define STATIC_ASSERT _Static_assert
  #else
    #define STATIC_ASSERT(b,s)
  #endif
#endif

#endif
