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
*                                BOARD SUPPORT PACKAGE (BSP) FUNCTIONS
*
*                                  TCP/IP STACK ISR SUPPORT FUNCTIONS
*
* Filename      : net_isr.c
* Version       : V1.88
* Programmer(s) : Jean-Denis Hatier
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <net.h>
#include  <reg_LH79520.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  LAN91C111_PWR_CTRL_ADDR  (*(volatile CPU_INT08U *)0x54200000)
#define  WLAN_INT_MSK_ADDR        (*(volatile CPU_INT08U *)0x54C00000)


/*
*********************************************************************************************************
*                                   NETWORK INTERRUPT INITIALIZATION
*
* Description : This function is called to initialize the interrupt controller associated with the NIC.
*
* Arguments   : None.
*********************************************************************************************************
*/

void  NetNIC_IntInit (void)
{
    LAN91C111_PWR_CTRL_ADDR    = 0x00;                          /* WLAN Power Enable.                                   */
    OSTimeDly(OS_TICKS_PER_SEC / 10);                           /* Delay 1/10 second to power up.                       */

    WLAN_INT_MSK_ADDR          = 0x13;                          /* WLAN Interrupts Enabled.                             */

                                                                /* LAN91C111 is on EXT #4 on LogicPD.                   */
    RCPC->intclear             =   RCPC_INTCLEAR(VIC_EXINT4);

                                                                /* Enable low level triggered interrupt.                */
    RCPC->intconfig           &= ~(RCPC_INTCONFIG(RCPC_INT4, 0x03));
    RCPC->intconfig           |=   RCPC_INTCONFIG(RCPC_INT4, RCPC_INT_LLT);

                                                                /* LAN91C111 is on EXT #4 on Logic PD.                  */
    VIC->vectcntl[VIC_VECT_1]  = VIC_VECTCNTL_ENABLE | VIC_EXINT4;
    VIC->vectaddr[VIC_VECT_1]  = (CPU_INT32U)NetNIC_ISR_Handler;
    VIC->intenable             = VIC_INT_ENABLE(VIC_EXINT4);
}


/*
*********************************************************************************************************
*                                        NETWORK INTERRUPT CLEAR
*
* Description : This function is called to clear the interrupt controller associated with the NIC.
*
* Arguments   : None.
*********************************************************************************************************
*/

void  NetNIC_IntClr (void)
{
                                                                /* LAN91C111 is on EXT #4 on Logic PD.                  */
    RCPC->intclear  = RCPC_INTCLEAR(VIC_EXINT4);                /* Clear the interrupt.                                 */
    VIC->vectoraddr = VIC_VECTORADDR_CLEAR;                     /* Clear the vector address register.                   */
}
