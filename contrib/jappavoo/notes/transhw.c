typedef uint64_t trans_node_t;

extern trans_node_t TRANS_NODE_NULL();
extern trans_node_t TRANS_NODE_MYNODE();
extern trans_node_t TRANS_NODE_SELF(); // special pseudonym for talk to yourself

// inspired by dhash and VMMHW
typedef uint64_t trans_key_t;

typedef union {
  uint128_t raw;
  struct {
    uint64_t w0;
    uint64_t w1;
  };
} trans_value_t;
const uint128_t VALUE_NULL=0;

typedef union  {
  uint128_t raw;
  struct {
    uint64_t w0;
    uint64_t w1;
  };
} trans_info_t;

typedef struct {
  trans_key_t   k;
  trans_value_t v;
  trans_info_t  i;
} trans_entry_t;

// TransHW is model around COSMgr/Chord like ring
// in reality may be a tree or more complex structure
// that reflects physical locality
typedef struct {
  trans_node_t   next;
  trans_node_t   prev;
  trans_node_t  *fingertable;
} trans_ring_t;

typedef struct {
  trans_key_t start;
  trans_key_t end;
} trans_key_span_t;

struct TransHW {
  trans_entry_t    entries[TRANS_TE_NUM];
  trans_key_span_t span;
  trans_ring_t     ring;
  trans_node_t     node;
} transhw;

void
trans_init(void)
{
  bzero(transhw, sizeof(transhw));
  transhw.node = TRANS_NODE_MYNODE();
  // initialize to be a self contained ring
  transhw.ring.next = TRANS_NODE_MYNODE();
  transhw.ring.prev = TRANS_NODE_MYNODE();
}

trans_ring_t trans_ring_get(node_t node);
void trans_ring_set(node_t node, trans_ring_t ring);
trans_ring_insert(node_t peer);
trans_node_t trans_find_home(key k);

void
trans_startup(node_t peer)
{
  trans_init();
  trans_ring_insert(peer);
}




// HOMES are responsible for computing / replicating a 
// value.  An key's value may be unique on each node.
// map ID->TE
// ID ->  HOMES, REPLICAS, RV, HV

trans_value_t * trans_find_or_allocate_and_lock(trans_key_t key)
{
  trans_entry_t *e;
  
 retry:
  e = find_local(key);
  if (e) {
    // Possible Local Hit
    lock(e);
    if (match(e, key)) {
      return value(e);
    } else {
      unlock(e);
      goto localretry;
    }
  } else {
    // find a home node for this id and see what it has to say
    trans_node_t home;	
    home = trans_find_home(id);  // eg. possible lookup via a Chord/COSMgr like alg
  }
}
