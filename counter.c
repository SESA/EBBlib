/* Copyright 2011 Boston University. All rights reserved. */

/* Redistribution and use in source and binary forms, with or without modification, are */
/* permitted provided that the following conditions are met: */

/*    1. Redistributions of source code must retain the above copyright notice, this list of */
/*       conditions and the following disclaimer. */

/*    2. Redistributions in binary form must reproduce the above copyright notice, this list */
/*       of conditions and the following disclaimer in the documentation and/or other materials */
/*       provided with the distribution. */

/* THIS SOFTWARE IS PROVIDED BY BOSTON UNIVERSITY ``AS IS'' AND ANY EXPRESS OR IMPLIED */
/* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND */
/* FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BOSTON UNIVERSITY OR */
/* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR */
/* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR */
/* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF */
/* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

/* The views and conclusions contained in the software and documentation are those of the */
/* authors and should not be interpreted as representing official policies, either expressed */
/* or implied, of Boston University */

#include <sdi/sdi.h>

#include "ebb.h"
#include "counter.h"

static void Counter_inc (Counter *self) {
/*   assert(self && self->val); */

  *self->val += 1; //should be atomic

  return;
}

static void Counter_dec (Counter *self) {
/*   assert(self && self->val); */
  
  *self->val -= 1; //should be atomic

  return;
}

static int Counter_val (Counter *self) {
/*   assert(self && self->val); */
 
  return *self->val;
}

Counter *new_counter() {
  int i, id, *val;
  Counter *c;
  
  val = alloc(sizeof(int));
  id = get_id();
  
  for (i = 0; i < NUMPROCS; i++) {
    c = (Counter *)&idtable[i][id];
    c->itf = alloc(sizeof(CounterInterface));
    c->itf->inc = Counter_inc;
    c->itf->dec = Counter_dec;
    c->itf->val = Counter_val;
    c->val = val;
  }

  return (Counter *)(id*sizeof(EBB_Object *));
}
