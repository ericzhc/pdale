
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
@         : Philippe Mabilleau ing. 2004-11-02
@
@********************************************************************************************************

    .extern pStackTop_User
    .extern pStackTop_FIQ
    .extern pStackTop_IRQ
    .extern pStackTop_Supervisor
    .extern pStackTop_Abort
    .extern pStackTop_Undefined
    .extern pStackTop_System

    .global BSP_SetStackPointers

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


    .ltorg
