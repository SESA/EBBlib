#ifndef __LRT_EVENT_LOC_H__
#define __LRT_EVENT_LOC_H__
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

/*
 * You need to be able to get the event location of the node you are
 * running on.  I am not making this a function on EventMgr, but
 * rather a global function so its clear that its the EL of the
 * current core and not the EL of the rep of the EventMgr
 * 
 * For now, one-to-one mapping event location and pic location. Since
 * this may change, we include the corresponding header file here.
 */
typedef int lrt_event_loc;

// this is used for any static sized arrays; should try to get rid of it
#define LRT_MAX_EL 64

// can't put protype for this, since inline on some platforms
// extern lrt_event_loc lrt_my_event_loc();
extern lrt_event_loc lrt_next_event_loc(lrt_event_loc l);
extern lrt_event_loc lrt_num_event_loc();

#ifdef LRT_ULNX
#include <l0/lrt/ulnx/event_loc.h>
#elif LRT_BARE
#include <l0/lrt/bare/event_loc.h>
#endif

#endif
