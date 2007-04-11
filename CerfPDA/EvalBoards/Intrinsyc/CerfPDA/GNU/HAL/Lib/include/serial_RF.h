//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2004 University of Sherbrooke All rights reserved.
//
// Module name:
//
//      serial_RF.h
//
// Description:
//
//      Interfaces with the StrongArm SA1110 serial port 1 hardware.
//
// Author:
//
//      Mike Kirkland
//	Philippe Mabilleau ing.
//
// Created:
//
//      DEcember 2003
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SERIAL_RF_H
#define SERIAL_RF_H

#include <types.h>

#ifndef SERIAL_H
#define SERIAL_BAUD_9600   (0x00000017)
#define SERIAL_BAUD_19200  (0x0000000B)
#define SERIAL_BAUD_38400  (0x00000005)
#define SERIAL_BAUD_57600  (0x00000003)
#define SERIAL_BAUD_115200 (0x00000001)
#endif

#define SA110_RF_INTERRUPT_TRANSMIT 0
#define SA110_RF_INTERRUPT_RECEIVER 1


void init_serial_rf(u32 baud);
void output_byte_serial_rf(char byte);
int input_byte_serial_rf(char *byte);
void set_data_rf(void);
void set_cmd_rf(void);
void output_string_serial_rf(char const *string);

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/
#define RF_PORT 0                                                      // modif ELR
//#define SERIAL_RF_UTCR3 (*(volatile u32 *) UTCR3(RF_PORT))
//#define SERIAL_RF_UTSR0 (*(volatile u32 *) UTSR0(RF_PORT))


#define SERIAL_RF_UTCR0     (*(volatile u32 *)  0x80010000)     
#define SERIAL_RF_UTCR1     (*(volatile u32 *)  0x80010004)     
#define SERIAL_RF_UTCR2     (*(volatile u32 *)  0x80010008)     
#define SERIAL_RF_UTCR3     (*(volatile u32 *)  0x8001000C)     
#define SERIAL_RF_UTDR      (*(volatile u32 *)  0x80010014)
#define SERIAL_RF_UTSR0     (*(volatile u32 *)  0x8001001C)
#define SERIAL_RF_UTSR1     (*(volatile u32 *)  0x80010020)
     

#define UTCR3_BREAK_ON (0x00000004)
#define UTCR3_RIE_ON   (0x00000008)
//#define UTCR3_TIE_ON (0x000000016)
#define UTCR3_TIE_ON (0x000000010)                                      // modif ELR

#define UTSR1_TX_BSY           (0x00000001)
#define UTSR1_RX_NOT_EMPTY     (0x00000002)
#define UTSR1_TX_NOT_FULL      (0x00000004)
#define UTSR1_RX_PARITY_ERROR  (0x00000008)
#define UTSR1_RX_FRAMING_ERROR (0x00000010)
#define UTSR1_RX_OVERFLOW      (0x00000020)


#define UTSR0_TFS (0x01)
#define UTSR0_RFS (0x02)
#define UTSR0_RID (0x04)
#define UTSR0_RBB (0x08)
#define UTSR0_REB (0x10)
#define UTSR0_EIF (0x20)


#define R_GPCLKR0   (*(volatile u8 *)	(0x80020060))
#define	R_PPAR	   (*(volatile u32 *)	(0x90060008))
#define R_GPLR	   (*(volatile u32 *)	(0x90040000))
#define R_GPDR	   (*(volatile u32 *)	(0x90040004))
#define	R_GPSR	   (*(volatile u32 *)	(0x90040008))
#define	R_GPCR	   (*(volatile u32 *)	(0x9004000C))

#define B_GPIO_14	(0x00004000)
#define B_GPIO_15    (0x00008000)

#define COM_PORT_RF 15
#endif //SERIAL_RF_H
