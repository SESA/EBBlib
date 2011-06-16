#include <ebb.h>
#include <types.h>
#include <SysStatus.h>

// include local version, so get from build and not install tree
#include "ThinWireChan.h"

struct ThinWireChanData_struct {
  /* put here the information needed to get to ethernet */
};


/*
 * writes data to a channel, e.g., socket, raw ethernet...
 */
static int
write_chan(ThinWireChanData *self, char *msg, int len) 
{
  tassertMsg(0, "NYI\n");
  // this routine will change to read/write to other channels
  return -1;
}


/*
 * reads data from a channel, e.g., socket, raw ethernet...
 */
static int
read_chan(ThinWireChanData *self, char *msg, int len) 
{
  tassertMsg(0, "NYI\n");
  // this routine will change to read/write to other channels
  return -1;
}

static SysStatusUval 
thw_write(ThinWireChanData *self, ChannelID channel, const char* msg, int length)
{
  char header[5];
  int lensent, outlen, i, inlen;
  
  header[0] = '0';     //'0'indicates "write"
  header[1] = ' ' + ((length >> 12) & 0x3f);
  header[2] = ' ' + ((length >>  6) & 0x3f);
  header[3] = ' ' + ((length >>  0) & 0x3f);
  header[4] = ' ' + channel;
  
  lensent = write_chan(self, header, 5);
  tassertMsg((lensent == 5), "oops, header write not five %d\n", lensent);
  
  outlen = length;
  while (outlen > 0) {
    
    i = write_chan(self, (char *)msg, outlen);
    tassertMsg((i>0), "thinwire write failed with %d\n", i);
    
    outlen -= i;
    msg    += i;
  }
  
  // read the ack
  memset(header, 0, 5);
  i = 0;
  do {
    int j = read_chan(self, &header[i], 5 - i);
    i += j;
  } while (i < 5);
  
  tassertMsg((header[4] - ' ') == channel, "thinwireWrite: expected ack on channel %d, got %d\n",
	     channel, (header[4] - ' '));
  
  inlen = ((header[1] - ' ') << 12) |
    ((header[2] - ' ') <<  6) |
    ((header[3] - ' ') <<  0);
  tassertMsg((inlen == length), "thinwireWrite:  expected ack length %d, got %d\n",
	     length, inlen);
  return length;
}

static SysStatusUval 
thw_read(ThinWireChanData *self, ChannelID channel, char* msg, uval length)
{
  int i, inlen, lensent;
  char header[5];
  header[0] = 'A'; // 'A' indicates "read"
  header[1] = ' ' + ((length >> 12) & 0x3f);
  header[2] = ' ' + ((length >>  6) & 0x3f);
  header[3] = ' ' + ((length >>  0) & 0x3f);
  header[4] = ' ' + channel;

  lensent = write_chan(self, header, 5);
  tassertMsg((lensent == 5), "oops, header write not five %d\n", lensent);

  i = 0;
  do {
    i += read_chan(self, &header[i], 5 - i);
  } while (i < 5);


  tassertMsg((header[0]-'0') == channel, "thinwireWrite: expected ack on channel %d, got %d\n",
	     channel, (header[4] - ' '));

  inlen = ((header[1] - ' ') << 12) |
    ((header[2] - ' ') <<  6) |
    ((header[3] - ' ') <<  0);


  tassertMsg((inlen <= length), "thinwireWrite:  expected length %d, got %d\n",
	     (int)length, inlen);

  length = inlen;
  while (length > 0) {
    i = read_chan(self, msg, length);
    tassertMsg((i>=0), "read_chan failed %d\n", i);
    length -= i;
    msg    += i;
  }

  return inlen;
}

void 
theThinWireChanInit()
{
  // initialize the thinwire channel
  // assign reference
  // assign function pointers
  // without late binding, fill all the tables with the reference
}


