typedef uintptr_t node_t;
typedef uintptr_t EL;

node_t myNode(void);
EL myEL(void);

typedef uintptr_t id_t;
typedef id_t key_t;

typedef struct value * value_t;

struct value {
  union {
    uint64_t    raw;
    uintptr_t   v;
    uintptr_t (*op)(uintptr_t arg);
  } w0;
  union {
    uint64_t  raw;
    uintptr_t arg;
  } w1;
  union {
    uint64_t  raw;
    uintptr_t v; // 
    node_t    home; // CACHE A HOME
  } w2;
  union {
    uint64_t  raw;
    uintptr_t v;
    void    (*replicate)(value_t v);
  } w3;
}; 

enum {NODE_NULL, SELF_NODE, MC_NODE};
node_t home(key_t k);
value_t getValue(key_t k);

// An entry has associated with it an operation that can be performed locally
// for an id.  The operation to be taken is lazily determined.  Initially
// the operation is 0. Every Id has at least one 'home' node that can be 
// determined via the id's 'home' function.  The home function is also lazily resolved
// via a default home lookup function.   The operation can be replicated from it's home
// node by the value's replicated function.  All functions may choose to cache
// a NULL or actually value to control if the assoicated function should be taken 
// repeately.  
replicate(id_t id, value_t v) 
{
  // home nodes shouls always have a non-zero replicate field
  if (v->w3.replicated) return v->w3.replicate(id, v, myNode());
  else {
    node_t h;
    if (v->w2.home) h = v->w2.home(id, v);
    else h = defhome(id, v);
    // now that we have a home ask it to replicate the value
    return remoteReplicate(h, id, v, myNode());
  }
}

uintptr_t 
doOp(id_t id) 
{
  uintptr_t rc;
  key_t k = id;
  value_t v;

  v = getValue(k);

 retry:
  if (v->v0.v) {
    // hit
    rc = v->v0.op(v->v1.arg);
  } else {
    // miss
    if (replicate(id, v)) goto retry;
  }
  return rc;
}
