#include <arch/ppc32/bpcore/ppc450_core.h>

.section ".text"
.globl _init_stack_bottom
.globl _init_stack_top

#define INIT_STACK_SIZE (4096)
_init_stack_bottom:
.space INIT_STACK_SIZE
_init_stack_top:

/*****************************************************************************
 *  Main entry point.
 */
  	.section ".text"
	.align	2
	.globl	_start
	.type	_start,@function
_start: 
	/* the 44x always has translation turned on.  We assume that
	the boot loader put sufficient number of TLB entries in so
		that we can boot-strap the kernel.  Interrupts are disabled. */

	/* set init MSR */
	lis	%r0, MSR_DISABLED@h
	ori	%r0, %r0,MSR_DISABLED@l
	mtmsr	%r0

	/* switch stack */
	lis	%r1, _init_stack_top@h
	ori	%r1, %r1,_init_stack_top@l

	mfspr	%r3, SPR_PIR
	cmpwi	%r3, 0
	bgt	wait_secondary

	bl	init

wait_secondary:
	b	.
.end
