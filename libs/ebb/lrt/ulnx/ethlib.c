#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <assert.h>
#include <pthread.h>

#include "../../../base/types.h"
#include "pic.h"

#define PCAP_DEFAULT_SNAPLEN 1500

pcap_t *hdl;

static 
void packet_callback(unsigned char *Args,
                      const struct pcap_pkthdr* Pkthdr,
                      unsigned char *Packet)
{
  fprintf(stderr, "+"); fflush(stderr);
}


// fixme with real args
sval
ethlib_nic_readpkt(void) {
  pcap_dispatch(hdl, 1, (void *) packet_callback, NULL);
  return 1;
}

sval
ethlib_nic_init(char *dev, lrt_pic_src *s)
{
  uval i;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_if_t *devs, *d;
 

  errbuf[0]=0;

  if (pcap_findalldevs(&devs, errbuf) || devs == NULL) {
    fprintf(stderr, "ERROR: %s: %s\n", __func__, errbuf);
    return -1;
  }

  for (i=0,d=devs; d!=NULL; i++,d=d->next) {
    i++;
    fprintf(stderr, "d:%ld: %s\n", i, d->name);
  }
  
  pcap_freealldevs(devs);


  if (dev == NULL) dev="any";

  hdl = pcap_open_live(dev, PCAP_DEFAULT_SNAPLEN, 
			       0, 0, errbuf);
  assert(hdl);

  if(pcap_setnonblock(hdl, 1, errbuf) == 1) {
    fprintf(stderr, "Could not set device \"%s\" to non-blocking: %s\n", dev, 
	    errbuf);
    return -1;
  }

  *s = pcap_get_selectable_fd(hdl);

  assert(*s != -1);

  return 0;
}
