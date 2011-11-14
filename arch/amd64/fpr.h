/******************************************************************************
 * K42: (C) Copyright IBM Corp. 2000.
 * All Rights Reserved
 *
 * This file is distributed under the GNU LGPL. You should have
 * received a copy of the license along with K42; see the file LICENSE.html
 * in the top-level directory for more details.
 *
 * $Id: fpr.h,v 1.4 2002/01/12 04:56:33 peterson Exp $
 *****************************************************************************/
/*****************************************************************************
 * Module Description: support routines for floating point registers
 * **************************************************************************/

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* DS: I find this to be wrong, x86-64 stack is 16 byte aligned at */
/* fuction entry so adding a spare 8 bytes causes an alignment */
/* problem */

/* On the X86-64, the stack is 8-byte aligned, and kept that way
 * by all stack operations.  The fxsave/fxrstor instructions saveg
 * all the floating point registers at one time (512 bytes
 * worth).  It would seem useful to save all the floating pointer
 * registers on the stack at times.  However, the fxsave/fxrstor
 * instructions require their address to be 16-byte aligned.  To
 * solve this, we always save/restore the floating point
 * registers on the stack by the
 * SAVE_FLOAT_ON_STACK/RESTORE_FLOAT_FROM_STACK macros.  These
 * macros are responsible for whatever is necessary to keep
 * things working.

 * When we push the registers on the stack, we may (or may not)
 * need to push an extra 8-bytes to get things aligned before we
 * can use the fxsave instruction.  The problem comes when we
 * restore.  After the fxrstor instruction, we may (or may not)
 * need to take the extra 8-byte off the stack.

 * If we sometimes need an extra 8-bytes, and sometimes don't,
 * the size of the floating point register save area becomes
 * variable sized.  This causes all sorts of problems.  So we
 * want the save area to be of fixed size.  To do that, we always
 * make the floating point register save area 8 bytes too big.
 * If the save area is 16-byte aligned, we save the FPR at the
 * beginning of the save area.  If the save area is off by
 * 8-bytes, then we save the FPR offset by 8-bytes in the save
 * area, so that they are 16-byte aligned.
 *
 * So the floating point register save area is always a fixed
 * size (and aligned on an 8-byte boundary), but the location of
 * the actual floating point registers in the save area varies.
 * As a result, you can never access them directly, but only thru
 * the following macros to save and restore them.
 */

#define FPR_SAVE_AREA_SIZE   (64*8)

/* save floating point registers in area pointed to by ptr.
   reg is a spare register for computation */

#define SAVE_FLOAT_(ptr,reg)								\
	mov	ptr,reg;			/* copy of pointer to compute on */ 	\
	fwait;										\
	fxsave	(reg);				/* save floating point registers */

#define RESTORE_FLOAT_(ptr,reg)			  					    \
	mov	ptr,reg;			/* copy of pointer to compute on */	    \
	fxrstor	(reg); 	 			/* restore fp registers */

//DS: Haven't touched this but likely buggy
#define COPY_FLOAT_SAVES_TO_STACK(psReg)					\
	leaq    -FPR_SAVE_AREA_SIZE(%rsp),%rsp; /* reserve space on stack for fpr */	 \
	pushq	%rdi;  				/* save registers we use for move */	 \
	pushq	%rsi;	 \
	pushq	%rcx;	 \
	movq	psReg, %rsi;			/* get address to move from */	 \
	add	$8,%rsi;			/* if 8, move to 16; if 16, move to 8 */	 \
	andq	$0xfffffffffffffff0,%rsi;	/* take back to 16-byte alignment */		 \
	leaq	3*8(%rsp), %rdi;		/* get address to move to (on stack) */	 \
	add	$8,%rdi;			/* if 8, move to 16; if 16, move to 8 */	 \
	andq	$0xfffffffffffffff0,%rdi;	/* take back to 16-byte alignment */		 \
	mov	$64,%rcx;			/* count of 64 floating point registers */	 \
	rep;	 										\
	movsq;					/* do the move -- 64 times from %rsi to %rdi */	 \
	popq	%rcx;				/* restore registers used for move */	 \
	popq	%rsi;									 \
	popq	%rdi
