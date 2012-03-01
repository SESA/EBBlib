#ifndef ARCH_PPC64_REGS_H
#define ARCH_PPC64_REGS_H

/*
 * SPRs
 */
#define SPRN_DAR	0x013
#define SPRN_DEC	0x016
#define SPRN_SRR0	0x01a
#define SPRN_SRR1	0x01b
#define SPRN_PID	0x030
#define SPRN_DECAR	0x036
#define SPRN_CSRR0	0x03a
#define SPRN_CSRR1	0x03b
#define SPRN_DEAR	0x03d
#define SPRN_ESR	0x03e
#define SPRN_IVPR	0x03f
#define SPRN_CTRLF      0x088
#define SPRN_CTRLT      0x098
#define SPRN_TB		0x10c
#define SPRN_TBU	0x10d
#define SPRN_SPRG0	0x110
#define SPRN_SPRG1	0x111
#define SPRN_SPRG2	0x112
#define SPRN_SPRG3	0x113
#define SPRN_SPRG4	0x114
#define SPRN_TBL	0x11c
#define SPRN_TBU_MT	0x11d
#define SPRN_PIR	0x11e
#define SPRN_PVR	0x11f
#define SPRN_EPCR	0x133
#define SPRN_IAC1	0x138
#define SPRN_IAC2	0x139
#define SPRN_IAC3	0x13a
#define SPRN_IAC4	0x13b
#define SPRN_DAC1	0x13c
#define SPRN_DAC2	0x13d
#define SPRN_DVC1	0x13e
#define SPRN_DVC2	0x13f
#define SPRN_TSR	0x150
#define SPRN_TCR	0x154
#define SPRN_IVOR0	0x190
#define SPRN_IVOR1	0x191
#define SPRN_IVOR2	0x192
#define SPRN_IVOR3	0x193
#define SPRN_IVOR4	0x194
#define SPRN_IVOR5	0x195
#define SPRN_IVOR6	0x196
#define SPRN_IVOR7	0x197
#define SPRN_IVOR8	0x198
#define SPRN_IVOR9	0x199
#define SPRN_IVOR10	0x19a
#define SPRN_IVOR11	0x19b
#define SPRN_IVOR12	0x19c
#define SPRN_IVOR13	0x19d
#define SPRN_IVOR14	0x19e
#define SPRN_IVOR15	0x19f
#define SPRN_IVOR38	0x1b0
#define SPRN_IVOR39	0x1b1
#define SPRN_IVOR40	0x1b2
#define SPRN_IVOR41	0x1b3
#define SPRN_IVOR42	0x1b4
#define SPRN_TENSR	0x1b5
#define SPRN_TENS	0x1b6
#define SPRN_TENC	0x1b7
#define SPRN_TIR	0x1be
#define SPRN_LPIDR	0x152
#define SPRN_IVOR32	0x210
#define SPRN_IVOR33	0x211
#define SPRN_IVOR34	0x212
#define SPRN_IVOR35	0x213
#define SPRN_IVOR36	0x214
#define SPRN_IVOR37	0x215
#define SPRN_MCSRR0	0x23a
#define SPRN_MCSRR1	0x23b
#define SPRN_MCSR	0x23c
#define SPRN_DAC3	0x351
#define SPRN_DAC4	0x350
#define SPRN_IMR	0x370
#define SPRN_IMMR	0x371
#define SPRN_IAR	0x372
#define SPRN_PPR32	0x382
#define SPRN_CELL_TSC	0x399
#define SPRN_EPLC	0x3b3
#define SPRN_EPSC	0x3b4

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
