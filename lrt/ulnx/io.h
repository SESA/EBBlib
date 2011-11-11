#ifndef __ULNX_EBB_IO_H__
#define __ULNX_EBB_IO_H__

#include <unistd.h>
#include <stdio.h>

#define EBB_LRT_printf printf
#define EBB_LRT_write  write

int
EBB_LRT_getBootInfoPage(char *, int *size);

#endif
