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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#if __APPLE__
#include <pthread.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#elif __linux__
#include <sys/epoll.h>
#endif

#include <l0/lrt/trans.h>
#include <lrt/assert.h>
#include <l0/lrt/event.h>
#include <l0/lrt/event_irq_def.h>
#include <l0/lrt/ulnx/lrt_start.h>
#include <l0/lrt/mem.h>
#include <l0/lrt/event_bv.h>
#include <lrt/string.h>

//the global event table
static struct lrt_event_descriptor lrt_event_table[LRT_EVENT_NUM_EVENTS];

struct lrt_event_local_data {
  int fd; //either epollfd or kqueuefd
  int pipefd_read; //No other event locations should read this!
  int pipefd_write; //For synthesized events from potentially other locations
};

// counters; FIXME currently only update on hardware
int lrt_event_dispatched_events __attribute__ ((aligned(256))) =0 ;
int lrt_event_bv_dispatched_events __attribute__ ((aligned(256))) =0;
int lrt_event_loop_count __attribute__ ((aligned(256))) =0;

//To be allocated at preinit, the array of local event data
static struct lrt_event_local_data *event_data;
static int num_cores = 0;

static const intptr_t PIPE_UDATA = -1;

// configuration flags:
int lrt_event_use_bitvector_local=1;
int lrt_event_use_bitvector_remote=0;
// counters 

static void 
dispatch_event(lrt_event_num en)
{
  struct lrt_event_descriptor *desc = &lrt_event_table[en];
  lrt_event_dispatched_events++;
  lrt_trans_id id = desc->id;
  lrt_trans_func_num fnum = desc->fnum;

  //this infrastructure should be pulled out of this file
  lrt_trans_rep_ref ref = lrt_trans_id_dref(id);
  ref->ft[fnum](ref);
}  

static void __attribute__ ((noreturn))
lrt_event_loop(void)
{
  //get my local event data
  struct lrt_event_local_data *ldata = &event_data[lrt_my_event_loc()];

  while (1) {
    int en = -1;

    if (lrt_event_use_bitvector_local || lrt_event_use_bitvector_remote) 
      en = lrt_event_get_unset_bit(lrt_my_event_loc());

    if (en != -1) {
      lrt_event_bv_dispatched_events++;
      dispatch_event(en);
      continue;
    }


#if __APPLE__
    struct kevent kev;
    //This call blocks until an event occurred
    //a potential optimization would be to allocate an array of kevents
    //to batch some events
    kevent(ldata->fd, NULL, 0, &kev, 1, NULL);
    //FIXME: check for errors
#elif __linux__
    struct epoll_event kev;

    //This call blocks until an event occurred
    int rc;
    do {
      rc = epoll_wait(ldata->fd, &kev, 1, -1);
    } while(rc == -1 && errno == EINTR);

    if (rc == -1) {
      perror("epoll");
      LRT_Assert(0);
    }
    //FIXME: check for errors
#endif

    lrt_event_num ev;

    if (
#if __APPLE__
        kev.udata == (void *)PIPE_UDATA
#elif __linux__
        kev.data.u64 == (uint64_t)PIPE_UDATA
#endif
        ) {
      //We received at least a byte on the pipe

      //This is technically a blocking read, but I don't believe it
      //matters because we only woke up because the pipe was ready
      //to read and we are the only reader
      ssize_t rc = read(ldata->pipefd_read, &ev, sizeof(ev));
      LRT_Assert(rc == sizeof(ev));
      //FIXME: check for errors

    } else {
      //IRQ occurred
#if __APPLE__
      ev = (lrt_event_num)(intptr_t)kev.udata;
#elif __linux__
      ev = (lrt_event_num)kev.data.u64;
#endif
    }
    dispatch_event(ev);

    //an optimization here would be to keep reading from the pipe or checking
    //other events before going back around the loop
  }
}

#ifdef __APPLE__
pthread_key_t lrt_event_myloc_pthreadkey;
lrt_event_loc lrt_my_event_loc()
{
  return ((lrt_event_loc)(uintptr_t)pthread_getspecific(lrt_event_myloc_pthreadkey));
};
#elif __linux__
__thread lrt_event_loc lrt_event_myloc;
#endif


