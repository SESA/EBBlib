/*
 * Copyright (C) 2011 by Project SESA, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <arch/ppc32/asdef.h>
#include <arch/ppc32/bpcore/ppc450_core.h>
	
	.globl _start
	.section .init.text, "ax"
	.balign 8
_start:
	nop

	mfspr	r21, SPRN_PIR
	cmpwi	r21, 0
	bgt	spin_secondary

	//load stack into r1
	lis	r1, _mini_stack@h
	ori	r1, r1, _mini_stack@l
	subi	r1, r1, 112 //ask about this

	//set small data areas to 0
	li	r13, 0
	li	r2, 0

	//load init addr into r20
	lis	r20, init@h
	ori	r20, r20, init@l
	
	mtctr	r20
	bctrl
	attn

entry_secondary: .long -1
spin_secondary:
	bl 	1f
1:
	mflr 	r1
	subi 	r1, r1, 8 //get entry word
2:
	dcbi	0, r1
	lwz	r2, 0(r1)
	addi	r3, r2, 1
	cmpwi	r3, 0
	beq	2b
	iccci	0, 0
	mtlr	r2
	blr
	
	.section .init.stack
	.align 16
	.space 0x4000
_mini_stack:
