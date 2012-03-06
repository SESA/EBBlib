//This is the bit of asm that we hit on the initial IPI
// it just calls lrt_start

	.globl lrt_start_isr
lrt_start_isr:
	movq 	%rbp, -24(%rsp) //store original rbp 3 words beyond the stack
	movq 	%rsp, %rbp //store original rsp
	subq 	$8, %rsp //buy a word on the stack
	andq 	$-0x10, %rsp //16 byte align the stack (growing downwards)
	//We now have 1 or 2 words free on the stack (depending on the original
	// alignment). This is why we put rbp 3 words beyond the stack
	movq 	%rbp, (%rsp) //store the original rsp right here
	movq 	-24(%rbp), %rbp //restore original rbp
	call 	lrt_start
	movq 	(%rsp), %rsp //restore original rsp
	iretq