void *
lrt_event_init(void *myloc)
{
#ifdef __APPLE__
  pthread_setspecific(lrt_event_myloc_pthreadkey, myloc);
#else
  lrt_event_myloc = (lrt_event_loc)(uintptr_t)myloc;
#endif

  //get my local event data
  struct lrt_event_local_data *ldata = &event_data[lrt_my_event_loc()];

  //setup
  #if __APPLE__
  ldata->fd = kqueue();
  //FIXME: check for errors
  #elif __linux__
  ldata->fd = epoll_create(1);
  //FIXME: check for errors
  #endif

  int pipes[2];
  int rc = pipe(pipes);
  LRT_Assert(rc == 0);
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
  #elif __linux__
  struct epoll_event ev = {
    .events = EPOLLIN,
    .data.u64 = (uint64_t)PIPE_UDATA
  };
  epoll_ctl(ldata->fd, EPOLL_CTL_ADD, ldata->pipefd_read, &ev);
  #endif

  // we call the start routine to initialize
  // mem and trans before falling into the loop
  lrt_start();
  lrt_event_loop();
}


/* get number of logical pics, i.e., cores */
lrt_event_loc
lrt_num_event_loc()
{
  return num_cores;
}

/* get next pic in some sequence from current one; loops */
lrt_event_loc lrt_next_event_loc(lrt_event_loc l)
{
  return (l+1)%num_cores;
}

void
lrt_event_preinit(int cores)
{
  num_cores = cores;
  // event_data = malloc(sizeof(*event_data) * num_cores), always on core 0 here
  event_data = lrt_mem_alloc((sizeof(*event_data) * num_cores), 8, 0);
  lrt_event_bv = lrt_mem_alloc(sizeof(struct corebv) * cores, 8, 0);
  bzero(lrt_event_bv, sizeof(struct corebv) * cores);

  //FIXME: check for errors
#if __APPLE__
  pthread_key_create(&lrt_event_myloc_pthreadkey, NULL);
  //FIXME: check for errors
#endif

  //FIXME: initialize event table
}

void
lrt_event_bind_event(lrt_event_num num, lrt_trans_id handler, lrt_trans_func_num fnum)
{
  lrt_event_table[num].id = handler;
  lrt_event_table[num].fnum = fnum;
}

void
lrt_event_trigger_event(lrt_event_num num,
                        enum lrt_event_loc_desc desc,
                        lrt_event_loc loc)
{
  int pipefd;
  int islocal = 0;
  if (loc == lrt_my_event_loc())
    islocal = 1;

  if ( (islocal && lrt_event_use_bitvector_local) ||
       (!islocal && lrt_event_use_bitvector_remote) ) {
    lrt_event_set_bit(loc, num);
  } 

  if ((!islocal) || !lrt_event_use_bitvector_local) {
    if (desc == LRT_EVENT_LOC_SINGLE) {
      //protects from a race on startup
      do {
	pipefd = *(volatile int *)&event_data[loc].pipefd_write;
      } while (pipefd == 0);
      
      ssize_t rc = write(pipefd, &num, sizeof(num));
      LRT_Assert(rc == sizeof(num));
      //FIXME: check errors
    } else if (desc == LRT_EVENT_LOC_ALL) {
      lrt_event_loc num_ev = lrt_num_event_loc();
      for (lrt_event_loc i = 0; i < num_ev; i++) {
	//protects from a race on startup
	do {
	  pipefd = *(volatile int *)&event_data[i].pipefd_write;
	} while (pipefd == 0);
	
	ssize_t rc = write(pipefd, &num, sizeof(num));
	LRT_Assert(rc == sizeof(num));
      }
    }
  }
  //FIXME: check for errors
}

void
lrt_event_route_irq(struct IRQ_t *isrc, lrt_event_num num,
                    enum lrt_event_loc_desc desc, lrt_event_loc loc)
{
  //No changes necessary if this conditional is true
  if ((desc == LRT_EVENT_LOC_NONE && isrc->desc == LRT_EVENT_LOC_NONE) ||
      (num == isrc->num &&
       ((desc == LRT_EVENT_LOC_ALL && isrc->desc == LRT_EVENT_LOC_ALL) ||
        (desc == LRT_EVENT_LOC_SINGLE && isrc->desc == LRT_EVENT_LOC_SINGLE &&
         loc == isrc->loc)))) {
    return;
  }
#if __APPLE__
  int numkevents = __builtin_popcount(isrc->flags);

  struct kevent kevs_add[numkevents];
  struct kevent kevs_remove[numkevents];

