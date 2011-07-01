#ifndef __C_OBJ_H__
#define __C_OBJ_H__

// cObj: CObjects A simple C component infrastructure 
//   There is no attempt to support complex OOP concepts or abstractions
//   rather just basic support for creating components that define 
//   a vtable of functions and associated data.   There is no direct support
//   for either interface or implementation inheritance.  However, there
//   is obvious ways to create vtables which are aggregates.  You may of course
//   use your own conventions to support inheritance.

// typedef struct CObj { void * _ftable; }

/* #define CObjFtable(o) (o->_ftable) */
#define CObjIfName(name) name ## _if
#define CObjInterface(name) struct CObjIfName(name) 
#define CObjImplements(name) CObjInterface(name) CObjIfName(name) 

#define CObject(name)		    \
  typedef struct name ## _obj name; \
  typedef name * name ## Ref;	    \
  struct name ## _obj
      
/* #define COBJ_CALL(o,f, ...) (CObjFtable(o)->f(o, ##__VA_ARGS__)) */

#endif
