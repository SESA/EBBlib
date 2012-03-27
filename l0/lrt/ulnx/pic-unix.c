/* *********************************************************************** 

 *  Module Purpose: pic-unix implements the functionality to implement
 *    a pic on top of cabilities provided by a standard unix system.

 * ***********************************************************************/

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
#include <l0/lrt/ulnx/pic-unix.h>

#ifndef FD_COPY
#define FD_COPY(src,dest) memcpy((dest),(src),sizeof(dest))
#endif

struct unix_pic {
  fd_set fdset;
  int maxfd;
} upic;


void 
lrt_pic_unix_wakeup(uintptr_t lcore)
{
  pthread_kill((pthread_t)lcore, SIGINT);
}

static void
sighandler(int s)
{
  return;
}

int
lrt_pic_unix_init()
{
  int i=0;
  int fd[FIRST_VECFD];
  struct sigaction sa;
  sigset_t blockset;

  // zero the unix specific pic info
  bzero(&upic, sizeof(upic));

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
    int tmpfd;
    tmpfd=dup(fd[0]);
    if (tmpfd != (FIRST_VECFD+i)) {
      fprintf(stderr, "ERROR: file %s line %d: runtime tromping over fd space\n"
	      "\tsuggest you increase the FIRST_VECFD NUM_VEC=%d "
	      "tmpfd=%d i=%d FIRST_VECFD=%d\n"
	      , __FILE__, __LINE__, NUM_VEC, tmpfd, i, FIRST_VECFD);
      return -1;
    }
  }
   
  // close and free fd's that we allocated to get to
  // FIRST_VEC
  for (i=0; i<FIRST_VECFD; i++) if(fd[i]) close(fd[i]);

  // explicity setup fdset so that we are not paying attention
  // to any vectors at start... vectors are added when they are mapped
  FD_ZERO(&upic.fdset);
  
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
  return 0;
}

uintptr_t 
lrt_pic_unix_getlcoreid()
{
  return (uintptr_t)pthread_self();
}

uintptr_t 
lrt_pic_unix_addcore(void *(*routine)(void *), void *arg)
{
  uintptr_t val;
  if (pthread_create((pthread_t *)(&val), NULL, routine, arg) != 0) {
    perror("pthread_create");
    return 0;
  }
  return val;
}

/*
 * Make the new FD the FD for this vector
 */
int
lrt_pic_unix_locked_enable(uintptr_t src, uintptr_t vec)
{
  int i;
  i = dup2(src, vec+FIRST_VECFD);
  if (i != vec+FIRST_VECFD) {
    fprintf(stderr, "ERROR: file %s line %d: runtime tromping over fd space\n"
	    "\tsuggest you increase the FIRST_VECFD\n", __FILE__, __LINE__);
    return -1;
  }
  FD_SET(i, &upic.fdset);
  if (i>upic.maxfd) {
    upic.maxfd=i;
  }
  return 0;
}

int 
lrt_pic_unix_blockforinterrupt(lrt_pic_unix_ints *s)
{
  fd_set rfds, efds;
  sigset_t emptyset;
  int i, v=0, rc, numintr=0;

  lrt_pic_unix_ints_clear(s);

#ifdef __APPLE__
  struct timespec tout;
  bzero(&tout, sizeof(tout));
  tout.tv_nsec = 100000000; // .1 seconds
#endif

  FD_COPY(&upic.fdset, &rfds);
  FD_COPY(&upic.fdset, &efds);
  
  sigemptyset(&emptyset);
#ifdef __APPLE__
  rc = pselect(upic.maxfd+1, &rfds, NULL, &efds, &tout, &emptyset);
#else
  rc = pselect(upic.maxfd+1, &rfds, NULL, &efds, NULL, &emptyset);
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
  
  for (i = FIRST_VECFD,v=0; i <= upic.maxfd; i++, v++) {
#if 0 // debugging crap on what was signalled
    if (FD_ISSET(i, &efds)) {
      fprintf(stderr, "---efds i is %d\n", i);
    }
    if (FD_ISSET(i, &rfds)) {
      fprintf(stderr, "---rfds i is %d\n", i);
    }
#endif
    if ((FD_ISSET(i, &efds) || (FD_ISSET(i, &rfds)))) {
      lrt_pic_unix_ints_set(s, v);
      numintr++;
    }
  }
  return numintr;
}
