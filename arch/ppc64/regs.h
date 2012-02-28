#ifndef ARCH_PPC64_REGS_H
#define ARCH_PPC64_REGS_H

#define MIN_FRAME_SZ	112
#define STACK_SZ	(8<<10)

/*
 * Interrupt Vector IDs
 */
#define IV_machine_check			0
#define IV_critical_input			1
#define IV_debug				2
#define IV_data_storage				3
#define IV_instruction_storage			4
#define IV_external				5
#define IV_alignment				6
#define IV_program				7
#define IV_floating_point_unavailable		8
#define IV_system_call				9
#define IV_auxiliary_processor_unavailable	10
#define IV_decrementer				11
#define IV_fixed_interval_timer			12
#define IV_watchdog_timer			13
#define IV_data_tlb_error			14
#define IV_instruction_tlb_error		15
#define IV_vector_unavailable			16
#define IV_embedded_floating_point_data		17
#define IV_embedded_floating_point_round	18
#define IV_embedded_performance_monitor		19
#define IV_processor_dbell			20
#define IV_processor_dbell_critical		21
#define IV_guest_processor_dbell		22
#define IV_guest_processor_dbell_critical	23
#define IV_ehv_system_call			24
#define IV_ehv_privilege			25
#define IV_lrat_error				26

#endif
