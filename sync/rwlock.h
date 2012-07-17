#ifndef SYNC_RWLOCK_H
#define SYNC_RWLOCK_H

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

#include <stdint.h>

#include <arch/cpu.h>

typedef union rwlock
{
  unsigned val;
  uint16_t val16;
  struct {
    uint8_t write;
    uint8_t read;
    uint8_t users;
  };
} rwlock;

static inline void
rwlock_init(rwlock *l)
{
  l->val = 0;
  __sync_synchronize();
}

static inline void
rwlock_wrlock(rwlock *l)
{
  rwlock temp;
  temp.val = 0;
  temp.users = 1;

  //Atomically acquire a ticket and increment the count
  unsigned me = __sync_fetch_and_add(&l->val, temp.val);

  temp.val = me;
  uint8_t val = temp.users;

  //wait until the write queue has our ticket number
  while (l->write != val)
    cpu_relax();
}

static inline void
rwlock_wrunlock(rwlock *l)
{
  rwlock temp = *l;

  __sync_synchronize();

  temp.write++;
  temp.read++;

  //we atomically increment the reader and writer queue numbers to let
  //the next user through whether they are a reader or a writer
  l->val16 = temp.val16;
}

static inline void
rwlock_rdlock(rwlock *l)
{
  rwlock temp;
  temp.val = 0;
  temp.users = 1;

  //Atomically acquire a ticket and increment the count
  unsigned me = __sync_fetch_and_add(&l->val, temp.val);

  temp.val = me;
  uint8_t val = temp.users;

  //Wait until the read queue has our ticket number
  while (l->read != val)
    cpu_relax();

  //We have the read lock, let the next user through if they are a reader
  l->read++;
}

static inline void
rwlock_rdunlock(rwlock *l)
{
  //we increment the write queue so if the next user is a writer,
  //they get through once all readers unlock
  __sync_add_and_fetch(&l->write, 1);
}

#endif
