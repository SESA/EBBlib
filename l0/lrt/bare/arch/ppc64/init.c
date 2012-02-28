#include <stdint.h>

#include <arch/ppc64/cpu.h>

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
  msr.ee = 0;

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
		 [msr] "=r" (msr)
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

  panic();
}
