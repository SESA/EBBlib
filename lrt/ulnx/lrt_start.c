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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

#include <l0/lrt/pic.h>
#include <l0/lrt/mem.h>
#include <l0/lrt/trans.h>

extern void l0_start(uintptr_t startinfo);

static struct start_args_t {
  intptr_t cores;
  volatile intptr_t cores_to_start;
  uintptr_t startinfo;
} start_args;

enum { LRT_STARTINFO_SIZE=4096 };

extern uintptr_t
lrt_startinfo(void)
{
  return start_args.startinfo;
}

extern intptr_t
lrt_startinfo_size(void)
{
  return LRT_STARTINFO_SIZE;
}

// first code to be runnining on an interrupt
void lrt_start(void)
{
  // check cores
  // start up another core, with the 
  fprintf(stderr, "%s: start pic id %" PRIuPTR "!\n", __func__, lrt_pic_myid);
  if (start_args.cores_to_start > 0) {
    while (__sync_fetch_and_add(&start_args.cores_to_start, -1) > 0) {
      intptr_t core;
      core = lrt_pic_add_core();
      fprintf(stderr, "***%s: started core %" PRIxPTR "!\n", __func__, core);
    }
  }

  // JA: CONFUSED WHY IS this independent of lrt_pic_add_core?
  lrt_mem_init();
  lrt_trans_init();
  l0_start(lrt_startinfo());
}

uintptr_t
startinfo(char *fn) 
{
  static char lrt_startinfo[LRT_STARTINFO_SIZE];
  int fd, n;
  uintptr_t addr = 0;

  fd=open(fn, O_RDONLY);
  if (fd == -1) fprintf(stderr, "%s: error opening startinfo fd=%d (%s)\n",
			__func__, fd, fn);
  else {
    n = read(fd, lrt_startinfo, LRT_STARTINFO_SIZE);
    if (n<0) fprintf(stderr, "%s: error reading startinfo from fd=%d (%s)\n",
		     __func__, fd, fn);
    else {
      fprintf(stderr, "%s: startinfo: %d bytes read from %s\n", __func__,
	      n, fn);
    }
    close(fd);
    addr = (uintptr_t)lrt_startinfo;
  }
  return addr;
}

//DS KLUDGE: I make main a weak symbol so that testing can overwrite the symbol
// but still link with all the code
__attribute__ ((weak)) 
int
main(int argc, char **argv) 
{
  start_args.cores = 1;
  start_args.cores_to_start = 0;
  start_args.startinfo = 0;

  if (argc>1) {
    start_args.cores=atoi(argv[1]);
    start_args.cores_to_start = start_args.cores -1;
  }

  if (argc==3) {
    start_args.startinfo = startinfo(argv[2]);
  }

  fprintf(stderr, "%s: start!\n", __func__); 
  lrt_pic_init(lrt_start);
  return -1;
}
