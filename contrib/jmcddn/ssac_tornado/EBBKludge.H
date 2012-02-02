#ifndef __EBB_KLUDGE_H__
#define __EBB_KLUDGE_H__

#define TRACE(fmt, ...)	fprintf(stderr, "%s: " fmt "\n", __func__, ##__VA_ARGS__)

#define MISC 			1
#define TR_INFO 	1
#define SET_CLSCD /*TODO: add functionality */

typedef int EBBRC;

/* TODO: make into functional printf wrappers*/
static inline char* tassert(...){ }
static inline char* trace(...){ }
static inline char* ass_printf(...){ }
static inline char* tr_printf(...){ }

class EBB {
protected:
  int _ref;
};


#endif

