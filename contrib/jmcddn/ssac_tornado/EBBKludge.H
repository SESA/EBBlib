#ifndef __EBB_KLUDGE_H__
#define __EBB_KLUDGE_H__


#define TRACE(fmt, ...)	fprintf(stderr, "%s: " fmt "\n", __func__, ##__VA_ARGS__)

typedef int EBBRC;

class EBB {
protected:
  int _ref;
};


#endif

