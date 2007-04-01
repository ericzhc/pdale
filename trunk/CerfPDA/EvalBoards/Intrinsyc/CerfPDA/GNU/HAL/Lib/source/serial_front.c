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

#define	UART_BASE	0x18000000

#define RHR	*(volatile unsigned char*) (UART_BASE + 0x00)
#define THR	*(volatile unsigned char*) (UART_BASE + 0x00)
#define IER	*(volatile unsigned char*) (UART_BASE + 0x02)
#define IIR	*(volatile unsigned char*) (UART_BASE + 0x04)
#define FCR	*(volatile unsigned char*) (UART_BASE + 0x04)
#define LCR	*(volatile unsigned char*) (UART_BASE + 0x06)
#define MCR	*(volatile unsigned char*) (UART_BASE + 0x08)
#define LSR	*(volatile unsigned char*) (UART_BASE + 0x0A)
#define MSR	*(volatile unsigned char*) (UART_BASE + 0x0C)
#define DLL	*(volatile unsigned char*) (UART_BASE + 0x00)
#define DLH	*(volatile unsigned char*) (UART_BASE + 0x02)
#define EFR *(volatile unsigned char*) (UART_BASE + 0x04) // Page 30, RTS = bit 6

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
	DLL = 0x90;	// 9600 bauds
	DLH = 0x00;
	LCR = 0x03;	// 8 bit, 1 stop, no parity
	
	IER = 0;	// no interrupts

	MCR = 0x08;	// OP output to low to activate the RS-232 buffer

	FCR = 0x04;	// clear Rx and Tx FIFOs
	FCR = 0x01;	// activate FIFO

}

void SetRTS () 
{
	LCR = 0xbf; 
	EFR |= 0x40;
}

void ClearRTS () 
{
	LCR = 0xbf; 
	EFR &= 0xbf;
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


