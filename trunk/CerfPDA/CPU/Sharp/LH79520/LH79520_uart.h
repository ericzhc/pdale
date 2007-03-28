/**********************************************************************
 *  $Workfile:   LH79520_uart.h  $
 *  $Revision:   1.4  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:52  $
 *
 *  Project: LH79520 IP headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for component:
 *
 *          LH79520 UART/IrDA
 *
 *  References:
 *      (1) Sharp LH79520 User's Manual, V3.0 (revised)
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_uart.h-arc  $
 * 
 *    Rev 1.4   01 Apr 2002 09:32:52   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.3   07 Mar 2002 10:19:38   kovitzp
 * Added UARTLCR_PARITY_EVEN & UARTLCR_PARITY_ODD
 * 
 *    Rev 1.2   Jan 04 2002 12:06:56   BarnettH
 * Changed deprecated #ifndef
 * 
 *    Rev 1.1   Dec 05 2001 09:34:32   BarnettH
 * Changed comments
 * 
 *    Rev 1.0   Dec 04 2001 16:23:20   BarnettH
 * Initial revision.
 * 
 * SHARP MICROELECTRONICS OF THE AMERICAS MAKES NO REPRESENTATION
 * OR WARRANTIES WITH RESPECT TO THE PERFORMANCE OF THIS SOFTWARE,
 * AND SPECIFICALLY DISCLAIMS ANY RESPONSIBILITY FOR ANY DAMAGES, 
 * SPECIAL OR CONSEQUENTIAL, CONNECTED WITH THE USE OF THIS SOFTWARE.
 *
 * SHARP MICROELECTRONICS OF THE AMERICAS PROVIDES THIS SOFTWARE SOLELY 
 * FOR THE PURPOSE OF SOFTWARE DEVELOPMENT INCORPORATING THE USE OF A 
 * SHARP MICROCONTROLLER OR SYSTEM-ON-CHIP PRODUCT. USE OF THIS SOURCE
 * FILE IMPLIES ACCEPTANCE OF THESE CONDITIONS.
 *
 *  COPYRIGHT (C) 2001 SHARP MICROELECTRONICS OF THE AMERICAS, INC.
 *      CAMAS, WA
 *********************************************************************/

#if !defined LH79520_UART_H
#define LH79520_UART_H

#include "SMA_types.h"

/*********************************************************************** 
 * UART Module Register Structure
 **********************************************************************/

typedef struct 
{
    volatile UNS_32 dr;         /* Data */ 
    volatile UNS_32 rsr_ecr;    /* Receive Status (read)
                                   Error Clear (write) */ 
    volatile UNS_32 reserveduart1[4];
    volatile UNS_32 fr;         /* Flag */ 
    volatile UNS_32 reserveduart2;
    volatile UNS_32 ilpr;       /* IrDA Low Power Counter */ 
    volatile UNS_32 ibrd;       /* Integer Baud Rate Divisor */ 
    volatile UNS_32 fbrd;       /* Fractional Baud Rate Divisor */ 
    volatile UNS_32 lcr_h;      /* Line Control HIGH Byte */ 
    volatile UNS_32 cr;         /* Control */ 
    volatile UNS_32 ifls;       /* Interrupt FIFO Level Select */ 
    volatile UNS_32 imsc;       /* Interrupt Mask Set/Clear */ 
    volatile UNS_32 ris;        /* Raw Interrupt Status */ 
    volatile UNS_32 mis;        /* Masked Interrupt Status */ 
    volatile UNS_32 icr;        /* Interrupt Clear */ 
} UARTREGS;

/*********************************************************************** 
 * UART Data Register Bit Field
 **********************************************************************/ 
#define UARTDR_DATA             ((n)&_BITMASK(8))  /* Data (8 bits) */ 
#define UARTDR_FE               _BIT(8)     /* Framing Error */ 
#define UARTDR_PE               _BIT(9)     /* Parity Error */ 
#define UARTDR_BE               _BIT(10)    /* Break Error */ 
#define UARTDR_OE               _BIT(11)    /* Overrun Error */ 

/***********************************************************************
 * UART Receive Status/Error Clear Register Bit Field
 **********************************************************************/ 
#define UARTECR_CLEAR           0           /* Error Clear */ 
#define UARTRSR_FE              _BIT(0)     /* Framing Error */ 
#define UARTRSR_PE              _BIT(1)     /* Parity Error */ 
#define UARTRSR_BE              _BIT(2)     /* Break Error */ 
#define UARTRSR_OE              _BIT(3)     /* Overrun Error */ 

/***********************************************************************
 * UART Flag Register Bit Field
 **********************************************************************/ 
#define UARTFR_BUSY         _BIT(3)     /* Busy */ 
#define UARTFR_RXFE         _BIT(4)     /* Receive FIFO Empty */ 
#define UARTFR_TXFF         _BIT(5)     /* Transmit FIFO Full */ 
#define UARTFR_RXFF         _BIT(6)     /* Receive FIFO Full */ 
#define UARTFR_TXFE         _BIT(7)     /* Transmit FIFO Empty */ 

/*********************************************************************** 
 * UART IRDA Low Power Counter Register Bit Field
 **********************************************************************/
/* Low Power Divisor */ 
#define UARTILPDVSR(n)      _SBF(0,((n)&_BITMASK(8)))

/*********************************************************************** 
 * UART Integer Baud Rate Register Bit Field
 **********************************************************************/
