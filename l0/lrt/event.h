#ifndef __LRT_EVENT_H__
#define __LRT_EVENT_H__
/*
 * Copyright (C) 2012 by Project SESA, Boston University
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

#include <l0/lrt/trans.h>
#include <l0/lrt/event_irq.h>
#include <l0/lrt/event_loc.h>
#include <l0/lrt/event_num.h>

extern void *lrt_event_init(void *myloc);
extern void lrt_event_preinit(int cores);
extern void lrt_event_trigger_event(lrt_event_num num,
                                    enum lrt_event_loc_desc desc,
                                    lrt_event_loc loc);
extern void lrt_event_route_irq(struct IRQ_t *isrc,
                                lrt_event_num num,
                                enum lrt_event_loc_desc desc,
                                lrt_event_loc loc);
extern void lrt_event_altstack_push(uintptr_t);
extern uintptr_t lrt_event_altstack_pop(void);

#ifdef LRT_ULNX
#include <l0/lrt/ulnx/event.h>
#elif LRT_BARE
#include <l0/lrt/bare/event.h>
#endif

#endif
