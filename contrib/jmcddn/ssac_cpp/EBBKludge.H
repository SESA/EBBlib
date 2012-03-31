#ifndef __EBB_KLUDGE_H__
#define __EBB_KLUDGE_H__

#ifndef EBBLIB
#define TRACE(fmt, ...)	fprintf(stderr, "%s: " fmt "\n", __func__, ##__VA_ARGS__)

#define MISC 			1
#define TR_INFO 	1
#define SET_CLSCD(A,B) /*TODO: add functionality / what is this? */

typedef int EBBRC;

#define tassert(EX,STMT)			\
  if (!(EX)) {					\
    TRACE("%s", #EX);				\
    STMT;					\
    abort();					\
  }

#define trace(S,L,STMT)				\
  {						\
    TRACE("%s %s", #S, #L);	                \
    STMT;                                       \
  }

#define ass_printf TRACE 
#define tr_printf TRACE

class CPlusEBB {
protected:
  void *_ref;
};

#define DREF(ref) ref
#else
#include <config.h>
#include <stdint.h>

#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <lrt/io.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/cplus/CPlusEBB.H>
#include <l0/cplus/CPlusEBBRoot.H>
#endif

#endif

