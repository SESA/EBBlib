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

#include <arch/powerpc/asdef.h>
#include <arch/powerpc/regs.h>

#define EX_LOOP(interrupt) \
	.balign 32 		; \
	_IVPR_ ## interrupt: 	; \
	b _IVPR_ ## interrupt

.section .isr, "ax"
.balign (1 << 16)
.globl _vec_start
_vec_start:	
	EX_LOOP(machine_check)
	EX_LOOP(critical_input)
	EX_LOOP(debug) 
	EX_LOOP(data_storage) 
	EX_LOOP(instruction_storage) 
	EX_LOOP(external) 
	EX_LOOP(alignment) 
	EX_LOOP(program) 
	EX_LOOP(floating_point_unavailable) 
	EX_LOOP(system_call) 
	EX_LOOP(auxiliary_processor_unavailable) 
	EX_LOOP(decrementer) 
	EX_LOOP(fixed_interval_timer) 
	EX_LOOP(watchdog_timer) 
	EX_LOOP(data_tlb_error) 
	EX_LOOP(instruction_tlb_error) 
	EX_LOOP(vector_unavailable) 
	EX_LOOP(embedded_floating_point_data)
	EX_LOOP(embedded_floating_point_round)
	EX_LOOP(embedded_performance_monitor)
	EX_LOOP(processor_dbell	) 
	EX_LOOP(processor_dbell_critical)
	EX_LOOP(guest_processor_dbell) 
	EX_LOOP(guest_processor_dbell_critical)
	EX_LOOP(ehv_system_call) 
	EX_LOOP(ehv_privilege) 
	EX_LOOP(lrat_error) 
_vec_end:	
