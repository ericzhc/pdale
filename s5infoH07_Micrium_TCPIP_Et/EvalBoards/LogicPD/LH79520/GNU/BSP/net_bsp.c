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
* Filename      : net_bsp.c
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
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <net.h>
#include  <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  LAN91C111_PWR_CTRL_ADDR         (*(volatile CPU_INT08U *)0x54200000)


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                  NETWORK INTERFACE CARD FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          LAN91C111_PwrCtrl()
*
* Description : Control power to the LAN91C111, if supported by your application's hardware.  This feature
*               would be provided if power consumption is an issue in your application.
*
* Argument(s) : pwr     control power to the LAN91C111 :
*                   NET_ON      apply  power to   the LAN91C111
*                   NET_OFF     remove power from the LAN91C111
*
* Return(s)   : None.
*
* Caller(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  LAN91C111_PwrCtrl (CPU_BOOLEAN  pwr)
{
    if (pwr == DEF_ON) {
        LAN91C111_PWR_CTRL_ADDR = 0;
    } else {
        LAN91C111_PWR_CTRL_ADDR = 1;
    }
}


/*
*********************************************************************************************************
*                                         LAN91C111_DlyReset()
*
* Description : Implement a 50 ms delay after reset to allow the chip to stabilize.
*               This delay was recommended by SMSC (see SMSC AN9.6, Section 4.7.1).
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  LAN91C111_DlyReset (void)
{
    CPU_INT16U  ticks;


    ticks = OS_TICKS_PER_SEC / 20;
    OSTimeDly(ticks);
}


/*
*********************************************************************************************************
*                                       LAN91C111_DlyAutoNegAck()
*
* Description : Implement a 1.5 second delay after auto-negotiation initiation.
*               This delay was recommended by SMSC (see SMSC AN9.6, Section 4.7.1).
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  LAN91C111_DlyAutoNegAck (void)
{
    CPU_INT16U  ticks;


    ticks = OS_TICKS_PER_SEC + (OS_TICKS_PER_SEC / 2);
    OSTimeDly(ticks);
}


/*
*********************************************************************************************************
*                                      LAN91C111_PhyFrameClkDly()
*
* Description : Implement a brief delay after LAN91C111 Physical Frame clock pulses
*               (see 'NIC\ETHER\LAN91C111\net_drv.c  LAN91C111_PhyFrameClk()  Note #2').
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : Various.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  LAN91C111_PhyFrameClkDly (void)
{
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                      NETWORK MODULE FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          NetUtil_TS_Get()
*
* Description : Get current Internet Timestamp.
*
*               (1) "The Timestamp is a right-justified, 32-bit timestamp in milliseconds since midnight
*                    UT [Universal Time]" (RFC #791, Section 3.1 'Options : Internet Timestamp').
*
*               (2) The developer is responsible for providing a real-time clock with correct time-zone
*                   configuration to implement the Internet Timestamp.
*
*
* Argument(s) : None.
*
* Return(s)   : Internet Timestamp.
*
* Caller(s)   : Various.
*
*               This function is an INTERNAL network protocol suite function but MAY be called by
*               application function(s).
*
* Note(s)     : None.
*********************************************************************************************************
*/

NET_TS  NetUtil_TS_Get (void)
{
    NET_TS  ts;


    /* $$$$ Insert code to return Internet Timestamp (see Notes #1 & #2).   */

    ts = NET_TS_NONE;

    return (ts);
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                            TRANSMISSION CONTROL PROTOCOL LAYER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        NetTCP_InitTxSeqNbr()
*
* Description : Initialize the TCP Transmit Initial Sequence Counter, 'NetTCP_TxSeqNbrCtr'.
*
*               (1) Possible initialization methods include :
*
*                   (a) Time-based initialization is one preferred method since it more appropriately
*                       provides a pseudo-random initial sequence number.
*
*                   (b) Hardware-generated random number initialization is NOT a preferred method since it
*                       tends to produce a discrete set of pseudo-random initial sequence numbers--often
*                       the same initial sequence number.
*
*                   (c) Hard-coded initial sequence number is NOT a preferred method since it is NOT random.
*
*                   See also 'net_tcp.h  NET_TCP_TX_GET_SEQ_NBR()  Note #1'.
*
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : NetTCP_Init().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by
*               application function(s).
*
* Note(s)     : None.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetTCP_InitTxSeqNbr (void)
{
    /* $$$$ Insert code to initialize TCP Transmit Initial Sequence Counter (see Note #1). */

    NetTCP_TxSeqNbrCtr = NET_TCP_SEQ_NBR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetTCP_TxConnRTT_GetTS_ms()
*
* Description : Get a TCP Round-Trip Time (RTT) timestamp.
*
*               (1) (a) (1) Although RFC #2988, Section 4 states that "there is no requirement for the
*                           clock granularity G used for computing RTT measurements ... experience has
*                           shown that finer clock granularities (<= 100 msec) perform somewhat better
*                           than more coarse granularities".
*
*                       (2) (A) RFC #2988, Section 2.4 states that "whenever RTO is computed, if it is
*                               less than 1 second then the RTO SHOULD be rounded up to 1 second".
*
*                           (B) RFC #1122, Section 4.2.3.1 states that "the recommended ... RTO ... upper
*                               bound should be 2*MSL" where RFC #793, Section 3.3 'Sequence Numbers :
*                               Knowing When to Keep Quiet' states that "the Maximum Segment Lifetime
*                               (MSL) is ... to be 2 minutes".
*
*                               Therefore, the required upper bound is :
*
*                                   2 * MSL = 2 * 2 minutes = 4 minutes = 240 seconds
*
*                   (b) Therefore, the developer is responsible for providing a timestamp clock with
*                       adequate resolution to satisfy the clock granularity (see Note #1a1) & adequate
*                       range to satisfy the minimum/maximum RTO values (see Note #1a2).
*
*
* Argument(s) : None.
*
* Return(s)   : TCP RTT timestamp, in milliseconds.
*
* Caller(s)   : NetTCP_RxPktValidate(),
*               NetTCP_TxPktPrepareHdr().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by
*               application function(s).
*
* Note(s)     : (2) (a) To avoid timestamp calculation overflow, a maximum clock tick threshold value
*                       MUST be configured to truncate all clock tick values.
*
*                   (b) Also, since the clock tick integer will periodically overflow; the maximum
*                       threshold MUST be a multiple of the clock tick to avoid a discontinuity in
*                       the timestamp calculation when the clock tick integer overflows.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
NET_TCP_TX_RTT_TS_MS  NetTCP_TxConnRTT_GetTS_ms (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    NET_TCP_TX_RTT_TS_MS  rtt_ts_ms;
    CPU_INT32U  clk_tick;
    CPU_INT32U  clk_tick_th;


    CPU_CRITICAL_ENTER();
    clk_tick         = OSTime;
    CPU_CRITICAL_EXIT();

    if (OS_TICKS_PER_SEC > 0) {
        clk_tick_th  = (NET_TCP_TX_RTT_TS_MAX /                 /* MUST cfg clk tick th ...                             */
                        DEF_TIME_NBR_mS_PER_SEC);               /* ... as multiple of max ts (see Note #2b).            */
        clk_tick    %=  clk_tick_th;                            /* Truncate clk ticks to avoid ovf (see Note #2a).      */

        rtt_ts_ms    = (NET_TCP_TX_RTT_TS_MS)((clk_tick * DEF_TIME_NBR_mS_PER_SEC) / OS_TICKS_PER_SEC);
    } else {
        rtt_ts_ms    = (NET_TCP_TX_RTT_TS_MS)clk_tick;
    }

    return (rtt_ts_ms);
}
#endif
