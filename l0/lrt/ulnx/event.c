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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#if __APPLE__
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif

#include <l0/types.h>
#include <lrt/assert.h>

struct lrt_event_descriptor {
  EBBId id; 
  FuncNum fnum;
};

#define LRT_NUM_EVENTS (256)
typedef uint8_t lrt_event_num;
STATIC_ASSERT((1 << (sizeof(lrt_event_num) * 8)) >= LRT_NUM_EVENTS,
	      "lrt_event_num cannot hold the range of events!");

//the global event table
struct lrt_event_descriptor lrt_event_table[LRT_NUM_EVENTS];

struct lrt_event_local_data {
  int fd; //either epollfd or kqueuefd
  int pipefd_read; //No other event locations should read this!
  int pipefd_write; //For synthesized events from potentially other locations
};

//To be allocated at preinit, the array of local event data
static struct lrt_event_local_data *event_data; 

static const intptr_t PIPE_UDATA = -1;

static void __attribute__ ((noreturn))
lrt_event_loop(void)
{
  //get my local event data
  struct lrt_event_local_data *ldata = &event_data[lrt_my_event_loc()];

  while (1) {
#if __APPLE__
    struct kevent kev;
    //This call blocks until an event occurred
    //a potential optimization would be to allocate an array of kevents
    //to batch some events
    kevent(ldata->fd, NULL, 0, &kev, 1, NULL);
    //FIXME: check for errors
#endif

    lrt_event_num ev;

#if __APPLE__
    if(kev.udata == (void *)PIPE_UDATA) {
#endif
      //We received at least a byte on the pipe

      //This is technically a blocking read, but I don't believe it
      //matters because we only woke up because the pipe was ready
      //to read and we are the only reader
      read(ldata->pipefd_read, &ev, sizeof(ev));
      //FIXME: check for errors
            
    } else {
      //IRQ occurred
#if __APPLE__
      ev = (lrt_event_num)(intptr_t)kev.udata;
#endif
    }

    struct lrt_event_descriptor *desc = &lrt_event_table[ev];
    EBBId id = desc->id;
    FuncNum fnum = desc->fnum;
    
    //this infrastructure should be pulled out of this file
#if 0
    //*(EBBId_DREF((EBBRepRef *)id))[fnum];
#endif
    printf("dispatching to id: %lx, funcnum: %d\n",
	   (uintptr_t)id, (int)fnum);

    //an optimization here would be to keep reading from the pipe or checking
    //other events before going back around the loop
  }
}

#ifdef __APPLE__
pthread_key_t lrt_event_myloc_pthreadkey;
lrt_event_loc lrt_my_event_loc()
{
  return ((lrt_event_loc)pthread_getspecific(lrt_event_myloc_pthreadkey));
};
#else
__thread lrt_event_loc lrt_event_myloc;
#endif


void *
lrt_event_init(void *myloc)
{
#ifdef __APPLE__
  pthread_setspecific(lrt_event_myloc_pthreadkey, myloc);
#else
  lrt_event_myloc = myloc;
#endif
  
  //get my local event data
  struct lrt_event_local_data *ldata = &event_data[lrt_my_event_loc()];

  //setup
  #if __APPLE__
  ldata->fd = kqueue();
  //FIXME: check for errors
  #endif

  int pipes[2];
  pipe(pipes);
  //FIXME: check for errors

  ldata->pipefd_read = pipes[0];
  //this act publishes that this event location is ready to receive events
  ldata->pipefd_write = pipes[1]; 

  //add the pipe to the watched fd
  #if __APPLE__
  struct timespec timeout = {
    .tv_sec = 0,
    .tv_nsec = 0
  };

  //setup the read pipe event
  struct kevent kev;
  EV_SET(&kev, ldata->pipefd_read, EVFILT_READ,
	 EV_ADD, 0, 0, (void *)PIPE_UDATA);

  //add it to the keventfd
  kevent(ldata->fd,  &kev, 1, NULL, 0, &timeout);
  //FIXME: check for errors
  #endif

  // we call the start routine to initialize 
  // mem and trans before falling into the loop
  lrt_start();
  lrt_event_loop();
}

void
lrt_event_preinit(int num_cores)
{
  event_data = malloc(sizeof(*event_data) * num_cores);
  //FIXME: check for errors
}

int main ()
{
  return 0;
}
