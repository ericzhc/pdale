@********************************************************************************************************
@                                               uC/CPU
@                                    CPU CONFIGURATION & PORT LAYER
@
@                          (c) Copyright 1999-2004; Micrium, Inc.; Weston, FL
@
@                   All rights reserved.  Protected by international copyright laws.
@                   Knowledge of the source code may not be used to write a similar
@                   product.  This file may only be used in accordance with a license
@                   and should not be redistributed in any way.
@********************************************************************************************************


@********************************************************************************************************
@
@                                           CPU PORT FILE
@
@                                              ARM7TDMI
@                                           GNU C Compiler
@
@ Filename      : cpu_a.s
@ Version       : V1.10
@ Programmer(s) : JJL
@                 ITJ
@ GNU Port      : Jean-Denis Hatier
@********************************************************************************************************


@********************************************************************************************************
@                                           PUBLIC FUNCTIONS
@********************************************************************************************************

        .global  CPU_SR_Save
        .global  CPU_SR_Restore


@********************************************************************************************************
@                                               EQUATES
@********************************************************************************************************

        .equ CPU_NO_INT, 0xC0                   @ Mask used to disable interrupts (Both FIR and IRQ)


@********************************************************************************************************
@                                      CODE GENERATION DIRECTIVES
@********************************************************************************************************

        .code 32

@$PAGE
@*********************************************************************************************************
@                                      CRITICAL SECTION FUNCTIONS
@
@ Description : Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking, the
@               state of the interrupt disable flag is stored in the local variable 'cpu_sr' & interrupts
@               are then disabled ('cpu_sr' is allocated in all functions that need to disable interrupts).
@               The previous interrupt state is restored by copying 'cpu_sr' into the CPU's status register.
@
@ Prototypes  : CPU_SR  CPU_SR_Save(void);
@               void    CPU_SR_Restore(CPU_SR cpu_sr);
@
@
@ Note(s)     : (1) These functions are used in general like this:
@
@                   void  Task (void *p_arg)
@                   {
@                                                               /* Allocate storage for CPU status register */
@                   #if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
@                       CPU_SR  cpu_sr;
@                   #endif
@
@                            :
@                            :
@                       CPU_CRITICAL_ENTER();                   /* cpu_sr = CPU_SR_Save();                  */
@                            :
@                            :
@                       CPU_CRITICAL_EXIT();                    /* CPU_SR_Restore(cpu_sr);                  */
@                            :
@                            :
@                   }
@
@               (2) CPU_SR_Restore() is implemented as recommended by Atmel's application note :
@
@                      "Disabling Interrupts at Processor Level"
@*********************************************************************************************************

CPU_SR_Save:
        MRS     R0,CPSR                           @ Set IRQ & FIQ bits in CPSR to dis all ints.
        ORR     R1,R0,#CPU_NO_INT
        MSR     CPSR_c,R1
        MRS     R1,CPSR                           @ Confirm that CPSR contains proper int dis flags.
        AND     R1,R1,#CPU_NO_INT
        CMP     R1,#CPU_NO_INT
        BNE     CPU_SR_Save                       @ Not properly DISABLED; try again.
        MOV     PC,LR                             @ DISABLED, rtn original CPSR contents in R0.


CPU_SR_Restore:                                   @ See Note #2.
        MSR     CPSR_c,R0
        MOV     PC,LR


        .ltorg
