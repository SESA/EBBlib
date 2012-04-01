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
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/lrt/pic.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>
#include <l1/App.h>
#include <io/lrt/LRTConsole.h>

static EBBRC
AllocAndBind(EventHandlerId * id, EBBRepRef repRef)
{
  EBBRC rc;
  CObjEBBRootSharedRef rootRef;

  rc = CObjEBBRootSharedCreate(&rootRef, repRef);
  EBBRCAssert(rc);

  rc = EBBAllocPrimId((EBBId *)id);
  EBBRCAssert(rc);

  rc = CObjEBBBind((EBBId)*id, rootRef); 
  EBBRCAssert(rc);

  return EBBRC_OK;
}

// Can't use EBBType as InAction causes recursive dependencies
// so we must manually forward declare all types 
CObject(CharStream) {
  CObjInterface(CharStream) *ft;
};
typedef CharStreamRef * CharStreamId;
typedef EBBRC (* InAction) (CharStreamId id);

CObjInterface(CharStream)
{
  EBBRC (*putChar)  (CharStreamRef _self, char c);
  EBBRC (*getChar)  (CharStreamRef _self, char *c);
  EVENTFUNC(inEvent);
  EVENTFUNC(outEvent);
};

// bad ugly test functions
static EBBRC
UglyPrintStr(CharStreamId id, char *buf)
{
  int i=0;
  EBBRC rc;

  while (buf[i]) {
    rc = COBJ_EBBCALL(id, putChar, buf[i]);
    EBBRCAssert(rc);
    i++;
  }

  return EBBRC_OK;
}

static EBBRC
UglyEcho(CharStreamId id)
{
  char c;

  while (COBJ_EBBCALL(id, getChar, &c) == EBBRC_OK) {
    COBJ_EBBCALL(id, putChar, c);
  }
  return EBBRC_OK;
}

enum {BUFSIZE=4096};

CObject(Console)
{
  COBJ_EBBFuncTbl(CharStream);

  lrt_pic_src indev;
  lrt_pic_src outdev;
  uintptr_t inEV;
  uintptr_t outEV;

  intptr_t inlen;
  intptr_t outlen;
  intptr_t outstart;
  char in_c[BUFSIZE];
  char out_c[BUFSIZE];

  CharStreamId actionId;
  InAction action;
};

typedef CharStreamId ConsoleId;

// FIXME: JA
// WARNING: THIS IS JUST EXAMPLE CODE BUFFING LOGIC IS PROBABLY ALL BULLSH!#$T 
// CERTAINLY NOT READY FOR TRUE COCURRENCY EITHER
static EBBRC
Console_putChar(CharStreamRef _self, char c)
{ 
  ConsoleRef self = (ConsoleRef)_self;
  EBBRC rc = EBBRC_RETRY;
  int end = self->outstart + self->outlen;

  if (end < BUFSIZE) {
    self->out_c[end] = c;
    self->outlen++;
    if (self->outlen==1) {
      COBJ_EBBCALL(theEventMgrPrimId, eventEnable, self->outEV);
    }
    rc = EBBRC_OK;
  }

  return rc;
}

static EBBRC
Console_getChar(CharStreamRef _self, char *c)
{
  ConsoleRef self = (ConsoleRef)_self;
  EBBRC rc = EBBRC_RETRY;

  if (self->inlen>0) {
    *c = self->in_c[self->inlen-1];
    self->inlen--;
    rc = EBBRC_OK;
  }

  return rc;
}

static EBBRC
Console_inEvent(CharStreamRef _self)
{
  ConsoleRef self = (ConsoleRef)_self;
  int n;
  EBBRC rc=EBBRC_OK;

  if (self->inlen < BUFSIZE) {
    rc=LRTConsoleRead(&(self->indev), 
		      &(self->in_c[self->inlen]), BUFSIZE - self->inlen, &n);
    EBBRCAssert(rc);
    if (n>0) self->inlen += n;
  }
 
  if (self->action!=NULL) rc = self->action(self->actionId);

  return rc;
}

static EBBRC
Console_outEvent(CharStreamRef _self)
{
  EBBRC rc;
  ConsoleRef self = (ConsoleRef)_self;
  int n;

  if (self->outlen>0) {
    rc = LRTConsoleWrite(&(self->outdev), &(self->out_c[self->outstart]),
			 self->outlen, &n);
    EBBRCAssert(rc);
    if (n>0) {
      self->outlen -= n;
      self->outstart += n;
      if (self->outlen==0) {
	self->outstart=0;
	COBJ_EBBCALL(theEventMgrPrimId, eventDisable, self->outEV);
      }
    }
  }

  return EBBRC_OK;
}

CObjInterface(CharStream) Console_ftable = {
  .putChar  = Console_putChar,
  .getChar  = Console_getChar,
  .inEvent  = (GenericEventFunc)Console_inEvent,
  .outEvent = (GenericEventFunc)Console_outEvent
};

void
ConsoleSetFT(ConsoleRef o)
{
  o->ft = &Console_ftable;
}

EBBRC
ConsoleCreate(ConsoleId *id, lrt_pic_src in, lrt_pic_src out, InAction action)
{
  ConsoleRef repRef;
  EBBRC rc;

  rc = EBBPrimMalloc(sizeof(Console), &repRef, EBB_MEM_DEFAULT);
  EBBRCAssert(rc);

  ConsoleSetFT(repRef);

  repRef->inlen    = 0;
  repRef->outlen   = 0;
  repRef->outstart = 0;
  repRef->indev    = in;
  repRef->outdev   = out;

  rc = AllocAndBind((EventHandlerId *)id, (EBBRepRef)repRef);
  EBBRCAssert(rc);

  repRef->action = action;
  repRef->actionId = *id;

  // setup up input event handling
  rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &(repRef->inEV));
  EBBRCAssert(rc);
  rc = COBJ_EBBCALL(theEventMgrPrimId, registerHandler, repRef->inEV, 
		    (EventHandlerId) *id, COBJ_FUNCNUM(repRef, inEvent), &in);
  EBBRCAssert(rc);

  // setup up output handling
  rc = COBJ_EBBCALL(theEventMgrPrimId, allocEventNo, &(repRef->outEV));
  EBBRCAssert(rc);
  rc = COBJ_EBBCALL(theEventMgrPrimId, registerHandler, repRef->outEV, 
		    (EventHandlerId) *id, COBJ_FUNCNUM(repRef, outEvent), 
		    &out);
  EBBRCAssert(rc);

  // now enable input handing as we are ready to roll
  rc = COBJ_EBBCALL(theEventMgrPrimId, eventEnable, repRef->inEV);
  EBBRCAssert(rc);

  return EBBRC_OK;
}

CObject(ConsTst) {
  CObjInterface(App) *ft;
};


static EBBRC 
ConsTst_start(AppRef _self, int argc, char **argv, 
		 char **environ)
{
  EBBRC rc;
  lrt_pic_src in, out, err;
  ConsoleId id;

  rc = LRTConsoleInit(&in,&out,&err);
  EBBRCAssert(rc);

  rc = ConsoleCreate(&id, in, out, UglyEcho);
  EBBRCAssert(rc);

  UglyPrintStr(id, "ConsTst: Hello World!!!!\n"
	       "Will now continue to echo keystrokes from console input"
	       " to output:\n");

  return EBBRC_OK;
}

CObjInterface(App) ConsTst_ftable = {
  .start = ConsTst_start
};

APP(ConsTst);

