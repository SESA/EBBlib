#ifndef __APP_H__
#define __APP_H__
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

#include <l0/cobj/CObjEBB.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/EventMgrPrim.h>
#include <l0/MemMgrPrim.h>

COBJ_EBBType(App) {
  // this is the primodial message to an application
  // remember that this is an event driven system so your
  // job is to simply do your setup work ... including
  // construction your objects and registering for events you
  // care about and then return.
  // On many cores you may not have any work to do here
  EBBRC (*start) (AppRef _self);
};

extern AppId theAppId;

extern EBBRep * App_createRep(CObjEBBRootMultiRef _self);

#define APP_BASE(REPTYPE)                                      \
EBBRep * App_createRep(CObjEBBRootMultiRef _self)              \
{                                                              \
  REPTYPE * repRef;                                            \
  EBBPrimMalloc(sizeof(REPTYPE), &repRef, EBB_MEM_DEFAULT);    \
  repRef->ft = &REPTYPE ## _ftable;                            \
  return (EBBRep *)repRef;                                     \
}

// defines if the application wants the start event on all
// cores or just one core
enum {
  APP_START_ONE,
  APP_START_ALL
};

extern const int app_start_model;

extern void EBB_init_default(void);
extern void create_app_obj_default(void);


extern EBBRC app_start(void);

#define APP_PROLOG				\
  EBBRC						\
  app_start(void)				\
  {						\
  EBBRC rc = EBBRC_OK;

#define APP_EPILOG				\
  return rc;					\
  }

#define APP_BODY \
  EBB_init_default();					\
  create_app_obj_default();				\
  return COBJ_EBBCALL(theAppId, start);

#define APP_START_ONE(REPTYPE)			\
  APP_PROLOG					\
  if (MyEventLoc() == 0) {			\
    APP_BODY					\
      }						\
  APP_EPILOG					\
  APP_BASE(REPTYPE)

#define APP_START_ALL(REPTYPE)			\
  APP_PROLOG					\
  APP_BODY					\
  APP_EPILOG					\
  APP_BASE(REPTYPE)


#endif
