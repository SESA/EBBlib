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
#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <assert.h>
#include <pthread.h>

#include <stdint.h>
#include <l0/lrt/pic.h>

enum {PCAP_DEFAULT_SNAPLEN = 1500};

pcap_t *hdl;

static 
void packet_callback(unsigned char *Args,
                      const struct pcap_pkthdr* Pkthdr,
                      unsigned char *Packet)
{
  fprintf(stderr, "+"); fflush(stderr);
}


// fixme with real args
intptr_t
ethlib_nic_readpkt(void) {
  pcap_dispatch(hdl, 1, (void *) packet_callback, NULL);
  return 1;
}

intptr_t
ethlib_nic_init(char *dev, lrt_pic_src *s)
{
  int i;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_if_t *devs, *d;
 

  errbuf[0]=0;

  if (pcap_findalldevs(&devs, errbuf) || devs == NULL) {
    fprintf(stderr, "ERROR: %s: %s\n", __func__, errbuf);
    return -1;
  }

  for (i=0,d=devs; d!=NULL; i++,d=d->next) {
    i++;
    fprintf(stderr, "d:%d: %s\n", i, d->name);
  }
  
  pcap_freealldevs(devs);


  if (dev == NULL) dev="any";

  hdl = pcap_open_live(dev, PCAP_DEFAULT_SNAPLEN, 
			       0, 0, errbuf);

  if (hdl == NULL) {
    fprintf(stderr, "ERROR: pcap_open_live on \"%s\" failed: %s\n", 
	    dev, errbuf);
    return -1;
  }

  if(pcap_setnonblock(hdl, 1, errbuf) == 1) {
    fprintf(stderr, "Could not set device \"%s\" to non-blocking: %s\n", dev, 
	    errbuf);
    return -1;
  }

  *s = pcap_get_selectable_fd(hdl);

  assert(*s != -1);

  return 0;
}
