#include <stdio.h>
#include "../types.h"
#include "myobj.h"

static void
MyObj_Counter_inc(void * _self)
{
  MyObjRef self = _self; self->val++; 
}

static void
MyObj_Counter_dec(void * _self)
{
  MyObjRef self = _self; self->val--;
}

static uval 
MyObj_Counter_val(void * _self)
{
  MyObjRef self = _self; return self->val;
}

static uval 
MyObj_File_open(void * _self, char *name, uval mode) 
{
  MyObjRef self = _self; 
  self->ocnt++; 
  return 1; 
}

static uval 
MyObj_File_close(void * _self) 
{ 
  MyObjRef self = _self; 
  self->ccnt++; 
  return 1; 
}

static uval 
MyObj_File_read(void * _self, char *buf, uval len) 
{ 
  MyObjRef self = _self; 
  self->rcnt+=len; 
  return 1; 
}

static uval
MyObj_File_write(void * _self, char *buf, uval len) 
{ 
  MyObjRef self = _self; 
  self->wcnt+=len; 
  return 1; 
}

static uval 
MyObj_Init(void * _self, uval v) 
{ 
  MyObj *self = _self; 
  self->val = v; 
  self->fd = 0; 
  self->offset= 0; 
  self->ocnt = self->ccnt = self->rcnt = self->wcnt = 0; 
}

static void
MyObj_Print(void *_self)
{
  MyObj *self = _self;
  printf("MyObj_Print: _self = %p: val=%ld, fd=%ld, offset=%ld"
	 " rcnt=%ld wcnt=%ld ocnt=%ld ccnt=%ld\n", 
	 _self, self->val, self->fd, self->offset,
	 self->rcnt, self->wcnt, self->ocnt, self->ccnt);
}

/* static instance of default function table for object instances */
struct MyObj_if MyObj_ftable = { 
  {MyObj_Counter_inc, MyObj_Counter_dec, MyObj_Counter_val},
  {MyObj_File_open, MyObj_File_close, MyObj_File_read, MyObj_File_write},
  MyObj_Init,
  MyObj_Print
};
