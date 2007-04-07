//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001 Intrinsyc Software Inc. All rights reserved.
//
// Module name:
//
//      serial.h
//
// Description:
//
//      Interfaces with the StrongArm SA1110 serial hardware.
//
// Author:
//
//      Mike Kirkland
//
// Created:
//
//      October 2001
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SERIAL_FRONT_H
#define SERIAL_FRONT_H

#include <types.h>
#include <serial.h>
#include <interrupt_sa.h>

/* TL16C7528 REGISTER */
#define	UART_BASE	0x18000000

#define RHR	*(volatile unsigned char*) (UART_BASE + 0x00)	// Receiver FIFO buffer
#define THR	*(volatile unsigned char*) (UART_BASE + 0x00)   // Transceiver FIFO buffer
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
#define TLR *(volatile unsigned char*) (UART_BASE + 0x0e)  

#define TRIGER_LEVEL 4

#define PARITY_NONE 0
#define PARITY_ODD 1
#define PARITY_EVEN 2

#define TRANSMIT_INTERRUPT 0
#define RECEIVER_INTERRUPT 1
#define NO_INTERRUPT 1
#define NOT_DEFINED_INTERRUPT 2

#define IIR_INTERRUPT_TRANSMIT 0
#define IIR_INTERRUPT_RECEIVER 1

#define COM_PORT 3

void init_serial_front(u32 baud);
void output_byte_serial_front(char byte);
int input_byte_serial_front(char *byte);

int GetInterruptStatus();
void setTxInterrupt();
void setRxInterrupt();
void clearInterrupt();
void setParity(int parity);
void initializeFIFO();
void SetRTS();
void ClearRTS();
void setBaudRate(u32 baud);
void setBufferTriger();

void setInterruptHandle(void (*handler) (void));
int txFIFOEmpty();
int rxfifoFull(); 

#endif //SERIAL_H
