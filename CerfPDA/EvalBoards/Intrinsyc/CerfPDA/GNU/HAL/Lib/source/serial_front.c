//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2003 University of Sherbrooke All rights reserved.
//
// Module name:
//
//      serial_front.c
//
// Description:
//
//      Interfaces with the TL16C752B serial hardware.
//      for the front serial port of the CerfPDA
//
// Author:
//
//      Philippe Mabilleau ing.
//
// Created:
//
//      November 2003
//
////////////////////////////////////////////////////////////////////////////////

#include <serial_front.h>

////////////////////////////////////////////////////////////////////////////////
// init_serial
// PURPOSE: Initializes the front serial port.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void init_serial_front(u32 baud)
{
	printf("Initializing serial front\n\r");
	
	setBaudRate(baud);
	LCR = 0;
	LCR |= 0x03;	// 8 bits configuration
	LCR &= 0xFB;	// 1 stop bit
	setParity(PARITY_NONE);

	MCR |= 0x08;		// OP output to low to activate the RS-232 buffer

	initializeFIFO();
	printf("Init done...serial front\n\r");
}

/*
	Divisor value on 16 bits: DLH (MSB) DLL (LSB)
	Depends on the value of the quartz: 22.1184 MHz from CerfPDA specs.
	Setting value is baudrate = quartz / 16 / x 
		where x is the value to set for DLL-DHL
	Ex: For 9600 baudrate
	(22.1184 = 0x90) (3.5795 = 0x17) (3.6864 = 0x18);	
	9600 bauds with a 3.6864 MHz needs a 24 factor divisor: CLOCK / (16*BaudRate) = value;
*/
void setBaudRate(u32 baud)
{
	LCR |= 0x80;	// Select DLL-DLH

	switch(baud) {
		default:
			DLL = 0x90;
			DLH = 0x00;
			break;
	}

	LCR &= 0x7f;	// Select DLL-DLH
}

void setParity(int parity)
{
	switch (parity) {
		case PARITY_NONE:	
			LCR &= 0xF7; // Force LCR bit 3 to zero
			break;
		case PARITY_ODD:
			LCR |= 0x8;	 // Activate LCR bit 3
			LCR &= 0xEF; // Force LCR bit 4 to zero
			break;
		case PARITY_EVEN:
			LCR |= 0x18; // Activate bits 3 and 4
			break;
		default: // no parity
			LCR &= 0xF7; // Force LCR bit 3 to zero
			break;
	}
}

void initializeFIFO()
{
	// (page 25)
	FCR = 0x0;		// desactivate FIFO before configuration: bit 0
	FCR |= 0x06;	// clear Rx and Tx FIFOs
	FCR &= 0xF7;	// DMA mode 0, bit 3
	FCR &= 0x0F;	// set trigger levels to 8 spaces/characters
	FCR |= 0x01;	// activate FIFO
}

void SetRTS()
{
	LCR = 0xbf; 
	EFR |= 0x40;
}

void ClearRTS()
{
	LCR = 0xbf; 
	EFR &= 0xbf;
}

void setBufferTriger()
{
	TLR = 0x40;
	
}

int GetInterruptStatus()
{
	if((IIR & 0x01) == 0) {
		switch((IIR & 0x0e)>>3) {
			case IIR_INTERRUPT_TRANSMIT:
				return TRANSMIT_INTERRUPT;
			case IIR_INTERRUPT_RECEIVER:
				return RECEIVER_INTERRUPT;
			default:
				return NOT_DEFINED_INTERRUPT;
		}
	} else {
		return NO_INTERRUPT;
	}
}

void setTxInterrupt()
{
	IER |= 2;
}

void setRxInterrupt()
{
	IER |= 1;
}


void clearInterrupt()
{
	IER = 0;	// no interrupts
}

 void setInterruptHandle(void (*handler) (void))
 {
	free_irq(COM_PORT);
	request_irq(COM_PORT, handler);
 }

 void freeInterruptHandle()
 {
	free_irq(COM_PORT);
 }

////////////////////////////////////////////////////////////////////////////////
// output_byte_serial_front
// PURPOSE: Prints a byte to the front serial port.
// PARAMS:  (IN) char byte - byte to print.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void output_byte_serial_front(char byte)
{
   //wait for room in the fifo.
   while((txFIFOEmpty()) == 0);

   THR = byte;
}

int txFIFOEmpty()
{
	return ((LSR & 0x20) != 0);
}

int rxfifoFull()
{
	return (LSR & 0x01 == 1);
}


////////////////////////////////////////////////////////////////////////////////
// input_byte_serial_front
// PURPOSE: Reads a byte from the front serial port.
// PARAMS:  (OUT) char *byte - byte read.
// RETURNS: 1 for success, 0 for failure.
////////////////////////////////////////////////////////////////////////////////
int input_byte_serial_front(char *byte)
{
   int error = 0;

   if( rxfifoFull())
   {
      error = (LSR & 0x0E); // overrun, parity, framming 
      if(error)
      {
         error = (int) RHR;  // flush the char in the FIFO
         return 0;
      }

      *byte = RHR;
      return 1;
   }
   else
   {
      return 0;
   }
}

