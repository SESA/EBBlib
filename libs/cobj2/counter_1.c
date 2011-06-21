#include "../types.h"
#include "counter_1.h"

static void
Counter_1_inc(void * _self) {
  Counter_1Ref self = _self;
  self->value++;
}

static void
Counter_1_dec(void * _self) {
  Counter_1Ref self = _self;
  self->value--;
}

static uval
Counter_1_val(void * _self) {
  Counter_1Ref self = _self;
  return self->value;
}

CObjInterface(Counter) Counter_1_ftable = {
  Counter_1_inc, Counter_1_dec, Counter_1_val
};
