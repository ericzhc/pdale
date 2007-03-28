;********************************************************************************************************
;                                              uC/TCP-IP
;                                      The Embedded TCP/IP Suite
;
;                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
;
;                   All rights reserved.  Protected by international copyright laws.
;                   Knowledge of the source code may not be used to write a similar
;                   product.  This file may only be used in accordance with a license
;                   and should not be redistributed in any way.
;********************************************************************************************************


;********************************************************************************************************
;
;                                       NETWORK UTILITY LIBRARY
;
;                                                 ARM
;                                             IAR Compiler
;
; Filename      : net_util_a.asm
; Version       : V1.85
; Programmer(s) : JDH
;********************************************************************************************************
; Note(s)       : (1) Assumes ARM CPU mode configured for Little Endian.
;********************************************************************************************************


;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

        PUBLIC  NetUtil_16BitSumDataCalcAlign_32


;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

        RSEG CODE:CODE:NOROOT(2)
        CODE32


;********************************************************************************************************
;                                    NetUtil_16BitSumDataCalcAlign_32
;
; Description : Inner loop of checksum calculation.
;
;               (1) Computes the sum of consecutive 16-bit values.
;
;               (2) The computation MUST be done in BIG ENDIAN order. This code are made for LITTLE
;                   ENDIAN CPU, so there IS byte swap in the computation.
;********************************************************************************************************

; CPU_INT32U  NetUtil_16BitSumDataCalcAlign_32 (CPU_INT08U  *pdata_08,  ;          ==> R0
;                                               CPU_INT16U   size)      ;          ==> R1
;                                                                       ; sum      ==> R2

NetUtil_16BitSumDataCalcAlign_32:
        STMFD       SP!, {R2-R12}

        MOV         R2, #0
        B           NetUtil_16BitSumDataCalcAlign_32_0

NetUtil_16BitSumDataCalcAlign_32_1:
        LDMIA       R0!, {R5-R12}                           ; Computation of the sum of 16 16-bit words
                                                            ; (from 8 32-bit CPU registers)
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
        CMP         R1, #(4*8*1)                            ; end of loop
        BCS         NetUtil_16BitSumDataCalcAlign_32_1

        B           NetUtil_16BitSumDataCalcAlign_32_2

NetUtil_16BitSumDataCalcAlign_32_3:
        LDMIA       R0!, {R5}                               ; Computation of the sum of 2 16-bit words
                                                            ; (from 1 32-bit CPU register)

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

        CMP         R1, #(4*1*1)                            ; end of loop
        BCS         NetUtil_16BitSumDataCalcAlign_32_3

        MOV         R0, R2
        LDMFD       SP!, {R2-R12}
        BX          LR                                      ; return


        END


	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
