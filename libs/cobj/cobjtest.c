#include "../types.h"
#include "cobj.h"


#include <stdio.h>

// CObjInterface introduces a struct of name CObj_if
CObjInterface (CObj) {
  const char *CObjFunc(name);
  uval        CObjFunc(init);
};

CObjInterface (Counter) {
  void CObjFunc(inc);
  void CObjFunc(dec);
  uval CObjFunc(val);
};

CObjInterface (File) {
  uval CObjFunc(open, char *name, uval mode);
  uval CObjFunc(close);
  uval CObjFunc(read, char *buf, uval len); 
  uval CObjFunc(write, char *buf, uval len); 
};

CObject (MyObj) {
  CObjInterface (MyObj) {
    CObjImplements(CObj);
    CObjImplements(Counter);
    CObjImplements(File);  
    uval CObjFunc(Init, uval v);
    void CObjFunc(Print);
  } * _vtable;

  uval val;
  uval fd;
  uval offset;
  uval ocnt, ccnt,rcnt, wcnt;
};


const char * MyObj_CObj_name(void * _self) { return 0; }
uval         MyObj_CObj_init(void * _self) { return 1; }
void MyObj_Counter_inc(void * _self) { MyObj *self = _self; self->val++; }
void MyObj_Counter_dec(void * _self) { MyObj *self = _self; self->val--; }
uval MyObj_Counter_val(void * _self) { MyObj *self = _self; return self->val; }
uval MyObj_File_open(void * _self, char *name, uval mode) {MyObj *self = _self; self->ocnt++; return 1; }
uval MyObj_File_close(void * _self) { MyObj *self = _self; self->ccnt++; return 1; }
uval MyObj_File_read(void * _self, char *buf, uval len) { MyObj *self = _self; self->rcnt+=len; return 1; }
uval MyObj_File_write(void * _self, char *buf, uval len) { MyObj *self = _self; self->wcnt+=len; return 1; }

uval 
MyObj_Init(void * _self, uval v) 
{ 
  MyObj *self = _self; 
  self->val = v; 
  self->fd = 0; 
  self->offset= 0; 
  self->ocnt = self->ccnt = self->rcnt = self->wcnt = 0; 
}

void
MyObj_Print(void *_self)
{
  MyObj *self = _self;
  printf("MyObj_Print: _self = %p: val=%ld, fd=%ld, offset=%ld"
	 " rcnt=%ld wcnt=%ld ocnt=%ld ccnt=%ld\n", 
	 _self, self->val, self->fd, self->offset,
	 self->rcnt, self->wcnt, self->ocnt, self->ccnt);
}

struct MyObj_if MyObj_vtable = { 
  {MyObj_CObj_name, MyObj_CObj_init}, 
  {MyObj_Counter_inc, MyObj_Counter_dec, MyObj_Counter_val},
  {MyObj_File_open, MyObj_File_close, MyObj_File_read, MyObj_File_write},
  MyObj_Init,
  MyObj_Print
};

uval 
MyObj_Alloc(MyObjRef *o)
{
  // this is where you can add
  // Object specific memory allocation logic
  
  // For the moment MyObj only supports static allocations
  *o = NULL;
  return -1;
}

uval
MyObj_Prep(MyObjRef o)
{
  o->_vtable = &MyObj_vtable;
} 

int
main(int argc, char **argv)
{
  static MyObj  theObj;
  MyObjRef ref = &theObj;
  char buf[80];

  MyObj_Prep(ref);
  
  COBJ_CALL(ref, Print);

  COBJ_CALL(ref, Init, 10);
  COBJ_CALL(ref, CObj_if.init);
  COBJ_CALL(ref, Counter_if.inc);
  COBJ_CALL(ref, File_if.open, "testFile", 0); 
  COBJ_CALL(ref, File_if.write, "data", 4);
  COBJ_CALL(ref, File_if.read, buf, sizeof(buf));
  COBJ_CALL(ref, File_if.close);

  COBJ_CALL(ref, Print);  
  
  return 0;
}
