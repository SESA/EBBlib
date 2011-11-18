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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <pthread.h>

#include <stdint.h>
#include <l0/lrt/ulnx/pic.h>

extern lrt_pic_handler EBBstart;

#ifdef __APPLE__
pthread_key_t lrt_pic_myid_pthreadkey;
#else
__thread lrt_pic_id lrt_pic_myid;
#endif
lrt_pic_id lrt_pic_firstid;
lrt_pic_id lrt_pic_lastid;

enum {NUM_LPICS_CONFIGED = 4};

#ifdef __APPLE__
enum {FIRST_VECFD = 128};
#else
enum {FIRST_VECFD = 16};
#endif
enum {NUM_MAPPABLE_VEC = 15};
// reserve 2 : 1 for ipi and 1 additional
enum {NUM_RES_VEC = 2};
enum {RES0_VEC = (NUM_MAPPABLE_VEC)};
enum {RES1_VEC = (RES0_VEC + 1)};
enum {IPI_VEC = (RES0_VEC)};
enum {RST_VEC = (RES1_VEC)};
enum {NUM_VEC = (NUM_MAPPABLE_VEC + NUM_RES_VEC)};

#ifndef FD_COPY
#define FD_COPY(src,dest) memcpy((dest),(src),sizeof(dest))
#endif

struct VecDesc {
  lrt_pic_set set;
  lrt_pic_handler h;
  int fd;
};

struct Pic {
  struct VecDesc vecs[NUM_VEC];
  fd_set fdset;
  uintptr_t free;
  uintptr_t numlpics;
  uintptr_t lpiccnt;
  uintptr_t lock;
  int maxfd;
} pic;

//FIXME: Do we want to pad these to cacheline size
struct LPic {
  struct timespec periodic;
  lrt_pic_set mymask;
  lrt_pic_id id;
  uintptr_t aux;
  uintptr_t ipiStatus;
  uintptr_t resetStatus;
} lpics[LRT_PIC_MAX_PICS];


static uintptr_t
lock(void)
{
  uintptr_t rc=0;
  
  while (!rc) {
    rc = __sync_bool_compare_and_swap(&(pic.lock), 0, 1);
  }
  return rc;
}

static void
unlock(void)
{
  __sync_bool_compare_and_swap(&(pic.lock), 1, 0);
}

static void
sighandler(int s)
{
  return;
}

static inline void 
wakeup(lrt_pic_id target)
{
  //  printf("wup: target=%ld ipiStatus=%ld\n", target, lpics[target].ipiStatus);
  pthread_kill((pthread_t)lpics[target].aux, SIGINT);
}

static inline void
wakeupall(void)
{
  lrt_pic_id i;
  assert(pic.lpiccnt == pic.numlpics);
  for (i=lrt_pic_firstid; i<=lrt_pic_lastid; i++) wakeup(i);
}

void
lrt_pic_enable(uintptr_t vec)
{
  lrt_pic_set_add(pic.vecs[vec].set, lrt_pic_myid);
}

void
lrt_pic_disable(uintptr_t vec)
{
  if (vec != RST_VEC) lrt_pic_set_remove(pic.vecs[vec].set, lrt_pic_myid);
}

void
lrt_pic_enableipi(void)
{
  lrt_pic_enable(IPI_VEC);
}

void
lrt_pic_disableipi(void)
{
  lrt_pic_disable(IPI_VEC);
}


uintptr_t 
lrt_pic_firstvec(void) 
{ 
  return 0; 
}

uintptr_t 
lrt_pic_numvec(void) 
{ 
  return NUM_MAPPABLE_VEC; 
}
    
