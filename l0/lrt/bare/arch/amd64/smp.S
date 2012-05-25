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

#include <l0/lrt/bare/arch/amd64/smp.h>

.section .init.smp, "ax"
.code16

.globl _smp_start
_smp_start:
        cli

        //set our data segment to the same as code segment
        mov %cs, %ax
        mov %ax, %ds
        //Have to offset because this code + data moves
        lgdt _smp_gdt - _smp_start

        //set PAE and PGE
        mov $0xa0, %eax
        mov %eax, %cr4

        //point cr3 to the pml4
        mov $init_pml4, %eax
        mov %eax, %cr3

        //set LME bit in EFER
        mov $0xc0000080, %ecx
        rdmsr
        or $0x00000100, %eax
        wrmsr

        //Activate longmode, enable paging and protection
        mov %cr0, %eax
        or $0x80000001, %eax
        mov %eax, %cr0

        //Long jump using the long mode code segment
        //This code gets moved so we need to calculate the address
        ljmp $0x8, $_smp_64 - _smp_start + SMP_START_ADDRESS
.code32
.align 16
_smp_gdt:
        .word   2*8 - 1
        .long   init_gdt
        .word   0

.code64
_smp_64:
        mov $_smp_stack + SMP_STACK_SIZE, %rsp
        mov $0x0, %rdi
        mov $lrt_event_init, %rax
        call *%rax
1:      jmp 1b
