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

#ifndef EBB_H
#define EBB_H

typedef struct EBB_Object_struct {
  void *itf;
  void *data;
} EBB_Object;

#ifndef NUM_VCPUS
#define NUM_VCPUS (4)
#endif

#ifndef NUM_EBB_IDS
#define NUM_EBB_IDS (100)
#endif

#ifdef L4

//one table for the root table
extern EBB_Object ebb_table[NUM_VCPUS+1][NUM_EBB_IDS];

#include <l4/types.h>
#include <l4/thread.h>

#define ROOT(obj)						\
  ((typeof(obj))(((L4_Word_t)obj) + ((L4_Word_t)ebb_table)))

#define REP(obj, i)							\
  ((typeof(obj))(((L4_Word_t)obj) + ((L4_Word_t)(ebb_table[i+1]))))

#define INVK(obj, fun, ...)						\
  ({									\
    typeof(obj) _obj = (obj);						\
    _obj = (typeof(_obj))(((L4_Word_t)_obj) + L4_UserDefinedHandle());	\
    _obj->itf->fun(_obj->data, ##__VA_ARGS__);				\
  })									\
  
#define INVK_ROOT(obj, fun, ...)					\
  ({									\
    typeof(obj) _obj = ROOT(obj);					\
    _obj->itf->fun(_obj->data, ##__VA_ARGS__);				\
  })									\
  
#endif

#endif
