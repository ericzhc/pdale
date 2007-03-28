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
*               Network Interface Card (NIC) port files provided, as is, for FREE and do
*               NOT require any additional licensing or licensing fee.
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
*                                        NETWORK INTERFACE CARD
*
*                                              LAN91C111
*
* Filename      : net_nic.h
* Version       : V1.88
* Programmer(s) : ITJ
*                 JJL
*********************************************************************************************************
* Note(s)       : (1) Supports LAN91C111 Ethernet controller as described in
*
*                         Standard Microsystems Corporation's (SMSC; http://www.smsc.com)
*                         (a) LAN91C111 data sheet            (SMSC LAN91C111; Revision B 09/17/2002)
*                         (b) LAN91C111 Application Note 9.6  (SMSC AN9.6;     Revision   11/25/2002)
*
*                 (2) REQUIREs Ethernet Network Interface Layer located in the following network directory :
*
*                         \<Network Protocol Suite>\IF\Ether\
*
*                              where
*                                      <Network Protocol Suite>    directory path for network protocol suite
*
*                 (3) LAN91C111 is implemented with a Little-endian architecture.
*
*                     (a) This driver accesses ALL registers & data as 16-bit Little-endian words.
*                         I.e. lower-octets start on an even-valued address/offset while upper-octets 
*                         immediately follow      on an odd -valued address/offset equal to the lower-
*                         octet address/offset plus one (1).
*
*                     (b) This driver requires that ONLY the 16 least significant data lines (D15 - D00) 
*                         are used & that the LAN91C111's Byte Enable lines (BE3 - BE0) are configured to 
*                         access 16-bit words (see SMSC AN9.6, Section 3.5.10) :
*
*                                  BE0 = BE1 = Active-Low 0        Lower-16 bits  ENABLED
*                                  BE1 = BE2 = Active-Low 1        Upper-16 bits DISABLED
*
*                     (c) Since this driver implements Little-endian word access (see Note #3a), LAN91C111
*                         register access does NOT require octet-swapping.
*
*                         Therefore, for connection to any processor, Big-endian or Little-endian, the 
*                         LAN91C111's 16 least significant data lines should be directly connected to the 
*                         processor's 16 least significant data lines :
*
*                                        -------------                 -------------
*                                        | LAN91C111 |                 |    CPU    |
*                                        |           |                 |           |
*                                        |           |                 |           |
*                                        |      D07- |        8        | D07-      |
*                                        |      D00  |--------/--------| D00       |
*                                        |           |                 |           |
*                                        |      D15- |        8        | D15-      |
*                                        |      D08  |--------/--------| D08       |
*                                        |           |                 |           |
*                                        -------------                 -------------
*
*                         Product-specific I/O functions NetNIC_Rd_16() & NetNIC_Wr_16() MUST NOT swap 
*                         the upper- & lower-octets for 16-bit word accesses.
*
*
*                         If the connection to any processor, Big-endian or Little-endian, octet-swaps 
*                         the LAN91C111's 16 least significant data lines with the processor's 16 least 
*                         significant data lines :
*
*                                        -------------                 -------------
*                                        | LAN91C111 |                 |    CPU    |
*                                        |           |                 |           |
*                                        |           |                 |           |
*                                        |      D07- |        8        | D15-      |
*                                        |      D00  |--------/--------| D08       |
*                                        |           |                 |           |
*                                        |      D15- |        8        | D07-      |
*                                        |      D08  |--------/--------| D00       |
*                                        |           |                 |           |
*                                        -------------                 -------------
*
*                         then product-specific I/O functions NetNIC_Rd_16() & NetNIC_Wr_16() MUST re-swap 
*                         the upper- & lower-octets for 16-bit word accesses using LAN91C111_NIC_TO_HOST_16()
*                         & LAN91C111_HOST_TO_NIC_16() macro's, respectively.
*
*$PAGE
*                     (d) Since this driver implements Little-endian word access (see Note #3a), LAN91C111 
*                         data access to & from packet frames MUST be octet-swapped for connections to a 
*                         Big-endian processor.  However, the driver performs this octet-swap internally 
*                         through use of LAN91C111_NIC_TO_HOST_16() & LAN91C111_HOST_TO_NIC_16() macro's
*                         (see this 'net_nic.c  LAN91C111 WORD ORDER - TO - CPU WORD ORDER').
*
*                 (4) #### Link detection & recovery state machine NOT yet implemented.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_NIC_MODULE
#define  NET_NIC_EXT
#else
#define  NET_NIC_EXT  extern
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                               NIC DRIVER LAYER / ETHERNET ERROR CODES
*
* Note(s) : (1) ALL NIC-independent    error codes #define'd in      'net_err.h';
*               ALL LAN91C111-specific error codes #define'd in this 'net_nic.h'.
*
*           (2) Network error code '10,000' series reserved for NIC drivers.
*********************************************************************************************************
*/

#define  LAN91C111_ERR_TX_ALLOC                        10600


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

NET_NIC_EXT  CPU_BOOLEAN  NetNIC_ConnStatus;                    /* NIC's connection status : DEF_ON/DEF_OFF.            */


#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)                        /* ------------------- NET DRV STATS ------------------ */
NET_NIC_EXT  NET_CTR      NetNIC_StatRxPktCtr;
NET_NIC_EXT  NET_CTR      NetNIC_StatTxPktCtr;
#endif

#if (NET_CTR_CFG_ERR_EN  == DEF_ENABLED)                        /* ------------------- NET DRV ERRS ------------------- */
NET_NIC_EXT  NET_CTR      NetNIC_ErrRxPktDiscardedCtr;
NET_NIC_EXT  NET_CTR      NetNIC_ErrTxPktDiscardedCtr;
#endif


                                                                /* ---------------- LAN91C111 VARIABLES --------------- */
NET_NIC_EXT  CPU_INT16U   LAN91C111_RegBankCur;                 /* Maintain state of cur reg bank.                      */

NET_NIC_EXT  CPU_INT16U   LAN91C111_TxPktNbr;                   /* Maintain one global pkt frame for ALL tx's.          */


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         NetNIC_Init             (NET_ERR      *perr);

void         NetNIC_IntEn            (NET_ERR      *perr);



                                                                /* ------------------- STATUS FNCTS ------------------- */
void         NetNIC_ConnStatusChk    (void);

CPU_BOOLEAN  NetNIC_ConnStatusGet    (void);




void         NetNIC_ISR_Handler      (void);                    /* Decode & handle rx/tx ISRs.                          */



                                                                /* --------------------- RX FNCTS --------------------- */
CPU_INT16U   NetNIC_RxPktGetSize     (void);                    /* Get NIC rx pkt size.                                 */

void         NetNIC_RxPkt            (void         *ppkt,       /* Rx pkt from NIC.                                     */
                                      CPU_INT16U    size,
                                      NET_ERR      *perr);

void         NetNIC_RxPktDiscard     (CPU_INT16U    size,       /* Discard rx pkt in NIC.                               */
                                      NET_ERR      *perr);



                                                                /* --------------------- TX FNCTS --------------------- */
void         NetNIC_TxPkt            (void         *ppkt,
                                      CPU_INT16U    size,
                                      NET_ERR      *perr);



                                                                /* ---------------- LAN91C111 REG FNCTS --------------- */
#if (NET_DBG_CFG_TEST_EN == DEF_ENABLED)
CPU_INT16U   LAN91C111_RegRd         (CPU_INT16U    reg_bank,
                                      CPU_INT16U    reg_offset);
#endif

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                     DEFINED IN NIC'S  net_nic.c
*********************************************************************************************************
*/
                                                                /* ---------------- LAN91C111 REG FNCTS --------------- */
CPU_INT16U  LAN91C111_RegRd          (CPU_INT16U    reg_bank,
                                      CPU_INT16U    reg_offset);

void        LAN91C111_RegWr          (CPU_INT16U    reg_bank,
                                      CPU_INT16U    reg_offset,
                                      CPU_INT16U    val);

                                                                /* -------------- LAN91C111 PHY REG FNCTS ------------- */

CPU_INT16U  LAN91C111_PhyRegRd       (CPU_INT08U    phy_reg);

void        LAN91C111_PhyRegWr       (CPU_INT08U    phy_reg,
                                      CPU_INT16U    val);

/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                      DEFINED IN OS'S  net_os.c
*********************************************************************************************************
*/

void         NetOS_NIC_Init          (NET_ERR      *perr);      /* Create Drv objs & start Drv tasks.                   */


void         NetOS_NIC_TxRdyWait     (NET_ERR      *perr);      /* Wait on drv tx rdy signal from NIC.                  */

void         NetOS_NIC_TxRdySignal   (void);                    /* Post    drv tx rdy signal from NIC.                  */


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                    DEFINED IN PRODUCT'S  net_bsp.c
*********************************************************************************************************
*/

                                                                /* See this 'net_nic.h  Notes #3b & #3c'.               */
#if (NET_NIC_CFG_RD_WR_SEL == NET_NIC_RD_WR_SEL_FNCT)
CPU_INT16U   NetNIC_Rd_16            (CPU_INT16U    reg_offset);

void         NetNIC_Wr_16            (CPU_INT16U    reg_offset,
                                      CPU_INT16U    val);
#endif



void         LAN91C111_PwrCtrl       (CPU_BOOLEAN   pwr);       /* Implement LAN91C111 pwr ctrl (if necessary) :        */
                                                                /*   DEF_ON    Set LAN91C111 'pwr' ON.                  */
                                                                /*   DEF_OFF   Set LAN91C111 'pwr' OFF.                 */

void         LAN91C111_DlyReset      (void);                    /* Implement 50 ms dly (see SMSC AN9.6, Section 4.7.1). */
                                                                /*   MAY  use OS dly to preempt or tmr loop.            */
             
void         LAN91C111_DlyAutoNegAck (void);                    /* Implement 1.5 s dly (see SMSC AN9.6, Section 4.7.1). */
                                                                /*   MUST use OS dly to preempt.                        */

void         LAN91C111_PhyFrameClkDly(void);                    /* Implement ???? dly; #### remove if unnecessary.      */
                                                                /*   MAY  use OS dly to preempt or tmr loop.            */

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                  DEFINED IN PRODUCT's  net_bsp_a.s
*********************************************************************************************************
*/

void         NetNIC_ISR              (void);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                    DEFINED IN PRODUCT'S  net_bsp.c
*
* Note(s) : (1) See this 'net_nic.c  LAN91C111_Init()      Note #5'.
*
*           (2) See this 'net_nic.c  NetNIC_ISR_Handler()  Note #3'.
*********************************************************************************************************
*/

#if (NET_NIC_CFG_INT_CTRL_EN == DEF_ENABLED)
void         NetNIC_IntInit          (void);                    /* Init int ctrl'r        [see Note #1].                */

void         NetNIC_IntClr           (void);                    /* Clr  int ctrl'r src(s) [see Note #2].                */
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_NIC_CFG_INT_CTRL_EN
#error  "NET_NIC_CFG_INT_CTRL_EN           not #define'd in 'net_cfg.h'"
#error  "                            [MUST be  DEF_DISABLED]           "
#error  "                            [     ||  DEF_ENABLED ]           "
#elif  ((NET_NIC_CFG_INT_CTRL_EN != DEF_DISABLED) && \
        (NET_NIC_CFG_INT_CTRL_EN != DEF_ENABLED ))
#error  "NET_NIC_CFG_INT_CTRL_EN     illegally #define'd in 'net_cfg.h'"
#error  "                            [MUST be  DEF_DISABLED]           "
#error  "                            [     ||  DEF_ENABLED ]           "
#endif



#ifndef  NET_NIC_CFG_RD_WR_SEL
#error  "NET_NIC_CFG_RD_WR_SEL             not #define'd in 'net_cfg.h'"
#error  "                            [MUST be  NET_NIC_RD_WR_SEL_FNCT ]"
#error  "                            [     ||  NET_NIC_RD_WR_SEL_MACRO]"
#elif  ((NET_NIC_CFG_RD_WR_SEL != NET_NIC_RD_WR_SEL_FNCT ) && \
        (NET_NIC_CFG_RD_WR_SEL != NET_NIC_RD_WR_SEL_MACRO))
#error  "NET_NIC_CFG_RD_WR_SEL       illegally #define'd in 'net_cfg.h'"
#error  "                            [MUST be  NET_NIC_RD_WR_SEL_FNCT ]"
#error  "                            [     ||  NET_NIC_RD_WR_SEL_MACRO]"
#endif



#ifndef  LAN91C111_CFG_MAC_ADDR_SEL
#error  "LAN91C111_CFG_MAC_ADDR_SEL        not #define'd in 'net_cfg.h'      "
#error  "                            [MUST be  LAN91C111_MAC_ADDR_SEL_CFG   ]"
#error  "                            [     ||  LAN91C111_MAC_ADDR_SEL_EEPROM]"
#elif  ((LAN91C111_CFG_MAC_ADDR_SEL != LAN91C111_MAC_ADDR_SEL_CFG   ) && \
        (LAN91C111_CFG_MAC_ADDR_SEL != LAN91C111_MAC_ADDR_SEL_EEPROM))
#error  "LAN91C111_CFG_MAC_ADDR_SEL  illegally #define'd in 'net_cfg.h'      "
#error  "                            [MUST be  LAN91C111_MAC_ADDR_SEL_CFG   ]"
#error  "                            [     ||  LAN91C111_MAC_ADDR_SEL_EEPROM]"
#endif

