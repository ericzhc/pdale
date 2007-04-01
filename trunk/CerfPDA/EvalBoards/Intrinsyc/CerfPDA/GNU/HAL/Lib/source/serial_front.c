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
void
init_serial_front(u32 baud)
{
	LCR |= 0x80;	// Select DLL-DLH
	setBaudRate(baud);
	DLH = 0x00;
	
	MCR = 0x08;	// OP output to low to activate the RS-232 buffer

	initializeFIFO();
}

void setBaudRate(u32 baud)
{
	// To be done
	DLL = 0x90;	// 9600 bauds
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
void setParity(int parity)
{
	switch (parity) {
		case PARTITY_NONE:	
			LCR = 0x03;
			break;
		case PARTITY_ODD:
			LCR = 0x0b;
			break;
		case PARTITY_EVEN:
			LCR = 0x1b;
			break;
		default:
			LCR = 0x03;
			break;
	}
}



void initializeFIFO()
{
	FCR = 0x04;	// clear Rx and Tx FIFOs and 8 spaces (page 25)
	FCR = 0x01;	// activate FIFO

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
	TLR = 0x44;
}

////////////////////////////////////////////////////////////////////////////////
// output_byte_serial_front
// PURPOSE: Prints a byte to the front serial port.
// PARAMS:  (IN) char byte - byte to print.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
output_byte_serial_front(char byte)
{
   //wait for room in the fifo.
   while((LSR & 0x20) == 0);

   THR = byte;
}

////////////////////////////////////////////////////////////////////////////////
// input_byte_serial_front
// PURPOSE: Reads a byte from the front serial port.
// PARAMS:  (OUT) char *byte - byte read.
// RETURNS: 1 for success, 0 for failure.
////////////////////////////////////////////////////////////////////////////////
int
input_byte_serial_front(char *byte)
{
   int error = 0;

   if(LSR & 0x01)
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


