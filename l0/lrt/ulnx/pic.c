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
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <stdint.h>
#include <l0/lrt/ulnx/pic-unix.h>
#include <l0/lrt/ulnx/pic.h>

#ifdef __APPLE__
pthread_key_t lrt_pic_myid_pthreadkey;
#else
__thread lrt_pic_id lrt_pic_myid;
#endif
lrt_pic_id lrt_pic_firstid;
lrt_pic_id lrt_pic_lastid;

/*
 * set: the set of cores on which this interrupt is enabled/disabled
 * handler: the handler, globally, for this interrupt
 */
struct VecDesc {
  lrt_pic_set set;
  lrt_pic_handler h;
};

/*
 * the global pic structure, maintains a 
 */
struct Pic {
  struct VecDesc vecs[NUM_VEC];
  uintptr_t free;
  uintptr_t numlpics;
  volatile uintptr_t lpiccnt;
  uintptr_t lock;
} pic;

/*
 * Array of pic structures with one per core. 
 */
//FIXME: Do we want to pad these to cacheline size
struct LPic {
  lrt_pic_set mymask;
  lrt_pic_id id;
  uintptr_t lcore;		/* logical core */
  volatile uintptr_t ipiStatus;
  volatile uintptr_t resetStatus;
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

static inline void
wakeupall(void)
{
  lrt_pic_id i;
  assert(pic.lpiccnt == pic.numlpics);
  for (i=lrt_pic_firstid; i<=lrt_pic_lastid; i++) 
    lrt_pic_unix_wakeup(lpics[i].lcore);
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
  uintptr_t id;

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
  
  if (lrt_pic_unix_init() != 0) { /* initialize logical HW for unix */
    //error, return -1;
    return -1;
  }

  assert(!lrt_pic_set_test(lpics[lrt_pic_myid].mymask, lrt_pic_myid));

  // setup where the initial ipi will be directed to
  lrt_pic_mapreset(h);

  // enable interrupts from myself
  lrt_pic_set_add(lpics[lrt_pic_myid].mymask, lrt_pic_myid);

  // get my core
  lpics[lrt_pic_myid].lcore = lrt_pic_unix_getlcoreid();

  // start up other cores
  for (id=lrt_pic_firstid+1; id<=lrt_pic_lastid; id++) {
    lpics[id].lcore = lrt_pic_unix_addcore((void *(*)(void*))lrt_pic_loop, 
					     (void *)id);
    if (lpics[id].lcore == 0) return -1;
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
  lrt_pic_unix_wakeup(lpics[target].lcore);
  return 1;
}

intptr_t
lrt_pic_reset()
{
  lpics[lrt_pic_myid].resetStatus = 1;
  lrt_pic_unix_wakeup(lpics[lrt_pic_myid].lcore);
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
  int rc=1;
  
  lock();

  if (pic.vecs[vec].h == 0) {
    rc=-1;
    goto done;
  }

  pic.vecs[vec].h = h;
  if ((rc = lrt_pic_unix_enable(s, vec)) != 0) 
    goto done;

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
  int v, numintr;
  struct LPic *lpic = lpics + myid;

#ifdef __APPLE__
  pthread_setspecific(lrt_pic_myid_pthreadkey, (void *)myid);
#else
  lrt_pic_myid = myid;
#endif
  lrt_pic_set_add(lpic->mymask, myid);
  bind_proc(lrt_pic_myid);

  lock(); pic.lpiccnt++; unlock();

  // all processors have ipi enabled at startup 
  // Semantics are that only reset is run on first proc
  // ipi is used on all other procs to get things going
  // reset code is expected to setup ipi vector appropriately
  // before invoking.  So at this point ipi vector maybe null
  lrt_pic_enableipi();
  
  // only reset vector to run on boot processor
  if (lrt_pic_myid == lrt_pic_firstid) lrt_pic_enable(RST_VEC);
  
  // wait for all lpics to be up before we get going
  while (pic.lpiccnt != pic.numlpics);
  
  while (1) {
    lrt_pic_unix_ints intrSet;
    
    if ((numintr = lrt_pic_unix_blockforinterrupt(&intrSet)) <0 ) {
      return -1;
    }
    
    if (lpic->resetStatus) {
      lpic->resetStatus=0;
      pic.vecs[RST_VEC].h();
    }
    
    if (lpic->ipiStatus && lrt_pic_set_test(pic.vecs[IPI_VEC].set, myid)) {
      lrt_pic_disableipi();
      assert(pic.vecs[IPI_VEC].h);
      pic.vecs[IPI_VEC].h();
    }
    
    
    for (v=0;v<NUM_VEC;v++) {
      if (lrt_pic_unix_ints_test(intrSet, v) && 
	  lrt_pic_set_test(pic.vecs[v].set, myid)) {
	numintr--;
	if (pic.vecs[v].h) {
	  pic.vecs[v].h();
	} else {
	  fprintf(stderr, "ERROR: %s: spurious interrupt on %d\n", __func__, v);
	}
	if(numintr<=0) break;
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
  lrt_pic_ackipi();
  fprintf(stderr, "%ld", lrt_pic_myid);
  fflush(stderr);
  sleep(2);
  lrt_pic_enableipi();
  // pass the ipi along to the next lrt
  lrt_pic_ipi((lrt_pic_myid+1)%(lrt_pic_lastid+1));
}


void
rsthdlr(void)
{
  fprintf(stderr, "%s: START: on %ld\n", __func__, 
	  lrt_pic_myid);
  fflush(stderr);  

  // setup our ipi handler and enable ipi 
  // FIXME:  for the moment these are global operations that affect all pics
  //         reset only runs on first lpic
  lrt_pic_mapipi(ipihdlr);

  // ipi is enabled by default... so all we have to do is send the first
  // ipi to ourselvs
  // send ipi to my self to get the ball rolling
  lrt_pic_ipi(lrt_pic_myid);
}

int
main(int argc, char **argv)
{
  uintptr_t cores=1;

  if (argc>1) cores=atoi(argv[1]);
  lrt_pic_init(cores, rsthdlr);
  return -1;
}

#endif
