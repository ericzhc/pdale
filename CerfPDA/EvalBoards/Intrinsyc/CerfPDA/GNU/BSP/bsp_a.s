
@********************************************************************************************************
@                                                uC/OS-II
@                                          The Real-Time Kernel
@
@                               (c) Copyright 2004, Micrium, Inc., Weston, FL
@                                           All Rights Reserved
@
@                                                   ARM
@                                          Board Support Package
@
@ File    : bsp_a.s
@ By      : Jean-Denis Hatier
@ Modif by: Eduardo Luis Romero, Avril 2006 (IRQ handling modifications)
@
@********************************************************************************************************

    .global ISR_Reset
    .global ISR_Undefined
    .global ISR_SWI
    .global ISR_CodeAbort
    .global ISR_DataAbort
    .global ISR_IRQ
    .global ISR_FIQ
    .global OS_CPU_ISR

    .extern ISR_Reset_C
    .extern ISR_Undefined_C
    .extern ISR_SWI_C
    .extern ISR_CodeAbort_C
    .extern ISR_DataAbort_C
    .extern ISR_IRQ_C
    .extern ISR_FIQ_C
    .extern Int_Status_C

    .extern pStackTop_User
    .extern pStackTop_FIQ
    .extern pStackTop_IRQ
    .extern pStackTop_Supervisor
    .extern pStackTop_Abort
    .extern pStackTop_Undefined
    .extern pStackTop_System

    .global BSP_SetStackPointers
    .extern OSIntEnter
    .extern OSIntExit
    .extern pExceptAddr
    
    .extern     OSIntCtxSwFlag
    .extern     OS_IntCtxSw
    .extern     irq_dev_handler_Tab
    .extern     IRQdefaultHandler

    .equ        ICIPadd, 0x90050000             @  Interrupt controller IRQ pending register    

    .code 32

BSP_SetStackPointers:
    MRS R0, CPSR                                @ Save CPSR

    LDR R1, =0x11                               @ Set FIQ mode stack
    MSR CPSR, R1
    LDR R2, =pStackTop_FIQ
    LDR SP, [R2]

    LDR R1, =0x12                               @ Set IRQ mode stack
    MSR CPSR, R1
    LDR R2, =pStackTop_IRQ
    LDR SP, [R2]

    LDR R1, =0x13                               @ Set Supervisor mode stack
    MSR CPSR, R1
    LDR R2, =pStackTop_Supervisor
    LDR SP, [R2]

    LDR R1, =0x17                               @ Set Abort mode stack
    MSR CPSR, R1
    LDR R2, =pStackTop_Abort
    LDR SP, [R2]

    LDR R1, =0x1b                               @ Set Undefined mode stack
    MSR CPSR, R1
    LDR R2, =pStackTop_Undefined
    LDR SP, [R2]

    LDR R1, =0x1f                               @ Set System mode stack
    MSR CPSR, R1
    LDR R2, =pStackTop_System
    LDR SP, [R2]

    MSR CPSR, R0                                @ Set CPSR back to original value
    MOV PC,LR                                   @ Return

ISR_Reset:
    STMFD   SP!,{R0-R3,R12,LR}

    LDR     R0,_pExceptAddr
    STR     LR,[R0]                             @ Store exception adress

    BL      OSIntEnter                          @ Indicate beginning of ISR
    BL      ISR_Reset_C                         @ Handle interrupt (see BSP.C)
    BL      OSIntExit                           @ Indicate end of ISR

    LDMFD   SP!,{R0-R3,R12,LR}                  @ Restore registers of interrupted task's stack
    SUBS    PC,LR,#0                            @ Return from exception

ISR_Undefined:
    STMFD   SP!,{R0-R3,R12,LR}

    LDR     R0,_pExceptAddr
    STR     LR,[R0]                             @ Store exception adress

    BL      OSIntEnter                          @ Indicate beginning of ISR
    BL      ISR_Undefined_C                     @ Handle interrupt (see BSP.C)
    BL      OSIntExit                           @ Indicate end of ISR

    LDMFD   SP!,{R0-R3,R12,LR}                  @ Restore registers of interrupted task's stack
    SUBS    PC,LR,#0                            @ Return from exception

ISR_SWI:
    STMFD   SP!,{R0-R3,R12,LR}

    LDR     R0,_pExceptAddr
    STR     LR,[R0]                             @ Store exception adress

    BL      OSIntEnter                          @ Indicate beginning of ISR
    BL      ISR_SWI_C                           @ Handle interrupt (see BSP.C)
    BL      OSIntExit                           @ Indicate end of ISR

    LDMFD   SP!,{R0-R3,R12,LR}                  @ Restore registers of interrupted task's stack
    SUBS    PC,LR,#0                            @ Return from exception

