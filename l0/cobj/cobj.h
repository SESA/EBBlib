#ifndef __C_OBJ_H__
#define __C_OBJ_H__
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

#define CObjectDefine(name)			\
  struct name ## _obj
      
/* #define COBJ_CALL(o,f, ...) (CObjFtable(o)->f(o, ##__VA_ARGS__)) */

// The folloing macro can be used to obtain a pointer to an object that 
// contains another object:
//      addr is the this pointer that you currently know
//      ctype is the type of the outer or containing object
//      field is the field name of interior object in the containing
//            object.
// FIXME: JA: Not really happy with this name or its semantics exactly
//            but for the moment it will allow us to explore nested 
//            objects
#define ContainingCOPtr(addr, ctype, field) \
  ((ctype *)(((uintptr_t)addr) - (__builtin_offsetof(ctype, field))))

#endif
