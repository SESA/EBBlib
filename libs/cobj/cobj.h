#ifndef __C_OBJ_H__
#define __C_OBJ_H__

// cObj: CObjects A simple C component infrastructure 
//   There is no attempt to support complex OOP concepts or abstractions
//   rather just basic support for creating components that define 
//   a vtable of functions and associated data.   There is no direct support
//   for either interface or implementation inheritance.  However, there
//   is obvious ways to create vtables which are aggregates.  You may of course
//   use your own conventions to support inheritance.

#define CObjInterface(name) struct name ## _if 
#define CObjFunc(func, ...) (*func) (void *, ##__VA_ARGS__)
#define CObjImplements(name) struct name ## _if name ## _if 

#define CObjData(name) struct name ##_data


#define CObject(name)				\
  typedef struct name ## _obj name;		\
  typedef name * name ## Ref;			\
  struct name ## _obj 				

//#define CObjInterfacesBegin struct vtable
//#define CObjInterfacesEnd * _vtable


// #define ObjData struct data 

#define COBJ_CALL(o,f, ...) (o->_vtable->f(o, ##__VA_ARGS__))


#endif
