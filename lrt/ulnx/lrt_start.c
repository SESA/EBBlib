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
#include <l0/lrt/mem.h>
#include <l0/lrt/trans.h>

extern void l0_start(void);

static struct boot_args_t {
  intptr_t cores;
  volatile intptr_t cores_to_start;
} boot_args;


// first code to be runnining on an interrupt
void lrt_start(void)
{
  // check cores
  // start up another core, with the 
  fprintf(stderr, "%s: start pic id %ld!\n", __func__, lrt_pic_myid);
  if (boot_args.cores_to_start > 0) {
    while (__sync_fetch_and_add(&boot_args.cores_to_start, -1) > 0) {
      intptr_t core;
      core = lrt_pic_add_core();
      fprintf(stderr, "***%s: started core %lx!\n", __func__, core);
    }
  }

  lrt_mem_init();
  lrt_trans_init();
  l0_start();
}

//DS KLUDGE: I make main a weak symbol so that testing can overwrite the symbol
// but still link with all the code
__attribute__ ((weak)) 
int
main(int argc, char **argv) 
{
  boot_args.cores = 1;
  boot_args.cores_to_start = 0;
  if (argc>1) {
    boot_args.cores=atoi(argv[1]);
    boot_args.cores_to_start = boot_args.cores -1;
  }
  fprintf(stderr, "%s: start!\n", __func__);
  lrt_pic_init(lrt_start);
  return -1;
}
