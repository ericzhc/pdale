@//////////////////////////////////////////////////////////////////////////////////
@// Copyright(c) 2001-2002 Intrinsyc Software Inc. All rights reserved.
@//
@// Module name:
@//
@//      _start_sa.s
@//
@// Description:
@//
@//
@// Author:
@//
@//	Philippe Mabilleau ing.
@//
@// Created:
@//
@//	October-08-2003
@//
@// Modif: 
@//         Eduardo Romero  Août 2006
@//
@////////////////////////////////////////////////////////////////////////////////

#include <start_sa.h>

.text

.globl _start
_start:
    b  _start_loc                   @ Modif ELR 08/2006
    .word   0xAA552559              @ Signature 1
    .word   0xC0000000              @ ram load address
    .word   (_ebss - 0xC0000000)    @ total lenght
    .word   0x454C5220              @ signature 2
    .word   0

_start_loc:
@	clear bss
	mov 	r6, #0			@/* Second arg: fill value.  */
	ldr	r5, .LC1		@/* First arg: start of memory block.  */
	ldr	r7, .LC2
	sub	r7, r7, r5		@/* Third arg: length of block.  */
	subs	r7, r7, #0
	beq	2f
1:	strb	r6, [r5]
	subs	r7, r7, #1
	add	r5, r5, #1
	bne	1b
2:

	stmfd	sp!, {r0-r12,lr}
	ldr	r4, =return_addr
	str	lr, [r4]
	ldr	r4, =stack_ptr
	str	sp, [r4]

@	mov	r4, #0xD2
@	msr	cpsr_c, r4

	ldr	sp, =_stack

	ldr	r4, =main
	mov	pc, r4

.globl set_CP15reg1
set_CP15reg1:
	mcr	p15, 0, r0, c1, c0, 0
	mov	pc, lr

.globl get_CP15reg1
get_CP15reg1:
	mrc	p15, 0, r0, c1, c0, 0
	mov	pc, lr

.globl set_CP15reg2
set_CP15reg2:
	mcr	p15, 0, r0, c2, c0, 0
	mov	pc, lr

.globl get_CP15reg2
get_CP15reg2:
	mrc	p15, 0, r0, c2, c0, 0
	mov	pc, lr

.globl set_CP15reg3
set_CP15reg3:
	mcr	p15, 0, r0, c3, c0, 0
	mov	pc, lr

.globl get_CP15reg3
get_CP15reg3:
	mrc	p15, 0, r0, c3, c0, 0
	mov	pc, lr

@////////////////////////////////////////////////////////////////////////////////
@// mmu_setup:
@// PURPOSE:   Setup the translation table base, domain access control
@// and enable the MMU with virtual vector interrupt adjust
@// PARAMS:    r0 - Translation table base address
@//            r1 - Domain access control value
@// REGISTERS: r0, r1
@// RETURNS:   Nothing.
@// NOTES:
@////////////////////////////////////////////////////////////////////////////////
.globl mmu_setup
mmu_setup:
	mcr	p15, 0, r0, c2, c0, 0
	mcr	p15, 0, r1, c3, c0, 0
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #0x01
	orr	r0, r0, #0x2000
	mcr	p15, 0, r0, c1, c0, 0
	mov	pc, lr

@////////////////////////////////////////////////////////////////////////////////
@// freq_change:
@// PURPOSE:   Change the CPU frequency and enable clock switching
@// PARAMS:    r0 - CCF value
@// REGISTERS: r0, r1
@// RETURNS:   Nothing.
@// NOTES:     This function conforms to the APCS and is callable from C as:
@//            void freq_change(u32 ccf);
@////////////////////////////////////////////////////////////////////////////////
.globl freq_change
freq_change:
        ldr     r1, =0x90020014
	str     r0, [r1]

        @// Enable clock switching
        ldr     r1, =0x00000000
	mcr     p15, 0, r1, c15, c1, 2

        @// return
	mov     pc, lr


@////////////////////////////////////////////////////////////////////////////////
@// exit2iboot:
@// PURPOSE:   Return to i-boot monitor
@// PARAMS:    r0 - return value
@// REGISTERS: r0, r1
@// RETURNS:   Nothing.
@////////////////////////////////////////////////////////////////////////////////
.globl exit2iboot
exit2iboot:
	mov	r1, #0xD3
	msr	cpsr_c, r1

	ldr	r1, =stack_ptr
	ldr	sp, [r1]
	ldmfd	sp!, {r0-r12,lr}
	ldr	r1, =return_addr
	ldr	pc, [r1]

	.align	2
.LC1:
	.word	_bss
.LC2:
	.word	_ebss

	.lcomm	return_addr, 4

	.lcomm	stack_ptr, 4

	.ltorg
