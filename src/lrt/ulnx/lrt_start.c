#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <l0/lrt/pic.h>


extern void EBBStart(void);

// add these to where they should go and implement them!
void
lrt_mem_init(void)
{
  fprintf(stderr, "%s: NYI\n", __func__);
}

void
lrt_trans_init(void)
{
  fprintf(stderr, "%s: NYI\n", __func__);
}

// first code to be runnining on an interrupt
void lrt_start(void)
{
  fprintf(stderr, "%s: start!\n", __func__);
  lrt_mem_init();
  lrt_trans_init();
  EBBStart();
}

int
main(int argc, char **argv)
{
  uval cores=1;

  fprintf(stderr, "%s: start!\n", __func__);
  if (argc>1) cores=atoi(argv[1]);
  lrt_pic_init(cores, lrt_start);
  return -1;
}
