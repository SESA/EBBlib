#ifndef _E1000_H_
#define _E1000_H_

#include <l4hal/types.h>

extern u8 e1000_bus;
extern u8 e1000_slot;
extern u16 e1000_func;

void e1000_init(void);
void e1000_reset(void);
void e1000_configure_tx(void);
void e1000_configure_rx(void);
sval e1000_read(void *buf, uval len);
sval e1000_write(void *buf, uval len);

#endif