intptr_t
lrt_pic_init(uintptr_t numlpics, lrt_pic_handler h)
{
  int fd[FIRST_VECFD];
  int i=0;
  uintptr_t id;
  struct sigaction sa;
  sigset_t blockset;

  // confirm sanity of pic configuration 
  assert(LRT_PIC_MAX_PICS/64 * 64 == LRT_PIC_MAX_PICS);
  assert(numlpics <= LRT_PIC_MAX_PICS);

  // initialize all pic state to zero
  bzero(&pic, sizeof(pic));
  bzero(lpics, sizeof(lpics));

  // setup basic global facts
  lrt_pic_firstid = LRT_PIC_FIRST_PIC_ID;
  lrt_pic_lastid  = numlpics-1;
  pic.numlpics = numlpics;

  // lrt_pic_myid setup for first pic (thread of execution of init)
#ifdef __APPLE__
  pthread_key_create(&lrt_pic_myid_pthreadkey, NULL);
  pthread_setspecific(lrt_pic_myid_pthreadkey, (void *)lrt_pic_firstid);
#else
  lrt_pic_myid = lrt_pic_firstid;
#endif
  
  // initialized working fd array 
  bzero(&fd, sizeof(fd));
  fd[i] = open("/dev/null", O_RDONLY);

  // get us to the FIRST_VEC fd by opening
  // fds until we get to FIRST_VEC
  while (fd[i] < (FIRST_VECFD-1)) {
    i++;
    fd[i] = dup(fd[0]);
  };

  // reserve fds for our vectors
  for (i=0; i<NUM_VEC; i++) {
    pic.vecs[i].fd=dup(fd[0]);
    if (pic.vecs[i].fd != (FIRST_VECFD+i)) {
      fprintf(stderr, "ERROR: file %s line %d: runtime tromping over fd space\n"
	      "\tsuggest you increase the FIRST_VECFD\n", __FILE__, __LINE__);
      return -1;
    }
  }
   
  // close and free fd's that we allocated to get to
  // FIRST_VEC
  for (i=0; i<FIRST_VECFD; i++) if(fd[i]) close(fd[i]);

  // explicity setup fdset so that we are not paying attention
  // to any vectors at start... vectors are added when they are mapped
  FD_ZERO(&pic.fdset);
  
  // setup default signal mask so that SIGINT is being ignored by
  // all pic threads when they start however ensure that a common 
  // handler is in place
  /* this code was based on http://lwn.net/Articles/176911/ */
  sigemptyset(&blockset);         /* Block SIGINT */
  sigaddset(&blockset, SIGINT);
  pthread_sigmask(SIG_BLOCK, &blockset, NULL);

  sa.sa_handler = sighandler;        /* Establish signal handler */
  sa.sa_flags = 0;
  sigemptyset(&(sa.sa_mask));
  sigaction(SIGINT, &sa, NULL);

  assert(!lrt_pic_set_test(lpics[lrt_pic_myid].mymask,lrt_pic_myid));
  // setup where the initial ipi will be directed to
  lrt_pic_mapreset(h);

  // record this the thread id of this thread as lpic 
  lrt_pic_set_add(lpics[lrt_pic_myid].mymask, lrt_pic_myid);
  lpics[lrt_pic_myid].aux = (uintptr_t)pthread_self();
  // start up threads for any other lpics
  for (id=lrt_pic_firstid+1; id<=lrt_pic_lastid; id++) {
    if (pthread_create((pthread_t *)(&lpics[id].aux), 
		       NULL, (void *(*)(void*))lrt_pic_loop, 
		       (void *)id) != 0) {
      perror("pthread_create");
      return -1;
    }
  }

  // initiate a reset to get things going once the loop is up
  lrt_pic_reset();
  lrt_pic_loop(lrt_pic_myid);

  assert(0);

  return 1;
}

intptr_t
lrt_pic_allocvec(uintptr_t *vec)
{
  uintptr_t rtn;
  uintptr_t i;
  intptr_t rc=-1;

  lock();

  for (i=0, rtn=pic.free; i<NUM_MAPPABLE_VEC; i++) {
    if (pic.vecs[rtn].h == NULL) {
      pic.vecs[rtn].h = (lrt_pic_handler)((uintptr_t)-1);
      pic.free++;
      if (pic.free >= NUM_MAPPABLE_VEC) pic.free=0;
      *vec = rtn;
      rc=1;
      goto done;
    }
    rtn++;
    if (rtn >= NUM_MAPPABLE_VEC) rtn=0;
  }    

 done:
  unlock();
  return rc;

}

intptr_t 
lrt_pic_mapipi(lrt_pic_handler h)
{

  pic.vecs[IPI_VEC].h = h;
  return 1;
}

intptr_t 
lrt_pic_mapreset(lrt_pic_handler h)
{

  pic.vecs[RST_VEC].h = h;
  return 1;
}


intptr_t
lrt_pic_ipi(lrt_pic_id target)
{
  if (target>lrt_pic_lastid) return -1;
  // FIXME: probably need to make this at least volatile
  lpics[target].ipiStatus = 1;
  wakeup(target);
  return 1;
}

intptr_t
lrt_pic_reset()
{
  lpics[lrt_pic_myid].resetStatus = 1;
  wakeup(lrt_pic_myid);
  return 1;
}

void
lrt_pic_ackipi(void)
{
  lpics[lrt_pic_myid].ipiStatus = 0;
}


