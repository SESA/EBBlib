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
#include <strings.h>
#include <assert.h>

#include <l0/lrt/pic.h>
#include <l0/lrt/mem.h>
#include <l0/lrt/trans.h>

extern void l0_start(uintptr_t startinfo);

static struct start_args_t {
  intptr_t cores;
  volatile intptr_t cores_to_start;
  uintptr_t start_info;
  intptr_t start_info_size;
} start_args;

enum { LRT_STARTINFO_SIZE=4096 };

extern uintptr_t
lrt_startinfo(void)
{
  return start_args.start_info;
}

extern intptr_t
lrt_startinfo_size(void)
{
  return start_args.start_info_size;
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

__attribute__ ((unused)) static void
dumpstartargs(void)
{
  uintptr_t s;
  int argc, i;
  char *data = (char *)start_args.start_info;
  
  fprintf(stderr, "%s: start_args.start_info_size=%" PRIdPTR "\n",
	  __func__,
	  start_args.start_info_size);

  if (start_args.start_info_size) {
    assert(start_args.start_info_size >= sizeof(int));
    argc = *((int *)data); data += sizeof(int);
    s = sizeof(int);
    for (i=0; i<argc; i++) {
      fprintf(stderr, "argv[%d]=%s\n", i, data);
      while (*data != '\0') { data++; s++; }
      data++; s++;
    }
    i=0;
    while (s<start_args.start_info_size) {
      fprintf(stderr, "environ[%d]=%s\n", i, data);
      while (*data != '\0') { data++; s++; }
      data++; s++; i++;
    }
    assert(s==start_args.start_info_size);
  }
}

void
startinfo(int argc, char **argv, char **environ, 
	  uintptr_t *addr, intptr_t *size) 
{
  char *data, *cur;
  int s, i;

  s = sizeof(int); // add bytes for argc
  for (i=0; i<argc; i++) {
    s += strlen(argv[i]); 
    s++; // add one for nul
  }

  for (i=0; environ[i]!=0; i++) {
    s += strlen(environ[i]);
    s++; // add one for nul
  }

  data = (char *)malloc(s);
  assert(data);
  cur = data;

  // first bytes are for argc
  *((int *)cur) = argc;
  cur+=sizeof(int);

  // followed by argv data
  for (i=0; i<argc; i++) {
    cur = stpcpy(cur, argv[i]);
    cur++;
  }

  // followed by environment data
  for (i=0; environ[i]!=0; i++) {
    cur = stpcpy(cur, environ[i]);
    cur++;
  }

  assert(s==(cur-data));

  *addr = (uintptr_t)data;
  *size = (uintptr_t)s;
}

//DS KLUDGE: I make main a weak symbol so that testing can overwrite the symbol
// but still link with all the code
__attribute__ ((weak)) 
int
main(int argc, char **argv, char **environ) 
{
  fprintf(stderr, "%s: start!\n", __func__); 

  start_args.cores = 1;
  start_args.cores_to_start = 0;
  start_args.start_info = 0;
  start_args.start_info_size = 0;
  
  if (argc>1) {
    start_args.cores=atoi(argv[1]);
    start_args.cores_to_start = start_args.cores -1;
  }

  startinfo(argc, argv, environ,
	    &start_args.start_info, &start_args.start_info_size);

  //  dumpstartargs();
#ifdef LRT_STANDALONE_TEST
  LRT_STANDALONE_TEST
#else
  lrt_pic_init(lrt_start);
#endif
  return -1;
}
