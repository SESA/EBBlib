#ifdef SWINTERRUPTS
struct corebv {
  uint64_t vec[LRT_EVENT_NUM_EVENTS/64];
};

static struct corebv *pending; 

static void
set_bit_bv(struct corebv *bv, lrt_event_num num) 
{
  int word = num/64;
  uint64_t mask = (uint64_t)1 << num%64;
  __sync_fetch_and_or (&bv->vec[word], mask);
}

static int
get_unset_bit_bv(struct corebv *bv) 
{
  int word, bit, num;
  for (word = 0; word <LRT_EVENT_NUM_EVENTS/64 ; word++) {
    if( bv->vec[word] ) break;
  }
  if (word >= LRT_EVENT_NUM_EVENTS) return -1;
  
  // FIXME: use gcc builtin routines for this
  for (bit = 0; bit < 64; bit++) {
    uint64_t mask = (uint64_t)1 << bit;
    if (bv->vec[word] & mask) {
      // found a set bit
      uint64_t mask = ~((uint64_t)1 << bit);
      __sync_fetch_and_and(&bv->vec[word], mask);
      break;
    }
  }
  num = word * 64 + bit;
  return num;
};


static void
set_bit(lrt_event_loc loc, lrt_event_num num) 
{
  set_bit_bv(&pending[loc], num);
};

static lrt_event_num
get_unset_bit(lrt_event_loc loc) 
{
  return get_unset_bit_bv(&pending[loc]);
};
#endif
