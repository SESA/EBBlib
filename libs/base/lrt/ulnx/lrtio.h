#ifndef __ULNX_EBB_LRTIO_H__
#define __ULNX_EBB_LRTIO_H__

#include <unistd.h>
#include <stdio.h>

#define EBB_LRT_printf printf
#define EBB_LRT_write  write

#if 0
typedef struct in_addr  LrtNodeId;
typedef struct sin_port LrtNodeAddr;

lrt_write_node(LrtNodeId id, LrtNodeAddr addr, char *buf, uval *len);
lrt_read_node(LrtNodeId id, LrtNodeAddr addr, char *buf, uval *len);
#endif

#endif
