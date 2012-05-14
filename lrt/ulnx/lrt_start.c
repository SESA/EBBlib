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

#define _GNU_SOURCE //need this for stpcpy
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#if __APPLE__
#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#include <mach/kern_return.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#include <l0/l0_start.h>
#include <l0/lrt/mem.h>
#include <l0/lrt/trans.h>
#include <l0/lrt/event.h>


static struct start_args_t {
  intptr_t physcores;		/* actual physical cores we have */
  intptr_t cores;		/* logical cores we are starting */
  uintptr_t start_info;
  intptr_t start_info_size;
} start_args;

enum { LRT_STARTINFO_SIZE=4096 };

extern uintptr_t
lrt_startinfo(void)
{
  return start_args.start_info;
}

char *
lrt_startinfo_addr(void)
{
  return (char *)start_args.start_info;
}

extern intptr_t
lrt_startinfo_size(void)
{
  return start_args.start_info_size;
}

// first code to be runnining on an interrupt
void lrt_start(void)
{
  // event initialization done before we got here
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

static int
parse_ebbos_arg(int i, char **argv, int *s)
{
  const char *ebbos_cores = "-ebbos_cores";
  int ret=1;

  if (strncmp(argv[i], ebbos_cores, strlen(ebbos_cores))==0) {
    // overridding the number of cores
    ret = 2;			/* 2 arguments to be handled */
    start_args.cores=atoi(argv[i+1]);
    fprintf(stdout, "EBBOS: overriding cores to %ld\n", start_args.cores);
  } else {
    fprintf(stderr, "EBBOS: unknown argument stripped: %s\n", argv[i]);
    exit(1);
  }
  return ret;
}

void
startinfo(int argc, char **argv, char **environ, 
	  uintptr_t *addr, intptr_t *size) 
{
  const char *ebbos_arg = "-ebbos";
  char *data, *cur;
  int *argcl;
  int s, i, j;

  s = sizeof(int); // add bytes for argc
  for (i=0; i<argc; i++) {
    s += strlen(argv[i]); 
    s++; // add one for null
  }

  for (i=0; environ[i]!=0; i++) {
    s += strlen(environ[i]);
    s++; // add one for null
  }

  data = (char *)malloc(s);
  assert(data);
  cur = data;

  // first bytes are for argc
  argcl = ((int *)cur);

  *((int *)cur) = argc;
  cur+=sizeof(int);

  *argcl = argc;		/* will change to strip off -lrt_cores */
  // followed by argv data
  for (i=0; i<argc; i++) {
    if (strncmp(argv[i], ebbos_arg, strlen(ebbos_arg))==0) {
      int osargs;		/* arguments processed */
      osargs = parse_ebbos_arg(i, argv, &s);
      for (j=0; j<osargs; j++) {
	s -= strlen(argv[i+j]) + 1;
      }
      i += osargs - 1;
      *argcl -= osargs;
    } else {
      cur = stpcpy(cur, argv[i]);
      cur++;
    }
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

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif


int
static num_phys_cores()
{
#ifdef __APPLE__
  //DS: activecpu is the number currently active (in case of hotplugging)
  int numthreads;
  size_t size;

  size = sizeof(numthreads);
  if (sysctlbyname("hw.activecpu", &numthreads, &size, NULL, 0) == -1) {
    perror("sysctlbyname");
    return -1;
  }

  return numthreads;
#else // if LINUX/UNIX
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

void
start_cores(int cores)
{
  int i;

  // check cores
  // start up another core, with the 
  fprintf(stderr, "EBBOS:%s: starting cores %d\n", __func__, cores);
  pthread_attr_t attr; 
  
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  
  for (i=0; i<start_args.cores; i++) {
    pthread_t tid; /* we don't actually need the thread id I think anywhere */
#ifdef __APPLE__
    //unique affinity is what we want, the scheduler takes this
    // as a suggestion. tag = 0 means no affinity preference
    thread_affinity_policy_data_t aff = {
      .affinity_tag = (i % start_args.physcores) + 1
    };
    //Create the thread suspended so we can get the tid
    if (pthread_create_suspended_np(&tid, &attr, lrt_event_init,
				    (void *)(uintptr_t)i) == -1) {
      perror("pthread_create_suspended_np");
      return;
    }
    //now set its affinity to the unique value
    kern_return_t err = thread_policy_set(pthread_mach_thread_np(tid),
					  THREAD_AFFINITY_POLICY,
					  (thread_policy_t)&aff,
					  THREAD_AFFINITY_POLICY_COUNT);
    LRT_Assert(err == KERN_SUCCESS);
    //now start the thread
    err = thread_resume(pthread_mach_thread_np(tid));
    LRT_Assert(err == KERN_SUCCESS);
#elif __linux__
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    // pin to a core, round robined over the physical cores
    CPU_SET(i%start_args.physcores, &cpus);
    pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
    if (pthread_create(&tid, &attr, lrt_event_init, 
		       (void *)(uintptr_t)i) != 0) {
      perror("pthread_create");
      return;
    }
#endif
  }
  // for now, wait until child exits; eventually
  // put in synchronizatoin here that will do something good when 
  // core exits
  pause();		   
}

int
main(int argc, char **argv, char **environ) 
{
  start_args.physcores = start_args.cores = num_phys_cores();
  start_args.start_info = 0;
  start_args.start_info_size = 0;
  
  // may change number of cores 
  startinfo(argc, argv, environ, &start_args.start_info, &start_args.start_info_size);

  lrt_event_preinit(start_args.cores);
  lrt_mem_preinit(start_args.cores);
  lrt_trans_preinit(start_args.cores);
  // calls event init on all cores, first event is lrt_start
  start_cores(start_args.cores);
  
  //  dumpstartargs();
  return -1;
}
