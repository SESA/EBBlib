#include <arch/ppc64/asdef.h>

	.globl _start
	.section .init.text, "ax"
	.balign 8
_start:
	nop

	//load stack into r1
	lis	r1, _mini_stack@highest
	ori	r1, r1, _mini_stack@higher
	rldicr	r1, r1, 32, 31
	oris	r1, r1, _mini_stack@h
	ori	r1, r1, _mini_stack@l
	subi	r1, r1, 112 //ask about this

	//load init addr into r20
	lis	r20, init@highest
	ori	r20, r20, init@higher
	rldicr	r20, r20, 32, 31
	oris	r20, r20, init@h
	ori	r20, r20, init@l
	
	ld	r2, 8(r20) //load TOC value
	ld	r20, 0(r20) //now call it
	mtctr	r20
	bctrl
	attn

	.section .init.stack
	.align 16
	.space 0x4000
_mini_stack: