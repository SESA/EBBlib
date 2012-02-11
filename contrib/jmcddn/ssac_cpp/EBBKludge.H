#ifndef __EBB_KLUDGE_H__
#define __EBB_KLUDGE_H__

#define TRACE(fmt, ...)	fprintf(stderr, "%s: " fmt "\n", __func__, ##__VA_ARGS__)

#define MISC 			1
#define TR_INFO 	1
#define SET_CLSCD(A,B) /*TODO: add functionality */

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

class EBB {
protected:
  void *_ref;
};

#define DREF(ref) ref

#endif

