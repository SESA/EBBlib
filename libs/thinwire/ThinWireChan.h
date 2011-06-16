#ifndef __THINWIRECHAN_H_
#define __THINWIRECHAN_H_
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


/*****************************************************************************
 * Module Description: Does communication over a multiplexed thin-wire
 * connection.  Right now, we just send through the command, we don't
 * even have locking. Later will add multiplexing, de-multiplexing,
 * only do blocking operation if data is there... all supported by
 * thinwire protocol.

 *
 **************************************************************************/
  
typedef enum _channels {
  CONSOLE_CHANNEL = 0,
  GDB_CHANNEL = 1,
  IPCUT_CHANNEL = 2,
  ZIGGYDEB_CHANNEL = 3,
  IPSEL_CHANNEL = 4,
  NUM_CHANNELS = 5 
} ChannelID;
  
typedef struct ThinWireChan_struct ThinWireChan; 
typedef struct ThinWireChanData_struct ThinWireChanData;


typedef struct ThinWireChanInterface_struct {
  SysStatusUval (*write) (ThinWireChanData *self, ChannelID channel, const char* buf, int length);
  SysStatusUval (*read) (ThinWireChanData *self, ChannelID channel, char* buf, uval length);
} ThinWireChanInterface;

struct ThinWireChan_struct {
  ThinWireChanInterface *itf;
  ThinWireChanData *data;
};

/* this is a weird object, since there is only one of these, and it will have a well known ID */
extern void theThinWireChanInit(void);
extern ThinWireChan *theThinWireChan;


#endif // __THINWIRECHAN_H_
