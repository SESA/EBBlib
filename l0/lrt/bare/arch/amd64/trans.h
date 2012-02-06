#ifndef L0_LRT_BARE_ARCH_AMD64_TRANS_H
#define L0_LRT_BARE_ARCH_AMD64_TRANS_H

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

#include <arch/amd64/paging.h>

#define LRT_TRANS_PGSIZE (LARGE_PAGE_SIZE) //2 MB
#define LRT_TRANS_PAGES (1)
#define LRT_TRANS_TBLSIZE (LRT_TRANS_PGSIZE * LRT_TRANS_PAGES) 

//These should be virtual addresses
#define GMem (0xFFFFFFFF00000000) //upper 4GB of memory
#define LMem (0xFFFFFFFE00000000) //next 4GB of memory

extern void lrt_trans_init(void);

#endif
