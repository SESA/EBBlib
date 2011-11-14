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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <l0/lrt/pic.h>


extern void EBBStart(void);

// add these to where they should go and implement them!
void
lrt_mem_init(void)
{
  fprintf(stderr, "%s: NYI\n", __func__);
}

void
lrt_trans_init(void)
{
  fprintf(stderr, "%s: NYI\n", __func__);
}

// first code to be runnining on an interrupt
void lrt_start(void)
{
  fprintf(stderr, "%s: start!\n", __func__);
  lrt_mem_init();
  lrt_trans_init();
  EBBStart();
}

int
main(int argc, char **argv)
{
  uintptr_t cores=1;

  fprintf(stderr, "%s: start!\n", __func__);
  if (argc>1) cores=atoi(argv[1]);
  lrt_pic_init(cores, lrt_start);
  return -1;
}
