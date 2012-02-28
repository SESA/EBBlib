#include <arch/ppc64/regs.h>

.section .isr, "ax"
.globl _vec_start
_vec_start:
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	attn
	.fill 28
	/*
	. = (IV_machine_check * 32) + _vec_start
	attn
	. = (IV_critical_input * 32) + _vec_start
	attn
	. = (IV_debug * 32) + _vec_start
	attn
	. = (IV_data_storage * 32) + _vec_start
	attn
	. = (IV_instruction_storage * 32) + _vec_start
	attn
	. = (IV_external * 32) + _vec_start
	attn
	. = (IV_alignment * 32) + _vec_start
	attn
	. = (IV_program * 32) + _vec_start
	attn
	. = (IV_floating_point_unavailable * 32) + _vec_start
	attn
	. = (IV_system_call * 32) + _vec_start
	attn
	. = (IV_auxiliary_processor_unavailable * 32) + _vec_start
	attn
	. = (IV_decrementer * 32) + _vec_start
	attn
	. = (IV_fixed_interval_timer * 32) + _vec_start
	attn
	. = (IV_watchdog_timer * 32) + _vec_start
	attn
	. = (IV_data_tlb_error * 32) + _vec_start
	attn
	. = (IV_instruction_tlb_error * 32) + _vec_start
	attn
	. = (IV_vector_unavailable * 32) + _vec_start
	attn
	. = (IV_embedded_floating_point_data * 32) + _vec_start
	attn
	. = (IV_embedded_floating_point_round * 32) + _vec_start
	attn
	. = (IV_embedded_performance_monitor * 32) + _vec_start
	attn
	. = (IV_processor_dbell * 32) + _vec_start
	attn
	. = (IV_processor_dbell_critical * 32) + _vec_start
	attn
	. = (IV_guest_processor_dbell * 32) + _vec_start
	attn
	. = (IV_guest_processor_dbell_critical * 32) + _vec_start
	attn
	. = (IV_ehv_system_call * 32) + _vec_start
	attn
	. = (IV_ehv_privilege * 32) + _vec_start
	attn
	. = (IV_lrat_error * 32) + _vec_start
	attn */
	// fill the rest with attn instructions
	.fill ((_vec_start + 0x1000 - .) / 4), 4, 0x200

_vec_end:
