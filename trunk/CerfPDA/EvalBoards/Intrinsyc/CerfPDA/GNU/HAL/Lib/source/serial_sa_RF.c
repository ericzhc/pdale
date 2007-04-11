//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2004 University of Sherbrooke All rights reserved.
//
// Module name:
//
//      serial_sa.c
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
//      December 2003
//
////////////////////////////////////////////////////////////////////////////////

#include <serial_RF.h>
#include <timer.h>

#define SERIAL_PORT 0

//UART Control Registers
#define UTCR0(x) (0x80010000 + (x * 0x00020000))
#define UTCR1(x) (0x80010004 + (x * 0x00020000))
#define UTCR2(x) (0x80010008 + (x * 0x00020000))
#define UTCR3(x) (0x8001000C + (x * 0x00020000))

//UART Status Registers
#define UTSR0(x) (0x8001001C + (x * 0x00020000))
#define UTSR1(x) (0x80010020 + (x * 0x00020000))

#define UTDR(x) (0x80010014 + (x * 0x00020000))

#define SERIAL_UTCR0 (*(volatile u32 *)(UTCR0(SERIAL_PORT)))
#define SERIAL_UTCR1 (*(volatile u32 *)(UTCR1(SERIAL_PORT)))
#define SERIAL_UTCR2 (*(volatile u32 *)(UTCR2(SERIAL_PORT)))
#define SERIAL_UTCR3 (*(volatile u32 *)(UTCR3(SERIAL_PORT)))

#define SERIAL_UTSR0 (*(volatile u32 *)(UTSR0(SERIAL_PORT)))
#define SERIAL_UTSR1 (*(volatile u32 *)(UTSR1(SERIAL_PORT)))

#define SERIAL_UTDR (*(volatile u32 *)(UTDR(SERIAL_PORT)))

#define UTSR1_TX_BSY (0x00000001)
#define UTSR1_RX_NOT_EMPTY (0x00000002)
#define UTSR1_TX_NOT_FULL (0x00000004)
#define UTSR1_RX_PARITY_ERROR (0x00000008)
#define UTSR1_RX_FRAMING_ERROR (0x00000010)
#define UTSR1_RX_OVERFLOW (0x00000020)

#define UTCR0_1_SBIT (0x00000000)
#define UTCR0_8_DBIT (0x00000008)

#define UTCR3_RX_ON (0x00000001)
#define UTCR3_TX_ON (0x00000002)

#define GPCLKR0	(*(volatile u8 *)	(0x80020060))
#define	PPAR	(*(volatile u32 *)	(0x90060008))
#define GPLR	(*(volatile u32 *)	(0x90040000))
#define GPDR	(*(volatile u32 *)	(0x90040004))
#define	GPSR	(*(volatile u32 *)	(0x90040008))
#define	GPCR	(*(volatile u32 *)	(0x9004000C))
#define	GAFR	(*(volatile u32 *)	(0x9004001C))

#define GPIO_14	(0x00004000)
#define GPIO_15 (0x00008000)

#define AEROCOMM	0

////////////////////////////////////////////////////////////////////////////////
// init_serial_rf
// PURPOSE: Initializes the RF port.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
init_serial_rf(u32 baud)
{

	while(SERIAL_RF_UTSR1 & UTSR1_TX_BSY);

	                                                            /* Turn everything off                            */
	SERIAL_RF_UTCR3 = 0;
	SERIAL_RF_UTCR0 = 0xFF;

	SERIAL_RF_UTCR0 = (UTCR0_1_SBIT | UTCR0_8_DBIT);

	                                                            /* Set the speed we want                          */
	SERIAL_RF_UTCR1 = 0;
	SERIAL_RF_UTCR2 = baud;

	                                                            /* Turn everything back on (interrupt on)         */
	SERIAL_RF_UTCR3 = (UTCR3_RX_ON | UTCR3_TX_ON | UTCR3_RIE_ON);
}

////////////////////////////////////////////////////////////////////////////////
// set_data_rf
// PURPOSE: Set transceiver in data mode.
// PARAMS:  Nothing.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
set_data_rf(void)
{
#ifdef	AEROCOMM
//	GPSR = GPIO_14;
#else
	GPCR = GPIO_14;
#endif
}
////////////////////////////////////////////////////////////////////////////////
// set_cmd_rf
// PURPOSE: Set transceiver in command mode.
// PARAMS:  Nothing.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
set_cmd_rf(void)
{
#ifdef	AEROCOMM
//	GPCR = GPIO_14;
#else
	GPSR = GPIO_14;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// output_byte_serial_rf
// PURPOSE: Prints a byte to the RF port.
// PARAMS:  (IN) char byte - byte to print.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
output_byte_serial_rf(char byte)
{
   //wait for room in the fifo.
   while((SERIAL_UTSR1 & UTSR1_TX_NOT_FULL) == 0);
   //wait for CTS
   while((GPLR & GPIO_15) != 0);

   SERIAL_UTDR = byte;
}

////////////////////////////////////////////////////////////////////////////////
// input_byte_serial_rf
// PURPOSE: Reads a byte from the RF port.
// PARAMS:  (OUT) char *byte - byte read.
// RETURNS: 1 for success, 0 for failure.
////////////////////////////////////////////////////////////////////////////////
int
input_byte_serial_rf(char *byte)
{
   int error = 0;

   if(SERIAL_UTSR1 & UTSR1_RX_NOT_EMPTY)
   {
      error = (SERIAL_UTSR1 &
               (UTSR1_RX_PARITY_ERROR |
                UTSR1_RX_FRAMING_ERROR |
                UTSR1_RX_OVERFLOW));
      if(error)
      {
         SERIAL_UTCR3 = 0;
         SERIAL_UTCR3 = (UTCR3_RX_ON | UTCR3_TX_ON);
         return 0;
      }

      *byte = (char)SERIAL_UTDR;
      return 1;
   }
   else
   {
      return 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
// output_string_serial_rf
// PURPOSE: Prints a null terminated string to the RF port.
// PARAMS:  (IN) char *string - string to print.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
output_string_serial_rf(char const *string)
{
   while(*string != 0)
   {
      output_byte_serial_rf(*string++);
   }
}

////////////////////////////////////////////////////////////////////////////////
// enable receiver
// PURPOSE: Enable receiver and it's interrupt
// PARAMS:  Nothing.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////

void enable_receiver_rf()
{
	
}

////////////////////////////////////////////////////////////////////////////////
// enable transceiver
// PURPOSE: Enable transceiver and it's interrupt
// PARAMS:  Nothing.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////

void enable_transceiver_rf()
{
	
}

////////////////////////////////////////////////////////////////////////////////
// txFIFOEmpty()
// PURPOSE: Enable transceiver and it's interrupt
// PARAMS:  Nothing.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////

int txFIFOEmpty_rf()
{
	return ((SERIAL_UTSR1 & 0x04) != 0);
}

int rxfifoFull_rf()
{
	return ((SERIAL_UTSR1 & 0x02) == 0);
}

int GetInterruptStatus_rf() // TODO: Fix interrupt masks
{
	if((SERIAL_UTSR0 & 0x01)== 0x01)
		return SA110_RF_INTERRUPT_TRANSMIT;
	else if((SERIAL_UTSR0 & 0x02) == 0x02)
		return SA110_RF_INTERRUPT_RECEIVER;

}

 void setInterruptHandle_rf(void (*handler) (void))
 {
	free_irq(COM_PORT_RF);
	request_irq(COM_PORT_RF, handler);
	 
 }

void  Serial1_irqHandler(void)
{
	
}
