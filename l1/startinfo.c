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
#include <config.h>

#include <stdint.h>
#include <stddef.h>

#include <l0/lrt/trans.h>
#include <l1/startinfo.h>
#include <lrt/startinfo.h>
#include <l0/MemMgrPrim.h>


EBBRC si_get_args(struct startinfo *si)
{
  int i;
  int sisize = lrt_startinfo_size();
  char *siptr = (char *)lrt_startinfo_addr();
  char *edata, *data;
  intptr_t s;
  EBBRC rc;

  si->argc = 0;
  si->environc = 0;
  si->argv = NULL;
  si->environ = NULL;
  
  if (sisize >= sizeof(int)) {
      data = siptr;
      // we assume that startinfo is of the format we expect
      // FIXME: might want to do something fancier
      si->argc = *((int *)data); data += sizeof(int);
      s = sizeof(int);
      
      rc = EBBPrimMalloc(si->argc*sizeof(char *), 
			 &(si->argv), EBB_MEM_DEFAULT);
      LRT_RCAssert(rc);
      for (i=0; i<si->argc; i++) {
	si->argv[i] = data;
	while (*data != '\0') { data++; s++; }
	data++; s++;
      }
      
      si->environc=0;
      edata=data;
      while (s < sisize) {
	while (*edata != '\0') { edata++; s++; }
	edata++; s++; si->environc++;
      }
      // environc is one greater than number of entries 
      // we malloc this many so that we have space for the null 
      // terminating entry
      rc = EBBPrimMalloc(si->environc*sizeof(char *), &(si->environ), 
			 EBB_MEM_DEFAULT);
      LRT_RCAssert(rc);
      si->environc--;  // environc tracks number of non null entries
      edata = data;
      for (i=0; i<=si->environc; i++) {
	si->environ[i] = edata;
	while (*edata != '\0') edata++;
	edata++;
      }
      si->environ[i] = NULL; // put null in last extra entry
      LRT_Assert(s == sisize && i-1 == si->environc);
    }
  return 0;
}
