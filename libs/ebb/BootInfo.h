#ifndef __BOOT_INFO_H__
#define __BOOT_INFO_H__

#define BOOTINFOSIZE 4096

struct BootInfo {
  uval8 raw[BOOTINFOSIZE];
};

#endif