  int i = numkevents;
  if (isrc->flags & LRT_EVENT_IRQ_READ) {
    EV_SET(&kevs_add[--i], isrc->fd, EVFILT_READ,
           EV_ADD, 0, 0, (void *)(uintptr_t)num);
    EV_SET(&kevs_remove[i], isrc->fd, EVFILT_READ,
           EV_DELETE, 0, 0, (void *)(uintptr_t)num);
  }

  if (isrc->flags & LRT_EVENT_IRQ_WRITE) {
    EV_SET(&kevs_add[--i], isrc->fd, EVFILT_WRITE,
           EV_ADD, 0, 0, (void *)(uintptr_t)num);
    EV_SET(&kevs_remove[i], isrc->fd, EVFILT_WRITE,
           EV_DELETE, 0, 0, (void *)(uintptr_t)num);
  }

  struct timespec timeout = {
    .tv_sec = 0,
    .tv_nsec = 0
  };
#elif __linux__
  struct epoll_event ev;
  ev.data.u64 = num;
  if (isrc->flags & LRT_EVENT_IRQ_READ) {
    ev.events |= EPOLLIN;
  }

  if (isrc->flags & LRT_EVENT_IRQ_WRITE) {
    ev.events |= EPOLLOUT;
  }
#endif

  if (isrc->desc == LRT_EVENT_LOC_ALL || desc == LRT_EVENT_LOC_ALL) {
    lrt_event_loc num_event_loc = lrt_num_event_loc();
    for (lrt_event_loc i = 0; i < num_event_loc; i++) {
      struct lrt_event_local_data *ldata = &event_data[i];
      //TODO: I would guess this statement could be written better...
      //This checks if the irq is currently mapped in on core i
      // and not requested to be mapped or having a different event.
      //In such a case, remove the event
      if ((isrc->desc == LRT_EVENT_LOC_ALL ||
           (isrc->desc == LRT_EVENT_LOC_SINGLE && isrc->loc == i)) &&
          !(num == isrc->num &&
            (desc == LRT_EVENT_LOC_ALL ||
             (desc == LRT_EVENT_LOC_SINGLE && loc == i)))) {
#if __APPLE__
        kevent(ldata->fd, kevs_remove, numkevents, NULL, 0, &timeout);
#elif __linux__
        epoll_ctl(ldata->fd, EPOLL_CTL_DEL, isrc->fd, NULL);
#endif
      }
      //This checks if the irq is requested to be mapped in on core i
      // and not already mapped or having a different event
      //In such a case, add the event
      if ((desc == LRT_EVENT_LOC_ALL ||
           (desc == LRT_EVENT_LOC_SINGLE && loc == i)) &&
          !(num == isrc->num &&
            (isrc->desc == LRT_EVENT_LOC_ALL ||
             (isrc->desc == LRT_EVENT_LOC_SINGLE && isrc->loc == i)))) {
#if __APPLE__
        kevent(ldata->fd, kevs_add, numkevents, NULL, 0, &timeout);
#elif __linux__
        epoll_ctl(ldata->fd, EPOLL_CTL_ADD, isrc->fd, &ev);
#endif
      }
    }
  } else {
    //Checks if the irq is currently mapped and either is on a different
    // event or is not mapped to the same event, then removes
    if (isrc->desc == LRT_EVENT_LOC_SINGLE &&
        !(num == isrc->num &&
          desc == LRT_EVENT_LOC_SINGLE &&
          isrc->loc == loc)) {
      struct lrt_event_local_data *ldata = &event_data[isrc->loc];
#if __APPLE__
      kevent(ldata->fd, kevs_remove, numkevents, NULL, 0, &timeout);
#elif __linux__
      epoll_ctl(ldata->fd, EPOLL_CTL_DEL, isrc->fd, NULL);
#endif
    }
    //Checks if the request is to map it and either is on a different event
    // or not the same location
    if (desc == LRT_EVENT_LOC_SINGLE &&
        !(num == isrc->num &&
          isrc->desc == LRT_EVENT_LOC_SINGLE &&
          isrc->loc == loc)) {
      struct lrt_event_local_data *ldata = &event_data[loc];
#if __APPLE__
      kevent(ldata->fd, kevs_add, numkevents, NULL, 0, &timeout);
#elif __linux__
      epoll_ctl(ldata->fd, EPOLL_CTL_ADD, isrc->fd, &ev);
#endif
    }
  }

  //update the IRQ struct
  isrc->desc = desc;
  isrc->loc = loc;
  isrc->num = num;
}
