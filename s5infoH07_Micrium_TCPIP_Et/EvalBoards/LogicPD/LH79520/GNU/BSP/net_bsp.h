/*
*********************************************************************************************************
*                                              uC/TCP-IP
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/TCP-IP is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/TCP-IP in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to help
*               you experience uC/TCP-IP.  The fact that the source code is provided does
*               NOT mean that you can use it without paying a licensing fee.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                            NETWORK BOARD SUPPORT PACKAGE (BSP) FUNCTIONS
*
*                                              TEMPLATE
*
* Filename      : net_bsp.h
* Version       : V1.88
* Programmer(s) : ITJ
*                 Jean-Denis Hatier
*********************************************************************************************************
* Note(s)       : (1) To provide the required Board Support Package functionality, insert the appropriate
*                     board-dependent code to perform the stated actions wherever '$$$$' comments are found.
*
*                     #### This note MAY be entirely removed for specific board support packages.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_BSP_MODULE
#define  NET_BSP_EXT
#else
#define  NET_BSP_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  LAN91C111_REG_ADDR_BASE                  0x54000300


/*
*********************************************************************************************************
*                                              MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       NIC READ/WRITE MACRO'S
*
* Description : Read/Write data values to & from NIC.
*
* Argument(s) : Various.
*
* Return(s)   : Various.
*
* Caller(s)   : Specific NIC Read/Write function(s).
*
*               These macro's are network protocol suite to network interface controller (NIC) macro's
*               & SHOULD be called only by appropriate network interface controller function(s).
*
* Note(s)     : (1) When NET_NIC_CFG_RD_WR_SEL configured to NET_NIC_RD_WR_SEL_MACRO, implement NIC read/write
*                   functionality with macro's.
*
*                   (a) The developer MUST implement the NIC read/write macro functions with the correct number
*                       & type of arguments necessary to implement the NIC's read/write functionality.
*
*                   (b) The developer SHOULD appropriately name the NIC read/write macro functions by appending
*                       the number of bits that the macro functions read/write to the end of the macro function
*                       name :
*                               NetNIC_Rd_xx()
*                               NetNIC_Wr_xx()
*
*                       For example, 16-bit read/write macro functions should be named :
*                               NetNIC_Rd_16()
*                               NetNIC_Wr_16()
*
*                   See also 'net_bsp.c  NetNIC_Rd_xx()  Note #1'
*                          & 'net_bsp.c  NetNIC_Wr_xx()  Note #1'.
*********************************************************************************************************
*/

#if (NET_NIC_CFG_RD_WR_SEL != NET_NIC_RD_WR_SEL_FNCT)

#define  NetNIC_Rd_16(reg_offset)       (*((volatile CPU_INT16U *)(LAN91C111_REG_ADDR_BASE + reg_offset)))
#define  NetNIC_Wr_16(reg_offset, val)  (*((volatile CPU_INT16U *)(LAN91C111_REG_ADDR_BASE + reg_offset)) = val)

#endif
