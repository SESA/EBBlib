//From A2 Processor User's Manual
// Preliminary 1.03 page 294
typedef union {
  uint32_t val;
  struct {
    uint32_t cm :1; //computation mode
    uint32_t reserved0 :2;
    uint32_t gs :1; //guest state
    uint32_t reserved1 :1;
    uint32_t ucle :1; //user cache locking enable
    uint32_t spv :1; //vector available
    uint32_t reserved2 :7;
    uint32_t ce :1; //critical enable
    uint32_t reserved3 :1;
    uint32_t ee :1; //external enable
    uint32_t pr :1; //problem state
    uint32_t fp :1; //floating point avilable
    uint32_t me :1; //machine check enable
    uint32_t fe0 :1; //FP Exception Mode 0
    uint32_t reserved4 :1;
    uint32_t de :1; //debug interrupt enable
    uint32_t fe1 :1; //FP Exception Mode 1
    uint32_t reserved5 :2;
    uint32_t is :1; //instruction address space
    uint32_t ds :1; //data address space
    uint32_t reserved6 :4;
  };
} msr;

STATIC_ASSERT(sizeof(msr) == 4, "msr packing issue");
