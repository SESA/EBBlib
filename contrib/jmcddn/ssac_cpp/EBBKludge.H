#ifndef __EBB_KLUDGE_H__
#define __EBB_KLUDGE_H__

// this is tornado speak for setting the class code, we should strip this out everywhere
// after we decide what we want to do with errors with EBBLIB
#define SET_CLSCD(A,B) 

#ifndef EBBLIB
#define TRACE(fmt, ...)	fprintf(stderr, "%s: " fmt "\n", __func__, ##__VA_ARGS__)

#define MISC 			1
#define TR_INFO 	1

typedef int EBBRC;
#else 
#define TRACE(fmt, ...)	EBB_LRT_printf("%s: " fmt "\n", __func__, ##__VA_ARGS__)
#endif

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

#ifndef EBBLIB
class CPlusEBB {
protected:
};

#define DREF(id) id
#define EBBRC_OK 0 

#else
#include <config.h>
#include <stdint.h>

#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <lrt/io.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cplus/CPlusEBB.H>
#include <l0/cplus/CPlusEBBRoot.H>
#include <l0/cplus/CPlusEBBRootShared.H>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>

#define DREF EBBId_DREF
#endif

#endif

