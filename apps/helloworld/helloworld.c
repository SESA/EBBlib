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
#include <stdint.h>

#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <lrt/io.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <lrt/exit.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <lrt/assert.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/lrt/pic.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/App.h>

CObject(HelloWorld) {
  CObjInterface(App) *ft;
};

#ifdef LRT_ULNX
#ifdef __APPLE__
#include <sys/sysctl.h>
#endif


int
static num_phys_cores()
{
#ifdef __APPLE__
  /* 
   * seems to be three options, for now pick ncpu, which is the maximum, presumably hyperthreaded
   */
#if 0
  char *clrname = "hw.physicalcpu_max";
  char *clrname = "hw.logicalcpu_max";
#endif
  char *clrname = "hw.ncpu";
  int mib[4], numcores;
  size_t len, size;
  len = 4;
  sysctlnametomib(clrname, mib, &len);
  size = sizeof(numcores);
  if (sysctl(mib, len, &numcores, &size, NULL, 0)==-1) {
    perror("sysctl");
    return -1;
  }
  return numcores;
#else // if LINUX/UNIX
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}
#endif 

static void 
dumpArgsAndEnviron(int argc, char **argv, char **environ)
{
  int i, numcores;
  if (argv) {
    for (i=0; i<argc; i++) lrt_printf("argv[%d]=%s\n", i, argv[i]);
  }

  if (environ) {
    for (i=0; environ[i]!=NULL; i++) lrt_printf("environ[%d]=%s\n", 
						i, environ[i]);
  }

#ifdef LRT_ULNX
  numcores = num_phys_cores();
#endif
  lrt_printf("number of cores from platform %d\n", numcores);

}




EBBRC 
HelloWorld_start(AppRef _self, int argc, char **argv, 
		 char **environ)
{
  lrt_printf("Hello world!\n");
  dumpArgsAndEnviron(argc, argv, environ);
  if (argv) {
    lrt_printf("%s: PASSED\n", argv[0]);
  }
  lrt_exit(0);
  return EBBRC_OK;
}

CObjInterface(App) HelloWorld_ftable = {
  .start = HelloWorld_start
};

APP(HelloWorld);

