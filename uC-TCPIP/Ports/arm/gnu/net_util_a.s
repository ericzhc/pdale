@********************************************************************************************************
@                                              uC/TCP-IP
@                                      The Embedded TCP/IP Suite
@
@                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
@
@               All rights reserved.  Protected by international copyright laws.
@
@               uC/TCP-IP is provided in source form for FREE evaluation, for educational
@               use or peaceful research.  If you plan on using uC/TCP-IP in a commercial
@               product you need to contact Micrium to properly license its use in your
@               product.  We provide ALL the source code for your convenience and to help
@               you experience uC/TCP-IP.  The fact that the source code is provided does
@               NOT mean that you can use it without paying a licensing fee.
@
@               Knowledge of the source code may NOT be used to develop a similar product.
@
@               Please help us continue to provide the Embedded community with the finest
@               software available.  Your honesty is greatly appreciated.
@********************************************************************************************************


@********************************************************************************************************
@
@                                       NETWORK UTILITY LIBRARY
@
@                                                ARM
@                                            GNU Compiler
@
@ Filename      : net_util_a.s
@ Version       : V1.89
@ Programmer(s) : JDH
@********************************************************************************************************
@ Note(s)       : (1) Assumes ARM CPU mode configured for Little Endian.
@********************************************************************************************************


@********************************************************************************************************
@                                          PUBLIC FUNCTIONS
@********************************************************************************************************

        .global  NetUtil_16BitSumDataCalcAlign_32


@********************************************************************************************************
@                                     CODE GENERATION DIRECTIVES
@********************************************************************************************************

        .code 32


@*$PAGE*
@********************************************************************************************************
@                                 NetUtil_16BitSumDataCalcAlign_32()
@
@ Description : Calculate 16-bit sum on 32-bit word-aligned data.
@
@ Argument(s) : pdata_32    Pointer  to 32-bit word-aligned data (see Note #2).
@
@               size        Size of data.
@
@ Return(s)   : 16-bit sum (see Notes #1 & #3).
@
@ Caller(s)   : NetUtil_16BitSumDataCalc().
@
@               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
@               application function(s).
@
@ Note(s)     : (1) Computes the sum of consecutive 16-bit values.
@
@               (2) Since many word-aligned processors REQUIRE  that multi-octet words be located on word-
@                   aligned addresses, sum calculation REQUIREs that 32-bit words are accessed on addresses
@                   that are multiples of 4 octets.
@
@               (3) The 16-bit sum MUST be returned in Big Endian/Network order.
@
@                   See 'net_util.c  NetUtil_16BitSumDataCalc()  Note #5b'.
@
@                   (a) Assumes Little Endian CPU Mode (see 'net_util_a.s  Note #1') thus requiring the 
@                       16-bit octets of the 32-bit data to be swapped.
@
@                       #### However, the 16-bit octets COULD be swapped after the 16-bit sum is fully 
@                       calculated.
@********************************************************************************************************

@ CPU_INT32U  NetUtil_16BitSumDataCalcAlign_32 (void        *pdata_32,   @       ==>  R0
@                                               CPU_INT32U   size)       @       ==>  R1
@                                                                        @  sum  ==>  R2

NetUtil_16BitSumDataCalcAlign_32:
        STMFD       SP!, {R2-R12}

        MOV         R2, #0
        B           NetUtil_16BitSumDataCalcAlign_32_0

NetUtil_16BitSumDataCalcAlign_32_1:
        LDMIA       R0!, {R5-R12}                           @ Calc sum of sixteen 16-bit words ...
                                                            @ ... using eight 32-bit CPU regs.

        MOV         R3, R5, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        MOV         R3, R5, LSL #16
        MOV         R3, R3, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2


        MOV         R3, R6, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        MOV         R3, R6, LSL #16
        MOV         R3, R3, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2


        MOV         R3, R7, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        MOV         R3, R7, LSL #16
        MOV         R3, R3, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

@*$PAGE*
        MOV         R3, R8, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        MOV         R3, R8, LSL #16
        MOV         R3, R3, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2


        MOV         R3, R9, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        MOV         R3, R9, LSL #16
        MOV         R3, R3, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2


        MOV         R3, R10, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        MOV         R3, R10, LSL #16
        MOV         R3, R3, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2


        MOV         R3, R11, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        MOV         R3, R11, LSL #16
        MOV         R3, R3, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2


        MOV         R3, R12, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        MOV         R3, R12, LSL #16
        MOV         R3, R3, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        SUB         R1, R1, #(4*8*1)

NetUtil_16BitSumDataCalcAlign_32_0:
        CMP         R1, #(4*8*1)                            @ end of loop
        BCS         NetUtil_16BitSumDataCalcAlign_32_1

        B           NetUtil_16BitSumDataCalcAlign_32_2


@*$PAGE*
NetUtil_16BitSumDataCalcAlign_32_3:
        LDMIA       R0!, {R5}                               @ Calc sum of two 16-bit words ...
                                                            @ ... using   one 32-bit CPU reg.

        MOV         R3, R5, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        MOV         R3, R5, LSL #16
        MOV         R3, R3, LSR #16
        MOV         R4, R3, LSR #8
        AND         R3, R3, #0xFF
        ORR         R4, R4, R3, LSL #8
        ADD         R2, R4, R2

        SUB         R1, R1, #(4*1*1)

NetUtil_16BitSumDataCalcAlign_32_2:

        CMP         R1, #(4*1*1)                            @ end of loop
        BCS         NetUtil_16BitSumDataCalcAlign_32_3

        MOV         R0, R2
        LDMFD       SP!, {R2-R12}
        BX          LR                                      @ return


       .ltorg

