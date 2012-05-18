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

#include <l0/lrt/event.h>
#include <l0/lrt/bare/arch/amd64/lrt_start.h>
#include <lrt/io.h>

lrt_event_loc
lrt_num_event_loc()
{
  return 1;
}

lrt_event_loc
lrt_next_event_loc(lrt_event_loc l)
{
  return 0;
}

void *
lrt_event_init(void *myloc)
{
  lrt_start();
  lrt_printf("entering event loop!");
  while (1)
    ;
}

void
lrt_event_bind_event(lrt_event_num num, lrt_trans_id handler,
                     lrt_trans_func_num fnum)
{
  LRT_Assert(0);
}

void
lrt_event_trigger_event(lrt_event_num num, enum lrt_event_loc_desc desc,
                        lrt_event_loc loc)
{
  LRT_Assert(0);
}

void lrt_event_route_irq(struct IRQ_t *isrc, lrt_event_num num,
                         enum lrt_event_loc_desc desc, lrt_event_loc loc)
{
  LRT_Assert(0);
}
