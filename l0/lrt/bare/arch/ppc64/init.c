#include <stdint.h>

#include <arch/ppc64/cpu.h>
#include <arch/ppc64/regs.h>
#include <l0/lrt/bare/uart.h>
#include <lrt/assert.h>

extern uint8_t _vec_start[];
extern uint8_t _start[];

static inline void __attribute__ ((noreturn))
panic (void) {
  while(1)
    ;
}

static inline void
clear_bss(void) {
  extern uint8_t sbss[];
  extern uint8_t ebss[];
  for (uint8_t *i = sbss; i < ebss; i++) {
    *i = 0;
  }
}

/* I only load 16 bits so I need this to be aligned so that
   there are only 16 significant bits. So I confirm that it
   is within the first 4GB (no longer than 32 bits) */
void *int_table[27] __attribute__ ((aligned(1 << 16)));

STATIC_ASSERT(&int_table <= 0xFFFFFFFFULL, 
	      "int_table not linked at appropriate location");

typedef union {
  uint32_t val;
  struct {
    uint32_t type :5;
    uint32_t broadcast :1;
    uint32_t lpidtag :12;
    uint32_t pirtag :14;
  };
} doorbell;

extern char ipi[];
asm (
     ".globl ipi\n\t"
     "ipi:\n\t"
     "lis 20, lrt_start@highest\n\t"
     "ori 20, 20, lrt_start@higher\n\t"
     "rldicr 20, 20, 32, 31\n\t"
     "oris 20, 20, lrt_start@h\n\t"
     "ori 20, 20, lrt_start@l\n\t"
     "ld 2, 8(20)\n\t"
     "ld 20, 0(20)\n\t"
     "mtctr 20\n\t"
     "bctrl"
     );

static char *uart_addr = (char *)0xffc000c000ULL;

void __attribute__ ((noreturn))
lrt_start(void)
{  
  uart_write(uart_addr, "In int\n", 7);

  panic();
}

void __attribute__ ((noreturn))
init(void)
{
  /* setup IVPR */
  asm volatile (
  		"mtivpr %[_vec_start]"
  		:
  		: [_vec_start] "r" (_vec_start)
  		);

  //start at medium priority
  asm volatile ("or 2, 2, 2");

  //setup MSR
  msr msr;
  asm volatile (
		"mfmsr %[msr]"
		: [msr] "=r" (msr)
		);

  //enable machine check
  msr.me = 1;

  //make sure external interrupts are off
  msr.ee = 1;

  //enable fpu
  msr.fp = 1;

  //enable 64 bit mode
  msr.cm = 1;
  
  //set interrupt compute mode
  epcr epcr;
  epcr.val = 0;
  epcr.icm = 1;
  asm volatile (
		"mtspr %[sprn_epcr], %[epcr]"
		:
		: [sprn_epcr] "i" (SPRN_EPCR),
		  [epcr] "r" (epcr)
		);

  //write msr
  asm volatile (
		"mtmsr %[msr]"
		:
		: [msr] "r" (msr)
		);

  //attn on null pointer jump
  //FIXME: issue with gcc claiming it will never be NULL
  /* if (_start == 0) { */
  /*   asm volatile ( */
  /* 		  "stw %[attn], 0(0)\n\t" */
  /* 		  "dcbst 0, %[zero]\n\t" */
  /* 		  "sync\n\t" */
  /* 		  "icbi 0, %[zero]\n\t" */
  /* 		  "isync" */
  /* 		  : */
  /* 		  : [attn] "r" (0x200), //opcode for attn */
  /* 		  : [zero] "r" (0) */
  /* 		  ); */
  /* } */


  clear_bss();

  int_table[20] = ipi;

  uint32_t pir;
  asm volatile (
		"mfspr %[pir], %[sprn_pir]"
		: [pir] "=r" (pir)
		: [sprn_pir] "i" (SPRN_PIR)
		);

  doorbell db;
  db.val = 0;
  db.broadcast = 1;
  /* db.pirtag = pir; */

  asm volatile (
		"msgsnd %[db]"
		:
		: [db] "r" (db)
		);

  uart_write(uart_addr, "After int\n", 10);

  panic();
}