/* Integer Baud Rate Divisor */ 
#define UARTBAUDDIVINT(n)   (_SBF(0,((n)&_BITMASK(16))))
/* The following constants assume UARTx Clock = 14.745600 MHz */ 
#define UARTBRINT_110       UARTBAUDDIVINT(0x20BA)
#define UARTBRINT_1200      UARTBAUDDIVINT(0x300)
#define UARTBRINT_2400      UARTBAUDDIVINT(0x180)
#define UARTBRINT_4800      UARTBAUDDIVINT(0xC0)
#define UARTBRINT_9600      UARTBAUDDIVINT(0x60)
#define UARTBRINT_14400     UARTBAUDDIVINT(0x40)
#define UARTBRINT_19200     UARTBAUDDIVINT(0x30)
#define UARTBRINT_38400     UARTBAUDDIVINT(0x18)
#define UARTBRINT_57600     UARTBAUDDIVINT(0x10)
#define UARTBRINT_115200    UARTBAUDDIVINT(0x8)
#define UARTBRINT_230400    UARTBAUDDIVINT(0x4)
#define UARTBRINT_460800    UARTBAUDDIVINT(0x2)

/*********************************************************************** 
 * UART Fractional Baud Rate Register Bit Field
 **********************************************************************/
/* Fractional Baud Rate Divisor */ 
#define UARTBAUDDIVFRAC(n)  _SBF(0,((n)&_BITMASK(7)))
/* The following constants assume UARTx Clock = 14.745600 MHz */
#define UARTBRFRAC_110       0
#define UARTBRFRAC_1200      0
#define UARTBRFRAC_2400      0
#define UARTBRFRAC_4800      0
#define UARTBRFRAC_9600      0
#define UARTBRFRAC_14400     0
#define UARTBRFRAC_19200     0
#define UARTBRFRAC_38400     0
#define UARTBRFRAC_57600     0
#define UARTBRFRAC_115200    0
#define UARTBRFRAC_230400    0
#define UARTBRFRAC_460800    0

/***********************************************************************
 * UART Line Control Register Bit Field
 **********************************************************************/
#define UARTLCR_BRK         _BIT(0)     /* Send Break */ 
#define UARTLCR_PEN         _BIT(1)     /* Parity Enable */ 
#define UARTLCR_EPS         _BIT(2)     /* Even Parity Select */ 
#define UARTLCR_STP1        0           /* One Stop Bit Select */ 
#define UARTLCR_STP2        _BIT(3)     /* Two Stop Bits Select */ 
#define UARTLCR_FEN         _BIT(4)     /* Enable FIFOs */ 
#define UARTLCR_WLEN5       _SBF(5,0)   /* Word Length 5 bits */ 
#define UARTLCR_WLEN6       _SBF(5,1)   /* Word Length 6 bits */ 
#define UARTLCR_WLEN7       _SBF(5,2)   /* Word Length 7 bits */ 
#define UARTLCR_WLEN8       _SBF(5,3)   /* Word Length 8 bits */ 
#define UARTLCR_SPS         _BIT(7)     /* Stick Parity Select */ 
#define UARTLCR_PARITY_NONE 0
#define UARTLCR_PARITY_EVEN (UARTLCR_PEN | UARTLCR_EPS)
#define UARTLCR_PARITY_ODD  (UARTLCR_PEN)

/***********************************************************************
 * UART Control Register Bit Field
 **********************************************************************/
#define UARTCR_ENABLE       _BIT(0)     /* UART Enable */ 
#define UARTCR_SIREN        _BIT(1)     /* SIR Enable */ 
#define UARTCR_SIRLP        _BIT(2)     /* SIR Low Power Mode */ 
#define UARTCR_LBE          _BIT(7)     /* Loopback Enable */ 
#define UARTCR_TXE          _BIT(8)     /* Transmit Enable */ 
#define UARTCR_RXE          _BIT(9)     /* Receive Enable */ 

/***********************************************************************
 * UART Interrupt FIFO Level Select Register Bit Field
 **********************************************************************/
/* Transmit Interrupt FIFO Level Select */ 
#define UARTIFLS_TX(n)          _SBF(0,((n)&_BITMASK(3)))
#define UARTIFLS_TX_1_8         UARTIFLS_TX(0)
#define UARTIFLS_TX_1_4         UARTIFLS_TX(1)
#define UARTIFLS_TX_1_2         UARTIFLS_TX(2)
#define UARTIFLS_TX_3_4         UARTIFLS_TX(3)
#define UARTIFLS_TX_7_8         UARTIFLS_TX(4)

/* Receive Interrupt FIFO Level Select */ 
#define UARTIFLS_RX(n)          _SBF(3,((n)&_BITMASK(3)))
#define UARTIFLS_RX_1_8         UARTIFLS_RX(0)
#define UARTIFLS_RX_1_4         UARTIFLS_RX(1)
#define UARTIFLS_RX_1_2         UARTIFLS_RX(2)
#define UARTIFLS_RX_3_4         UARTIFLS_RX(3)
#define UARTIFLS_RX_7_8         UARTIFLS_RX(4)

/***********************************************************************
 * UART Interrupt Mask Set/Clear Register Bit Field
 * UART Raw Interrupt Status Register Bit Field
 * UART Interrupt Status Register Bit Field
 * UART Interrupt Clear Register Bit Field
 **********************************************************************/
/* Interrupt Bits for all Interrupt Registers */ 
#define UARTINT_RX       _BIT(4)     /* Receive */ 
#define UARTINT_TX       _BIT(5)     /* Transmit */ 
#define UARTINT_RT       _BIT(6)     /* Receive Timeout */ 
#define UARTINT_FE       _BIT(7)     /* Framing Error */ 
#define UARTINT_PE       _BIT(8)     /* Parity Error */ 
#define UARTINT_BE       _BIT(9)     /* Break Error */ 
#define UARTINT_OE       _BIT(10)    /* Overrun Error */ 

#endif /* LH79520_UART_H */ 
