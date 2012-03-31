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

/*  MACRO DEMISTIFICATION *************
 *
#define CObject(name)		    \
  typedef struct name ## _obj name; \
  typedef name * name ## Ref;	    \
  struct name ## _obj
#define CObjIfName(name) name ## _if
#define CObjInterface(name) struct CObjIfName(name)
#define CObjImplements(name) CObjInterface(name) CObjIfName(name)
 *
 *  typedef struct SSAC_obj SSAC;
 *  typedef SSAC* SSACRef
 *  struct SSAC_obj {
 *    struct App_if *ft;
 *  }
 *
 * */
CObject(SSAC) {
  CObjInterface(App) *ft; //
};

/* debug dump envoirnment vars */
static void
dumpArgsAndEnviron(int argc, char **argv, char **environ)
{
  int i;
  for (i=0; i<argc; i++) EBB_LRT_printf("argv[%d]=%s\n", i, argv[i]);
  for (i=0; environ[i]!=NULL; i++) EBB_LRT_printf("environ[%d]=%s\n",
						  i, environ[i]);
}

/* EBB start function */
EBBRC
SSAC_start(AppRef _self, int argc, char **argv,
		 char **environ)
{
  EBB_LRT_printf("SSAC LOADED\n");
  dumpArgsAndEnviron(argc, argv, environ);
  EBB_LRT_printf("%s: PASSED\n", argv[0]);
  return EBBRC_OK;
}

/* Specify EBB interface */

/* MACRO DEFISTIFICATION **
 *  struct App_if SSAC_ftable = ...
 */
CObjInterface(App) SSAC_ftable = {
  .start = SSAC_start
};

/* Define Application */
APP(SSAC);
/* MACRO DEMISTIFICATION ********
  EBBRep * App_createRep(CObjeBBRootMultiRef _self)
  {
      SSAC * repRef;
      EBBPrimMalloc(sizeof(SSAC), &redRef, EBB_MEM_DEFAULT);
      repRef->ft = &SSAC_ftable;
      return (SSAC *) repRef;
  }
*/
