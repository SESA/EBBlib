#ifndef __ULNX_ETH_LIB_H__
#define __ULNX_ETH_LIB_H__

sval ethlib_nic_init(char *dev, lrt_pic_src *s);
sval ethlib_nic_readpkt(void);

#endif
