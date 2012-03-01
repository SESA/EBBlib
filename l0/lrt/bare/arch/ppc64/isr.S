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

#include <arch/ppc64/asdef.h>
#include <arch/ppc64/regs.h>

//exactly 8 opcodes means this is the right size
#define EX_JUMP(interrupt)	\
	_IVPR_ ## interrupt:	; \
	nop			; \
	mtspr	SPRN_SPRG0, r20	; \
	mfctr	r20		; \
	mtspr	SPRN_SPRG1, r20 ; \
	lis	r20, int_table@h	; \
	ld	r20, (IV_ ## interrupt) * 8(r20) ; \
	mtctr	r20			       ; \
	bctr
	
.section .isr, "ax"
.globl _vec_start
_vec_start:
	EX_JUMP(machine_check)
	EX_JUMP(critical_input)
	EX_JUMP(debug) 
	EX_JUMP(data_storage) 
	EX_JUMP(instruction_storage) 
	EX_JUMP(external) 
	EX_JUMP(alignment) 
	EX_JUMP(program) 
	EX_JUMP(floating_point_unavailable) 
	EX_JUMP(system_call) 
	EX_JUMP(auxiliary_processor_unavailable) 
	EX_JUMP(decrementer) 
	EX_JUMP(fixed_interval_timer) 
	EX_JUMP(watchdog_timer) 
	EX_JUMP(data_tlb_error) 
	EX_JUMP(instruction_tlb_error) 
	EX_JUMP(vector_unavailable) 
	EX_JUMP(embedded_floating_point_data)
	EX_JUMP(embedded_floating_point_round)
	EX_JUMP(embedded_performance_monitor)
	EX_JUMP(processor_dbell	) 
	EX_JUMP(processor_dbell_critical)
	EX_JUMP(guest_processor_dbell) 
	EX_JUMP(guest_processor_dbell_critical)
	EX_JUMP(ehv_system_call) 
	EX_JUMP(ehv_privilege) 
	EX_JUMP(lrat_error) 
	.fill ((_vec_start + 0x1000 - .) / 4), 4, 0x200
_vec_end:	
	