ISR_CodeAbort:
    STMFD   SP!,{R0-R3,R12,LR}

    LDR     R0,_pExceptAddr
    STR     LR,[R0]                             @ Store exception adress

    BL      OSIntEnter                          @ Indicate beginning of ISR
    BL      ISR_CodeAbort_C                     @ Handle interrupt (see BSP.C)
    BL      OSIntExit                           @ Indicate end of ISR

    LDMFD   SP!,{R0-R3,R12,LR}                  @ Restore registers of interrupted task's stack
    SUBS    PC,LR,#0                            @ Return from exception

ISR_DataAbort:
    STMFD   SP!,{R0-R3,R12,LR}

    LDR     R0,_pExceptAddr
    STR     LR,[R0]                             @ Store exception adress

    BL      OSIntEnter                          @ Indicate beginning of ISR
    BL      ISR_DataAbort_C                     @ Handle interrupt (see BSP.C)
    BL      OSIntExit                           @ Indicate end of ISR

    LDMFD   SP!,{R0-R3,R12,LR}                  @ Restore registers of interrupted task's stack
    SUBS    PC,LR,#4                            @ Return from exception

@ Modif ELR Avr 2006
@
@ISR_IRQ:
@    STMFD   SP!,{R0-R3,R12,LR}
@
@    LDR     R0,_pExceptAddr
@    STR     LR,[R0]                             @ Store exception adress
@
@   BL      OSIntEnter                          @ Indicate beginning of ISR
@    BL      ISR_IRQ_C                           @ Handle interrupt (see BSP.C)
@    BL      OSIntExit                           @ Indicate end of ISR
@
@    LDMFD   SP!,{R0-R3,R12,LR}                  @ Restore registers of interrupted task's stack
@    SUBS    PC,LR,#4                            @ Return from ISR
@

ISR_IRQ:
    stmfd   sp!,{r0-r3,r12,lr}                      @ save registers
    bl      OSIntEnter                              @ Indicate beginning of ISR
  	mov	    r1, #0                                  @ uses r1 as counter [0..31]
  	sub     r1,r1,#1                                @ sub 1 to compensate the first loop inc
irq_loop_i:
    add     r1,r1,#1                                @ inc r1
    and     r1,r1,#0x1F                             @ to keep r1 in the range [0..31]
    ldr     r3,=ICIPadd
	ldr	    r2, [r3, #0]                            @ r2 = ICIP
	cmp     r2, #0
	beq     irq_quit                                @ quit if no more pending interrupts
	mov     r3,#1
	tst     r2,r3,lsl r1                            @ test the pending int flags
	beq     irq_loop_i                              @ continue if the tested flag is zero
	stmfd   sp!,{r0-r1}                             @ found int pend: save r0..r1
	ldr     r3,=irq_dev_handler_Tab                 @ load the irq_handler add from the tab
	ldr     r3,[r3,r1,asl #2]                       @  (r1 is the index)
	cmp     r3,#0                                   @ the pointer is NULL ??
	beq     irq_default_handler
	mov     lr,pc                                   @   no, call the handler
	mov     pc,r3
	b       irq_restore_regs
irq_default_handler:
    mov     r0,r1                                   @     uses the counter (r1) as parameter
	bl      IRQdefaultHandler                       @   yes, call the default irq handler
irq_restore_regs:
	ldmfd   sp!,{r0-r1}                             @ restore registers
	b       irq_loop_i
irq_quit:
    bl      OSIntExit                               @ Indicate end of ISR
@ uCOS-II: context switch    
    ldr     r0,=OSIntCtxSwFlag                      @ See if we need to do a context switch
    ldr     R1,[R0]
    cmp     R1,#1
    beq     OS_IntCtxSw                             @ Yes, Switch to Higher Priority Task
                                                    @ (OS_IntCtxSw will not return to here!)
@ end of interrupt
    ldmfd   sp!,{r0-r3,r12,lr}                      @ Non, restore registers of interrupted task's stack
    subs    PC,LR,#4                                @ Return from ISR


ISR_FIQ:
    STMFD   SP!,{R0-R3,R12,LR}

    LDR     R0,_pExceptAddr
    STR     LR,[R0]                             @ Store exception adress

    BL      OSIntEnter                          @ Indicate beginning of ISR
    BL      ISR_FIQ_C                           @ Handle interrupt (see BSP.C)
    BL      OSIntExit                           @ Indicate end of ISR

    LDMFD   SP!,{R0-R3,R12,LR}                  @ Restore registers of interrupted task's stack
    SUBS    PC,LR,#4                            @ Return from ISR

_pExceptAddr:
   .int pExceptAddr

    .ltorg
