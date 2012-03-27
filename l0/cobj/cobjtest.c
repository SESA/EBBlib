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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "cobj.h"

// This is a simple standalone 'C' illustration of how the 'C Object' 
// infrastructure can be used.  To compile:
//  gcc -Wall cobjtest.c -o cobjtest
//  ./cobjtest
//  or relying on default make rules you can do make cobjtest

// FIXME: JA: Someone should actually test the following code ;-)
// The following is an example of the how one might use the 
// ContainingOC macro
//
// THIS EXAMPLE IS A LITTE OVERLOADED ;-) Sorry.  It actually illustrates
// an object (A) that implements two interfaces (A) and (B) and can be 
// reference by pointers to either type.  To do this it embeddes an empty 
// (B) instance inorder to contain a  sole vtable pointer to the 
// B interface that is setup to point to the appropriate offset 
// in its main vtable

CObject(B) {
  CObjInterface(B) *ft;
};  

CObjInterface(B) {
  intptr_t (*trans)(BRef _self, intptr_t key); 
};

CObject(A) {
  CObjInterface(A) *ft;
  intptr_t base;
  uintptr_t mask;
  CObjectDefine(B) b;
};

CObjInterface(A) {
  intptr_t  (*getBase) (ARef _self);
  intptr_t  (*setBase) (ARef _self, intptr_t b);
  uintptr_t (*getMask) (ARef _self);
  uintptr_t  (*setMask) (ARef _self, uintptr_t m);
  CObjImplements(B);
};

static intptr_t 
A_getBase(ARef self) 
{
  return self->base; 
}

static intptr_t 
A_setBase(ARef self, intptr_t b) 
{
  intptr_t old = self->base;
  self->base = b; 
  return old;
}

static uintptr_t 
A_getMask(ARef self) 
{
  return self->mask; 
}

static uintptr_t 
A_setMask(ARef self, uintptr_t m) 
{
  uintptr_t old = self->mask;
  self->mask = m; 
  return old;
}

static intptr_t 
A_B_trans(BRef _self, intptr_t key) 
{ 
  ARef self = ContainingCOPtr(_self,A,b); // convert BRef into ARef
  return self->base + (intptr_t)(((uintptr_t)key & self->mask));
} 

CObjInterface(A) A_ftable = { 
  .getBase = A_getBase,
  .setBase = A_setBase,
  .getMask = A_getMask,
  .setMask = A_setMask,
  {
    .trans = A_B_trans
  }
}; 

static inline void
ASetFT(ARef a) 
{ 
  a->ft = &A_ftable; 
  a->b.ft = &(A_ftable.B_if); 
}
 
int 
ACreate(ARef *a_ptr, BRef *b_ptr, intptr_t b, uintptr_t m) {
  ARef aref;
  aref = (ARef)malloc(sizeof(ARef));
  aref->base = b; aref->mask = m;
  ASetFT(aref);
  *a_ptr = aref; 
  *b_ptr = &(aref->b);
  return 1;
}

int
main(int argc, char **argv)
{
  ARef a;
  BRef b;
  
  ACreate(&a, &b, 0xdeadbeef, 0x3);

  // You may now refrence the instance either by :
  //   the A Interface via the a_ptr pointer
  //   or by the B interface by the b_ptr.  

  printf("base=%ld mask=0x%lx trans(0xdead)=%ld\n", 
	 a->ft->getBase(a), a->ft->getMask(a),
	 b->ft->trans(b, 0xdead));

  return 1;
}

