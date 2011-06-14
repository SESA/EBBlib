#include "ebb.h"
#include "counter.h"

struct CounterData_struct {
  Counter *self;
  int val;
};

static void Counter_inc (CounterData *data) {

  data->val += 1; //should be atomic

  return;
}

static void Counter_dec (CounterData *data) {
  
  data->val -= 1; //should be atomic

  return;
}

static int Counter_val (CounterData *data) {
  return data->val;
}

#ifdef L4

static CounterInterface root_counter_itf;
static CounterData root_counter_data;

Counter *new_counter() {
  int i;

  //should be buying an ID here, but we're just gonna steal it for now
  Counter *counter = (Counter *)0;

  root_counter_itf.inc = Counter_inc;
  root_counter_itf.dec = Counter_dec;
  root_counter_itf.val = Counter_val;

  root_counter_data.self = counter;
  root_counter_data.val = 0;

  ROOT(counter)->itf = &root_counter_itf;
  ROOT(counter)->data = &root_counter_data;
  
  //no late binding
  for (i = 0; i < NUM_VCPUS; i++) {
    REP(counter,i)->itf = ROOT(counter)->itf;
    REP(counter,i)->data = ROOT(counter)->data;
  }

  return counter;
}

#endif