// FIXME: 1: make the vectors lpic specific
//        2: lpic loop need not monitor fd's that it is not mapped too
//           thus we can avoid waking up all threads unecessarily
//        3: no need to wakeupall can wakeup only affected lpics
//    Perhaps just provide two interfaces for common cases of this, 'on' and 
//    'all': lrt_pic_mapvec, lrt_pic_mapvec_on, lrt_pic_mapvec_all
intptr_t
lrt_pic_mapvec(lrt_pic_src s, uintptr_t vec, lrt_pic_handler h)
{
  int i;
  int sfd = (int)s;
  int rc=1;
  
  lock();

  if (pic.vecs[vec].h == 0) {
    rc=-1;
    goto done;
  }

  pic.vecs[vec].h = h;
  i = dup2(sfd, pic.vecs[vec].fd);
  assert(i == pic.vecs[vec].fd);
  FD_SET(i, &pic.fdset);
  if (i>pic.maxfd) pic.maxfd=i;
  wakeupall();

 done:
  unlock();
  return rc;
}

static void
bind_proc(uintptr_t p)
{
#if 0
  cpu_set_t mask;

  CPU_ZERO( &mask );
  CPU_SET(p, &mask);
  if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
    perror("ERROR: Could not set CPU Affinity");
  }
#else
  fprintf(stderr, "%s: NYI\n", __func__);
#endif
}

// conncurrent pic loop
// one loop for each local pic
// each pic will wake up on any interrupt occurring but only dispatches
// the handler if the interrupt includes the pic id of the loop in its pic set
intptr_t 
lrt_pic_loop(lrt_pic_id myid)
{
  fd_set rfds, efds;
  int v,i, rc;
  sigset_t emptyset;
  struct LPic *lpic = lpics + myid;

#ifdef __APPLE__
  struct timespec tout;
  bzero(&tout, sizeof(tout));
  tout.tv_nsec = 100000000; // .1 seconds
  pthread_setspecific(lrt_pic_myid_pthreadkey, (void *)myid);
#else
  lrt_pic_myid = myid;
#endif
  lrt_pic_set_add(lpic->mymask, myid);
  bind_proc(lrt_pic_myid);

  lock(); pic.lpiccnt++; unlock();

  // start with ipi and reset enabled on all lpics so that we can get going via an ipi
  lrt_pic_enableipi();
  lrt_pic_enable(RST_VEC);
  // wait for all lpics to be up before we get going
  while (pic.lpiccnt != pic.numlpics);
  //  printf("%d: pic.lpiccnt=%ld\n", lrt_pic_myid, pic.lpiccnt);

  while (1) {
    FD_COPY(&pic.fdset, &rfds);
    FD_COPY(&pic.fdset, &efds);

    sigemptyset(&emptyset);
#ifdef __APPLE__
    rc = pselect(pic.maxfd+1, &rfds, NULL, &efds, &tout, &emptyset);
#else
    rc = pselect(pic.maxfd+1, &rfds, NULL, &efds, NULL, &emptyset);
#endif
    if (rc < 0) {
      if (errno==EINTR) {
	// do nothing
      } else {
	fprintf(stderr, "Error: pselect failed (%d)\n", errno);
	perror("pselect");
	return -1;
      }
    }

    // may later want to actually have a period event that has a programmable
    // period
#ifndef __APPLE__
    if (rc == 0) {
      fprintf(stderr, "What Select timed out\n");
      return -1;
    }
#endif

   if (lpic->resetStatus) {
     lpic->resetStatus=0;
     pic.vecs[RST_VEC].h();
   }

    if (lpic->ipiStatus && lrt_pic_set_test(pic.vecs[IPI_VEC].set, myid)) {
      lrt_pic_disableipi();
      pic.vecs[IPI_VEC].h();
    }

    for (i = FIRST_VECFD,v=0; i <= pic.maxfd; i++, v++) {
      if ((FD_ISSET(i, &efds) || (FD_ISSET(i, &rfds)))
	  && lrt_pic_set_test(pic.vecs[i].set, myid)) {
	if (pic.vecs[v].h) {
	  pic.vecs[v].h();
	} else {
	  fprintf(stderr, "ERROR: %s: spurious interrupt on %d\n", __func__, i);
	}
      }
    }
  }

  return -1;
}

#ifdef PIC_TEST
#include <unistd.h>
#include <stdlib.h>

void
ipihdlr(void)
{
  fprintf(stderr, "%ld", lrt_pic_myid);
  fflush(stderr);
  sleep(2);
  lrt_pic_ackipi();
  // pass the ipi along to the next lrt
  lrt_pic_ipi((lrt_pic_myid+1)%(lrt_pic_lastid+1));
}

int
main(int argc, char **argv)
{
  uintptr_t cores=1;

  if (argc>1) cores=atoi(argv[1]);
  lrt_pic_init(cores, ipihdlr);
  return -1;
}

#endif
