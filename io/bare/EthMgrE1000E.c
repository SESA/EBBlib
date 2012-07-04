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
#include <l0/EBBMgrPrim.h>
#include <lrt/io.h>
#include <io/bare/pci.h>
#include <io/EthMgr.h>
#include <io/bare/pci.h>

#ifndef LRT_ULNX
/*
 * For now, doing something brain damged, telling 
 * pci bus to give us info about the one nic we know
 * about.  We should probably move this outside of this
 * file and ask for a list of all NICs...
 */
EBBRC
EthMgrCreate(EthMgrId *id) 
{
  //struct pci_info pi;
  EBBRC rc=0;
  pci_print_all();

  //rc = pci_get_info(PCI_VENDOR_INTEL, PCI_INTEL_DEVID_E1000E, &pi);
  
  LRT_RCAssert(rc);
  return rc;
}
#endif
