//This is the bit of asm that we hit on the initial IPI
// it just calls lrt_start

	.globl lrt_start_isr
lrt_start_isr:
	pushq	%rsp //store original rsp
	pushq	(%rsp) //store it again right afterwards
	andq	$-0x10, %rsp //align to 16 bytes
	call 	lrt_start
	//whether we were aligned or not, the original rsp is
	// 8 bytes up from our current stack point (because we pushed it
	// twice)
	movq	8(%rsp), %rsp
	iretq
