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
void init_serial_front(short config)
{
	switch (config) {
		case GPS_CONFIG:
			#if DEBUG
				printf("Using GPS config\n\r");
			#endif
			ConfigGPS();
			break;
		case BCREADER_CONFIG:
			#if DEBUG
				printf("Using barcode reader config\n\r");
			#endif
			ConfigBCR();
			break;
		default:
			#if DEBUG
				printf("Using default config\n\r");
			#endif
			// 9600, no parity, 1 stop bit, 8 bit config, FIFO enabled
			
			
			int temp1;
	
			temp1 = LCR;

			EFR = 0x10;		// enable enhanced fucntions access
			IER = 0;		// disable all interrupts
			FCR = 0x06;		// clear Rx and Tx FIFOs
			FCR = 0xA1;		// FIFO trigger levels: tx=32, rx=32
			EFR = 0x00;     // disable enhanced functions access
			LCR = 0x03;     // 8 data bits, 1 stop bit, no parity		
			setBaudRate(SF_9600_BAUDS);
			MCR = 0x0F;     //  RTS = on
							//  DTR = on
							//  FIFO Rdy enable = on
							//  IRQ enable  = on
			temp1 = MSR;
			break;
	}


	GAFR_SF &= 0xfffffff7;			// GAFR.3 = 0 (disable Alternate function on pin 3)
	GPDR_SF &= 0xfffffff7;			// define GPIO.3 as input
	GFER_SF &= 0xfffffff7;			// GFER.3 = 0 (disable falling edge on pin 3)
	GRER_SF |= 0x08;				// GRER.3 = 1 (enable rising edge on pin 3)
	GEDR_SF = 0x08;					// clear GEDR.3 
	
	
	//setBufferTriger();
	setRxInterrupt();
	
	#if DEBUG
		printf("Serial front driver init...done\n\r");
	#endif
}

void ConfigGPS() 
{
	// TSIP: 9600, odd parity, 1 stop bit, 8 bit config, FIFO enabled
	// page 40 - Lassen IQ Reference
	int temp1;
	
	temp1 = LCR;

	EFR = 0x10;		// enable enhanced fucntions access
	IER = 0;		// disable all interrupts
	FCR = 0x06;		// clear Rx and Tx FIFOs
	FCR = 0xA1;		// FIFO trigger levels: tx=32, rx=32
	EFR = 0x00;     // disable enhanced functions access
	LCR = 0x03;     // 8 data bits, 1 stop bit, no parity		
	setBaudRate(SF_9600_BAUDS);
	setParity(PARITY_ODD);
	MCR = 0x0F;     //  RTS = on
					//  DTR = on
					//  FIFO Rdy enable = on
					//  IRQ enable  = on
	temp1 = MSR;
}

void ConfigBCR()
{
	int temp1;
	
	temp1 = LCR;

	EFR = 0x10;		// enable enhanced fucntions access
	IER = 0;		// disable all interrupts
	FCR = 0x06;		// clear Rx and Tx FIFOs
	FCR = 0xA1;		// FIFO trigger levels: tx=32, rx=32
	EFR = 0x00;     // disable enhanced functions access
	LCR = 0x03;     // 8 data bits, 1 stop bit, no parity		
	setBaudRate(SF_9600_BAUDS);
	MCR = 0x0F;     //  RTS = on
					//  DTR = on
					//  FIFO Rdy enable = on
					//  IRQ enable  = on
	temp1 = MSR;
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

	LCR &= 0x7f;	// Deselect DLL-DLH
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
	int temp1;
	
	temp1 = LCR;
	LCR = 0xBF;
	EFR |= 0x10;

	FCR = 0x0;		// desactivate FIFO before configuration (bit 0)
	FCR |= 0x06;	// clear Rx and Tx FIFOs
	
	FCR &= 0xF9;
	//FCR &= 0xF7;	// DMA mode 0, bit 3
	FCR |= 0x04;	// DMA mode 0, bit 3
	
	FCR &= 0x0F;	// set trigger levels to 8 spaces/characters
	FCR |= 0x01;	// activate FIFO

	EFR &= 0xBF;
	LCR = temp1;
}

void SetCTS()
{
	LCR |= 0x80; // activate EFR 
	EFR |= 0x80; // Auto-CTS enabled
}

void ClearCTS()
{
	LCR &= 0x7f;  // desactivate EFR
	EFR &= 0x7f;  // Auto-CTS disabled
}

void SetRTS()
{
	LCR |= 0x80; 
	EFR |= 0x40;
}

void ClearRTS()
{
	LCR &= 0x7f; // desactivate EFR
	EFR &= 0xbf; // Auto-RTS off
}

void setBufferTriger() 
{
	int temp1;
	
	temp1 = LCR;
	LCR = 0xBF;
	EFR |= 0x10;
	MCR |= 0x40;
	TLR = 0x11;
	MCR &= 0xbf;
	EFR &= 0xBF;
	LCR = temp1;
}

int GetInterruptStatus() // TODO: Fix interrupt masks
{
	if((IIR & 0x01) == 0) {
		switch((IIR)) {
			case IIR_INTERRUPT_TRANSMIT:
				return TRANSMIT_INTERRUPT;
			case IIR_INTERRUPT_RECEIVER:
				return RECEIVER_INTERRUPT;
			case IIR_INTERRUPT_RECEIVER_TIMEOUT:
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
	return ((LSR & 0x01) == 1);
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

