/**********************************************************************
 *  $Workfile:   SMA_ascii.h  $
 *  $Revision:   1.1  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:31:06  $
 *
 *  Project: Common Include Files
 *
 *  Description:
 *      SMA_ascii.h contains the Sharp SMA #defines for ASCII 
 *      (American Standard Code for Information Interchange)
 *      non-printing characters.
 *
 *  Reference:
 *      ANSI 3.4-1986 (R1997), "Information Systems - Coded Character
 *      Sets - 7-Bit American National Standard Code for Information
 *      Interchange (7-Bit ASCII)" 
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/include/SMA_ascii.h-arc  $
 * 
 *    Rev 1.1   01 Apr 2002 09:31:06   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.0   Feb 11 2002 18:07:04   BarnettH
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

#ifndef SMA_ASCII_H
#define SMA_ASCII_H

#define ASCII_NUL           (0x00)
#define ASCII_CTRL_@        (0x00)
#define ASCII_SOH           (0x01)
#define ASCII_CTRL_A        (0x01)
#define ASCII_STX           (0x02)
#define ASCII_CTRL_B        (0x02)
#define ASCII_ETX           (0x03)
#define ASCII_CTRL_C        (0x03)
#define ASCII_EOT           (0x04)
#define ASCII_CTRL_D        (0x04)
#define ASCII_ENQ           (0x05)
#define ASCII_CTRL_E        (0x05)
#define ASCII_ACK           (0x06)
#define ASCII_CTRL_F        (0x06)
#define ASCII_BEL           (0x07)
#define ASCII_CTRL_G        (0x07)
#define ASCII_BS            (0x08)
#define ASCII_CTRL_H        (0x08)
#define ASCII_HT            (0x09)
#define ASCII_TAB           (0x09)
#define ASCII_CTRL_I        (0x09)
#define ASCII_LF            (0x0A)
#define ASCII_CTRL_J        (0x0A)
#define ASCII_VT            (0x0B)
#define ASCII_CTRL_K        (0x0B)
#define ASCII_FF            (0x0C)
#define ASCII_CTRL_L        (0x0C)
#define ASCII_CR            (0x0D)
#define ASCII_CTRL_M        (0x0D)
#define ASCII_SO            (0x0E)
#define ASCII_CTRL_N        (0x0E)
#define ASCII_SI            (0x0F)
#define ASCII_CTRL_O        (0x0F)
#define ASCII_DLE           (0x10)
#define ASCII_CTRL_P        (0x10)
#define ASCII_DC1           (0x11)
#define ASCII_CTRL_Q        (0x11)
#define ASCII_DC2           (0x12)
#define ASCII_CTRL_R        (0x12)
#define ASCII_DC3           (0x13)
#define ASCII_CTRL_S        (0x13)
#define ASCII_DC4           (0x14)
#define ASCII_CTRL_T        (0x14)
#define ASCII_NAK           (0x15)
#define ASCII_CTRL_U        (0x15)
#define ASCII_SYN           (0x16)
#define ASCII_CTRL_V        (0x16)
#define ASCII_ETB           (0x17)
#define ASCII_CTRL_W        (0x17)
#define ASCII_CAN           (0x18)
#define ASCII_CTRL_X        (0x18)
#define ASCII_EM            (0x19)
#define ASCII_CTRL_Y        (0x19)
#define ASCII_SUB           (0x1A)
#define ASCII_CTRL_Z        (0x1A)
#define ASCII_ESC           (0x1B)
#define ASCII_CTRL_LBRAK    (0x1B)
#define ASCII_FS            (0x1C)
#define ASCII_CTRL_BAKSL    (0x1C)
#define ASCII_GS            (0x1D)
#define ASCII_CTRL_RBRAK    (0x1D)
#define ASCII_RS            (0x1E)
#define ASCII_CTRL_CARET    (0x1E)
#define ASCII_US            (0x1F)
#define ASCII_CTRL__        (0x1F)
#define ASCII_SP            (0x20)
#define ASCII_DEL           (0x7F)

#endif /* SMA_ASCII_H */
