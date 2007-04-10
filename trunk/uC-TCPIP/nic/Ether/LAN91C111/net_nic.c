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
* Filename      : net_nic.c
* Version       : V1.88
* Programmer(s) : ITJ
*                 JJL
*                 SR
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
*                         (see 'LAN91C111 WORD ORDER - TO - CPU WORD ORDER').
*
*                 (4) #### Link detection & recovery state machine NOT yet implemented.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_NIC_MODULE
#include  <net.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          LAN91C111 DEFINES
*********************************************************************************************************
*/

#define  LAN91C111_INIT_AUTO_NEG_RETRIES                   5
#define  LAN91C111_TX_ALLOC_INIT_RETRIES                   5


/*
*********************************************************************************************************
*                             LAN91C111 WORD ORDER - TO - CPU WORD ORDER
*
* Note(s) : (1) Since LAN91C111 is implemented with a Little-endian architecture    (see this 'net_nic.h
*               Note #3'), & this driver accesses ALL words as 16-bit Little-endian (see this 'net_nic.h
*               Note #3a'), then data access MUST be octet-swapped for Big-engian processors & MUST NOT 
*               be swapped for Little-endian processors (see this 'net_nic.h  Note #3d').
*********************************************************************************************************
*/

#if (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)

#define  LAN91C111_NIC_TO_HOST_16(val)                   ((((CPU_INT16U)(val) & 0xFF00) >>  8) | \
                                                          (((CPU_INT16U)(val) & 0x00FF) <<  8))

#else

#define  LAN91C111_NIC_TO_HOST_16(val)                                  (val)

#endif


#define  LAN91C111_HOST_TO_NIC_16(val)          LAN91C111_NIC_TO_HOST_16(val)


/*$PAGE*/
/*
*********************************************************************************************************
*                                         LAN91C111 REGISTERS
*
* Note(s) : (1) See SMSC LAN91C111, Sections 8.3 - 8.26 for LAN91C111 register summary.
*
*           (2) LAN91C111 registers accessed & modified by appropriately configuring the LAN91C111 Bank
*               Select Register & by addressing the desired register address offset(s).
*
*               (a) A register can ONLY be accessed when that register's bank select value is configured/
*                   written into the Bank Select Register (see SMSC LAN91C111, Section 8.4) :
*
*                            1 1 1 1 1 1
*                            5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
*                           ---------------------------------
*                           |x x x x x x x x x x x x x| BNK |
*                           ---------------------------------
*                            LAN91C111 Bank Select Register
*
*                       where
*
*                               BNK         Indicates register's bank :
*                                               '000' - Bank 0
*                                               '001' - Bank 1
*                                               '010' - Bank 2
*                                               '011' - Bank 3
*
*               (b) After the Bank Select Register is configured with the appropriate register bank value,
*                   a register is accessed by addressing the appropriate offset from the base register address
*                   (see SMSC LAN91C111, Section 8.3) :
*
*                            1 1 1 1 1 1
*                            5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
*                           ---------------------------------
*                           |x x x x x x x x x x x x|OFFSET |
*                           ---------------------------------
*                                Register Address Offset
*
*                       where
*
*                               OFFSET      Indicates register's offset from the base register address :
*                                               '0000' - lowest         register offset
*                                               '1110' - highest 16-bit register offset
*                                               '1100' - highest 32-bit register offset
*
*               Mapping of register bank & offset values to physical registers is implemented by LAN91C111_RegRd()
*               & LAN91C111_RegWr() that call product-specific I/O functions NetNIC_Rd_16() & NetNIC_Wr_16().  These
*               functions perform the actual read & write to the LAN91C111 & MUST be developer-implemented in
*               
*                   \<Your Product Application>\net_bsp.c
*
*                       where
*                               <Your Product Application>    directory path for Your Product's Application
*
*           (3) The current register bank select is maintained using a virtual copy of the LAN91C111's current
*               Bank Select Register value.  Each time a register is accessed, its register bank is compared 
*               to the maintained, virtual copy of the current register bank.  If the banks are the same, then 
*               the register's bank is already configured as the current bank in the Bank Select Register & no 
*               bank select configuration is required.  If the banks are different, then the register's bank 
*               value MUST be configured into the Bank Select Register (see Note #2a).  This reduces the total 
*               number of Bank Select Register writes required.
*
*           (4) Registers referenced as offsets from a base register address (see Note #2b).  The base register
*               address MUST be correctly implemented in product-specific I/O functions NetNIC_Rd_16() & 
*               NetNIC_Wr_16().  See SMSC LAN91C111, Section 5 'Address' & Section 8.3; & SMSC AN9.6 3.3 for 
*               LAN91C111 Base Register address summary.
*
*           (5) To protect LAN91C111 register accesses during multi-thread operation, registers MUST be
*               accessed in critical sections &/or mutual exclusion.
*
*               (a) For single-register, single-access; critical sections are implemented in LAN91C111_RegRd()
*                   & LAN91C111_RegWr().
*
*               (b) For read-modify-write register access OR multiple-register-access sequences; critical
*                   sections &/or mutual exclusion enclosing the entire register access/sequence is REQUIRED.
*********************************************************************************************************
*/
/*$PAGE*/
                                                                    /* ------------------- REG BANKS ------------------ */
#define  LAN91C111_REG_BANK_0                                 0x0000
#define  LAN91C111_REG_BANK_1                                 0x0001
#define  LAN91C111_REG_BANK_2                                 0x0002
#define  LAN91C111_REG_BANK_3                                 0x0003
#define  LAN91C111_REG_BANK_DFLT                LAN91C111_REG_BANK_0


                                                                    /* --------------------- REGS --------------------- */
#define  LAN91C111_REG_BANK_SEL_OFFSET                        0x000E

                                                                    /* ------------------ BANK 0 REGS ----------------- */
#define  LAN91C111_REG_TCR_BANK                 LAN91C111_REG_BANK_0
#define  LAN91C111_REG_TCR_OFFSET                             0x0000

#define  LAN91C111_REG_EPH_BANK                 LAN91C111_REG_BANK_0
#define  LAN91C111_REG_EPH_OFFSET                             0x0002

#define  LAN91C111_REG_RCR_BANK                 LAN91C111_REG_BANK_0
#define  LAN91C111_REG_RCR_OFFSET                             0x0004

#define  LAN91C111_REG_CTR_BANK                 LAN91C111_REG_BANK_0
#define  LAN91C111_REG_CTR_OFFSET                             0x0006

#define  LAN91C111_REG_MIR_BANK                 LAN91C111_REG_BANK_0
#define  LAN91C111_REG_MIR_OFFSET                             0x0008

#define  LAN91C111_REG_RPCR_BANK                LAN91C111_REG_BANK_0
#define  LAN91C111_REG_RPCR_OFFSET                            0x000A

#define  LAN91C111_REG_RESERVED_0_BANK          LAN91C111_REG_BANK_0
#define  LAN91C111_REG_RESERVED_0_OFFSET                      0x000C

#define  LAN91C111_REG_BANK_SEL_0_BANK          LAN91C111_REG_BANK_0
#define  LAN91C111_REG_BANK_SEL_0_OFFSET        LAN91C111_REG_BANK_SEL_OFFSET


                                                                    /* ------------------ BANK 1 REGS ----------------- */
#define  LAN91C111_REG_CFG_BANK                 LAN91C111_REG_BANK_1
#define  LAN91C111_REG_CFG_OFFSET                             0x0000

#define  LAN91C111_REG_BASE_BANK                LAN91C111_REG_BANK_1
#define  LAN91C111_REG_BASE_OFFSET                            0x0002

#define  LAN91C111_REG_MAC_0_BANK               LAN91C111_REG_BANK_1
#define  LAN91C111_REG_MAC_0_OFFSET                           0x0004

#define  LAN91C111_REG_MAC_2_BANK               LAN91C111_REG_BANK_1
#define  LAN91C111_REG_MAC_2_OFFSET                           0x0006

#define  LAN91C111_REG_MAC_4_BANK               LAN91C111_REG_BANK_1
#define  LAN91C111_REG_MAC_4_OFFSET                           0x0008

#define  LAN91C111_REG_GENERAL_BANK             LAN91C111_REG_BANK_1
#define  LAN91C111_REG_GENERAL_OFFSET                         0x000A

#define  LAN91C111_REG_CTRL_BANK                LAN91C111_REG_BANK_1
#define  LAN91C111_REG_CTRL_OFFSET                            0x000C

#define  LAN91C111_REG_BANK_SEL_1_BANK          LAN91C111_REG_BANK_1
#define  LAN91C111_REG_BANK_SEL_1_OFFSET        LAN91C111_REG_BANK_SEL_OFFSET


                                                                    /* ------------------ BANK 2 REGS ----------------- */
#define  LAN91C111_REG_MMU_BANK                 LAN91C111_REG_BANK_2
#define  LAN91C111_REG_MMU_OFFSET                             0x0000

#define  LAN91C111_REG_PNR_BANK                 LAN91C111_REG_BANK_2
#define  LAN91C111_REG_PNR_OFFSET                             0x0002

#define  LAN91C111_REG_FIFO_BANK                LAN91C111_REG_BANK_2
#define  LAN91C111_REG_FIFO_OFFSET                            0x0004

#define  LAN91C111_REG_PTR_BANK                 LAN91C111_REG_BANK_2
#define  LAN91C111_REG_PTR_OFFSET                             0x0006

#define  LAN91C111_REG_DATA_0_BANK              LAN91C111_REG_BANK_2
#define  LAN91C111_REG_DATA_0_OFFSET                          0x0008

#define  LAN91C111_REG_DATA_2_BANK              LAN91C111_REG_BANK_2
#define  LAN91C111_REG_DATA_2_OFFSET                          0x000A

#define  LAN91C111_REG_INT_BANK                 LAN91C111_REG_BANK_2
#define  LAN91C111_REG_INT_OFFSET                             0x000C

#define  LAN91C111_REG_BANK_SEL_2_BANK          LAN91C111_REG_BANK_2
#define  LAN91C111_REG_BANK_SEL_2_OFFSET        LAN91C111_REG_BANK_SEL_OFFSET


                                                                    /* ------------------ BANK 3 REGS ----------------- */
#define  LAN91C111_REG_MULTICAST_0_BANK         LAN91C111_REG_BANK_3
#define  LAN91C111_REG_MULTICAST_0_OFFSET                     0x0000

#define  LAN91C111_REG_MULTICAST_2_BANK         LAN91C111_REG_BANK_3
#define  LAN91C111_REG_MULTICAST_2_OFFSET                     0x0002

#define  LAN91C111_REG_MULTICAST_4_BANK         LAN91C111_REG_BANK_3
#define  LAN91C111_REG_MULTICAST_4_OFFSET                     0x0004

#define  LAN91C111_REG_MULTICAST_6_BANK         LAN91C111_REG_BANK_3
#define  LAN91C111_REG_MULTICAST_6_OFFSET                     0x0006

#define  LAN91C111_REG_MGMT_BANK                LAN91C111_REG_BANK_3
#define  LAN91C111_REG_MGMT_OFFSET                            0x0008

#define  LAN91C111_REG_REV_BANK                 LAN91C111_REG_BANK_3
#define  LAN91C111_REG_REV_OFFSET                             0x000A

#define  LAN91C111_REG_ERCV_BANK                LAN91C111_REG_BANK_3
#define  LAN91C111_REG_ERCV_OFFSET                            0x000C

#define  LAN91C111_REG_BANK_SEL_3_BANK          LAN91C111_REG_BANK_3
#define  LAN91C111_REG_BANK_SEL_3_OFFSET        LAN91C111_REG_BANK_SEL_OFFSET


/*$PAGE*/
/*
*********************************************************************************************************
*                                       LAN91C111 REGISTER BITS
*********************************************************************************************************
*/

                                                                /* ------- TRANSMIT CONTROL REGISTER (TCR) BITS ------- */
#define  LAN91C111_REG_TCR_SW_DPLX_HALF           DEF_BIT_NONE  /* 0 : Half Dplx (dflt).                                */
#define  LAN91C111_REG_TCR_SW_DPLX_FULL           DEF_BIT_15    /* 1 : Full Dplx.                                       */

#define  LAN91C111_REG_TCR_EPH_LPBK_DIS           DEF_BIT_NONE  /* 0 : EPH Lpbk DIS (dflt).                             */
#define  LAN91C111_REG_TCR_EPH_LPBK_EN            DEF_BIT_13    /* 1 : EPH Lpbk EN.                                     */

#define  LAN91C111_REG_TCR_SQET_STOP_DIS          DEF_BIT_NONE  /* 0 : Ignores SQET (dflt).                             */
#define  LAN91C111_REG_TCR_SQET_STOP_EN           DEF_BIT_12    /* 1 : Traps   SQET.                                    */

#define  LAN91C111_REG_TCR_RX_LPBK_DIS            DEF_BIT_NONE  /* 0 : Rx Lpbk DIS (dflt).                              */
#define  LAN91C111_REG_TCR_RX_LPBK_EN             DEF_BIT_11    /* 1 : Rx Lpbk EN.                                      */

#define  LAN91C111_REG_TCR_MON_CARR_DIS           DEF_BIT_NONE  /* 0 : Ignore  Carrier (dflt).                          */
#define  LAN91C111_REG_TCR_MON_CARR_EN            DEF_BIT_10    /* 1 : Monitor Carrier.                                 */

#define  LAN91C111_REG_TCR_TX_CRC_EN              DEF_BIT_NONE  /* 0 : Tx CRC     appended (dflt).                      */
#define  LAN91C111_REG_TCR_TX_CRC_DIS             DEF_BIT_08    /* 1 : Tx CRC NOT appended.                             */

#define  LAN91C111_REG_TCR_TX_PAD_DIS             DEF_BIT_NONE  /* 0 : Tx frames NOT padded (dflt).                     */
#define  LAN91C111_REG_TCR_TX_PAD_EN              DEF_BIT_07    /* 1 : Tx frames     padded.                            */

#define  LAN91C111_REG_TCR_TX_COLL_NOT_FORCED     DEF_BIT_NONE  /* 0 : Tx Collisions NOT forced (dflt).                 */
#define  LAN91C111_REG_TCR_TX_COLL_FORCED         DEF_BIT_02    /* 1 : Tx Collisions     forced.                        */

#define  LAN91C111_REG_TCR_LPBK_OUT_LO            DEF_BIT_NONE  /* 0 : Force Lpbk output pin LO (dflt).                 */
#define  LAN91C111_REG_TCR_LPBK_OUT_HI            DEF_BIT_01    /* 1 : Force Lpbk output pin HI.                        */

#define  LAN91C111_REG_TCR_TX_DIS                 DEF_BIT_NONE  /* 0 : Tx DIS (dflt).                                   */
#define  LAN91C111_REG_TCR_TX_EN                  DEF_BIT_00    /* 1 : Tx EN.                                           */


                                                                /* ------- RECEIVE CONTROL REGISTER (RCR) BITS -------- */
#define  LAN91C111_REG_RCR_SW_RESET               DEF_BIT_15    /* 1 : Initiate SW reset.                               */

#define  LAN91C111_REG_RCR_RX_FLTR_CARR_DIS       DEF_BIT_NONE  /* 0 : Rx frames immediately (dflt).                    */
#define  LAN91C111_REG_RCR_RX_FLTR_CARR_EN        DEF_BIT_14    /* 1 : Rx frames after 12-bit carrier sense.            */

#define  LAN91C111_REG_RCR_RX_COLL_EN             DEF_BIT_NONE  /* 0 : Do NOT abort Rx Collision frames (dflt).         */
#define  LAN91C111_REG_RCR_RX_COLL_DIS            DEF_BIT_13    /* 1 :        Abort Rx Collision frames.                */

#define  LAN91C111_REG_RCR_RX_CRC_EN              DEF_BIT_NONE  /* 0 : Rx CRC     appended (dflt).                      */
#define  LAN91C111_REG_RCR_RX_CRC_DIS             DEF_BIT_09    /* 1 : Rc CRC NOT appended.                             */

#define  LAN91C111_REG_RCR_RX_DIS                 DEF_BIT_NONE  /* 0 : Rx DIS (dflt).                                   */
#define  LAN91C111_REG_RCR_RX_EN                  DEF_BIT_08    /* 1 : Rx EN.                                           */

#define  LAN91C111_REG_RCR_RX_MULTICAST_TBL       DEF_BIT_NONE  /* 0 : Rx Tbl Multicast addrs ONLY (dflt).              */
#define  LAN91C111_REG_RCR_RX_MULTICAST_ALL       DEF_BIT_02    /* 1 : Rx ALL Multicast addrs.                          */

#define  LAN91C111_REG_RCR_RX_PROMISC_DIS         DEF_BIT_NONE  /* 0 : Rx dest frames ONLY (dflt).                      */
#define  LAN91C111_REG_RCR_RX_PROMISC_EN          DEF_BIT_01    /* 1 : Rx ALL  frames.                                  */


                                                                /* --- RECEIVE/PHYSICAL CONTROL REGISTER (RPCR) BITS --- */
#define  LAN91C111_REG_RPCR_SPD_10                DEF_BIT_NONE  /* 0 :  10 Mbps (dlft).                                 */
#define  LAN91C111_REG_RPCR_SPD_100               DEF_BIT_13    /* 1 : 100 Mbps.                                        */

#define  LAN91C111_REG_RPCR_DPLX_HALF             DEF_BIT_NONE  /* 0 : Half Dplx (dflt).                                */
#define  LAN91C111_REG_RPCR_DPLX_FULL             DEF_BIT_12    /* 1 : Full Dplx.                                       */

#define  LAN91C111_REG_RPCR_AUTO_NEG_DIS          DEF_BIT_NONE  /* 0 : Auto Negotiation DIS (dflt).                     */
#define  LAN91C111_REG_RPCR_AUTO_NEG_EN           DEF_BIT_11    /* 1 : Auto Negotiation EN.                             */

                                                                /* LED SELECT SIGNAL - LED A                            */
#define  LAN91C111_REG_RPCR_LED_A_LINK            0x0000        /* 0 : Logical OR of 100 Mbps or 10 Mbps link detected  */
#define  LAN91C111_REG_RPCR_LED_A_LINK_10         0x0040        /* 2 : 10 Mbps link detected                            */
#define  LAN91C111_REG_RPCR_LED_A_FULL_DUPLEX     0x0060        /* 3 : Full Duplex Mode Enabled                         */
#define  LAN91C111_REG_RPCR_LED_A_RX_TX           0x0080        /* 4 : Tx or Rx packet occurred                         */
#define  LAN91C111_REG_RPCR_LED_A_LINK_100        0x00A0        /* 5 : 100 Mbps link detected                           */
#define  LAN91C111_REG_RPCR_LED_A_RX              0x00C0        /* 6 : Rx packet occurred                               */
#define  LAN91C111_REG_RPCR_LED_A_TX              0x00E0        /* 7 : Tx packet occurred                               */

                                                                /* LED SELECT SIGNAL - LED B                            */
#define  LAN91C111_REG_RPCR_LED_B_LINK            0x0000        /* 0 : Logical OR of 100 Mbps or 10 Mbps link detected  */
#define  LAN91C111_REG_RPCR_LED_B_LINK_10         0x0008        /* 2 : 10 Mbps link detected                            */
#define  LAN91C111_REG_RPCR_LED_B_FULL_DUPLEX     0x000C        /* 3 : Full Duplex Mode Enabled                         */
#define  LAN91C111_REG_RPCR_LED_B_RX_TX           0x0010        /* 4 : Tx or Rx packet occurred                         */
#define  LAN91C111_REG_RPCR_LED_B_LINK_100        0x0014        /* 5 : 100 Mbps link detected                           */
#define  LAN91C111_REG_RPCR_LED_B_RX              0x0018        /* 6 : Rx packet occurred                               */
#define  LAN91C111_REG_RPCR_LED_B_TX              0x001C        /* 7 : Tx packet occurred                               */


                                                                /* -------- CONFIGURATION REGISTER (CFG) BITS --------- */
#define  LAN91C111_REG_CFG_EPH_LO_PWR_EN          DEF_BIT_NONE  /* 0 : EPH Lo Pwr Mode EN (dflt).                       */
#define  LAN91C111_REG_CFG_EPH_LO_PWR_DIS         DEF_BIT_15    /* 1 : EPH Lo Pwr Mode DIS.                             */

#define  LAN91C111_REG_CFG_WAIT_STATES_EN         DEF_BIT_NONE  /* 0 : Wait states EN (dflt).                           */
#define  LAN91C111_REG_CFG_WAIT_STATES_DIS        DEF_BIT_12    /* 1 : Wait states DIS.                                 */

#define  LAN91C111_REG_CFG_PHY_INT                DEF_BIT_NONE  /* 0 : Internal PHY (dlft).                             */
#define  LAN91C111_REG_CFG_PHY_EXT                DEF_BIT_09    /* 1 : External PHY.                                    */


                                                                /* ----------- CONTROL REGISTER (CTRL) BITS ----------- */
#define  LAN91C111_REG_CTRL_RX_CRC_ERR_DIS        DEF_BIT_NONE  /* 0 :        Abort Rx CRC err frames (dflt).           */
#define  LAN91C111_REG_CTRL_RX_CRC_ERR_EN         DEF_BIT_14    /* 1 : Do NOT abort Rx CRC err frames.                  */

#define  LAN91C111_REG_CTRL_TX_REL_MAN            DEF_BIT_NONE  /* 0 : Manual rel Tx pkts (dflt).                       */
#define  LAN91C111_REG_CTRL_TX_REL_AUTO           DEF_BIT_11    /* 1 : Auto   rel Tx pkts.                              */

#define  LAN91C111_REG_CTRL_LINK_ERR_DIS          DEF_BIT_NONE  /* 0 : Link Err Int DIS (dflt).                         */
#define  LAN91C111_REG_CTRL_LINK_ERR_EN           DEF_BIT_07    /* 1 : Link Err Int EN.                                 */

#define  LAN91C111_REG_CTRL_CTR_ERR_DIS           DEF_BIT_NONE  /* 0 : Ctr  Ovf Int DIS (dflt).                         */
#define  LAN91C111_REG_CTRL_CTR_ERR_EN            DEF_BIT_06    /* 1 : Ctr  Ovf Int EN.                                 */

#define  LAN91C111_REG_CTRL_TX_ERR_DIS            DEF_BIT_NONE  /* 0 : Tx   Err Int DIS (dflt).                         */
#define  LAN91C111_REG_CTRL_TX_ERR_EN             DEF_BIT_05    /* 1 : Tx   Err Int EN.                                 */

#define  LAN91C111_REG_CTRL_EEPROM_DFLT           DEF_BIT_NONE  /* 0 : CFG/BASE/MAC Regs (dflt).                        */
#define  LAN91C111_REG_CTRL_EEPROM_GENERAL        DEF_BIT_02    /* 1 : GEN Reg ONLY.                                    */



/*$PAGE*/
                                                                /* ---- MEMORY MANAGEMENT UNIT REGISTER (MMU) BITS ---- */
#define  LAN91C111_REG_MMU_CMD_MASK                   0x00E0
#define  LAN91C111_REG_MMU_CMD_NOP                    0x0000    /* No op.                                               */
#define  LAN91C111_REG_MMU_CMD_TX_ALLOC               0x0020    /* Req tx alloc.                                        */
#define  LAN91C111_REG_MMU_CMD_RESET                  0x0040    /* Reset MMU.                                           */
#define  LAN91C111_REG_MMU_CMD_RX_REMOVE              0x0060    /* Remove       rx frame from Rx FIFO.                  */
#define  LAN91C111_REG_MMU_CMD_RX_REMOVE_REL          0x0080    /* Remove & rel rx frame from Rx FIFO.                  */
#define  LAN91C111_REG_MMU_CMD_REL_PKT                0x00A0    /* Rel pkt.                                             */
#define  LAN91C111_REG_MMU_CMD_TX_PKT                 0x00C0    /* Tx  pkt.                                             */
#define  LAN91C111_REG_MMU_CMD_TX_RESET               0x00E0    /* Reset Tx FIFOs.                                      */

#define  LAN91C111_REG_MMD_CMD_TX_PG_SIZE                256    /* Backwards-compatible with LAN91C96.                  */


                                                                /* -------- PACKET NUMBER REGISTER (PNR) BITS --------- */
#define  LAN91C111_REG_PNR_TX_PKT_NBR                 0x003F    /* Cur tx pkt nbr.                                      */

#define  LAN91C111_REG_PNR_TX_ALLOC_SHIFT                  8
#define  LAN91C111_REG_PNR_TX_ALLOC                   0x3F00
#define  LAN91C111_REG_PNR_TX_ALLOC_SUCCESS           0x0000    /* 0 : Tx alloc successful.                             */
#define  LAN91C111_REG_PNR_TX_ALLOC_FAIL          DEF_BIT_15    /* 1 : Tx alloc failed (dflt).                          */


                                                                /* --------- FIFO PORTS REGISTER (FIFO) BITS ---------- */
#define  LAN91C111_REG_FIFO_RX_NOT_EMPTY          DEF_BIT_NONE  /* 0 : Rx FIFO NOT empty.                               */
#define  LAN91C111_REG_FIFO_RX_EMPTY              DEF_BIT_15    /* 1 : Rx FIFO     empty (dflt).                        */
#define  LAN91C111_REG_FIFO_RX_PKT_NBR                0x3F00

#define  LAN91C111_REG_FIFO_TX_NOT_EMPTY          DEF_BIT_NONE  /* 0 : Tx FIFO NOT empty.                               */
#define  LAN91C111_REG_FIFO_TX_EMPTY              DEF_BIT_07    /* 1 : Tx FIFO     empty (dflt).                        */
#define  LAN91C111_REG_FIFO_TX_PKT_NBR                0x003F


                                                                /* ----------- POINTER REGISTER (PTR) BITS ------------ */
#define  LAN91C111_REG_PTR_FIFO_TX                DEF_BIT_NONE  /* 0 : Tx FIFO Pkt Ptr.                                 */
#define  LAN91C111_REG_PTR_FIFO_RX                DEF_BIT_15    /* 1 : Rx FIFO Pkt Ptr.                                 */

#define  LAN91C111_REG_PTR_INC_MAN                DEF_BIT_NONE  /* 0 : Manual ptr inc (dflt).                           */
#define  LAN91C111_REG_PTR_INC_AUTO               DEF_BIT_14    /* 1 : Auto   ptr inc.                                  */

#define  LAN91C111_REG_PTR_WR                     DEF_BIT_NONE  /* 0 : Wr.                                              */
#define  LAN91C111_REG_PTR_RD                     DEF_BIT_13    /* 1 : Rd.                                              */

#define  LAN91C111_REG_PTR_TX_UNDERRUN_DIS        DEF_BIT_NONE  /* 0 : Early Tx underrun detection DIS (dflt).          */
#define  LAN91C111_REG_PTR_TX_UNDERRUN_EN         DEF_BIT_12    /* 1 : Early Tx underrun detection EN.                  */

#define  LAN91C111_REG_PTR_WR_FIFO_EMPTY          DEF_BIT_NONE  /* 0 : Wr data FIFO     empty (dflt).                   */
#define  LAN91C111_REG_PTR_WR_FIFO_NOT_EMPTY      DEF_BIT_11    /* 1 : Wr data FIFO NOT empty.                          */

#define  LAN91C111_REG_PTR_PKT_IX                     0x07FF


/*$PAGE*/
                                                                /* ------- INTERRUPT STATUS REGISTER (INT) BITS ------- */
#define  LAN91C111_REG_INT_MASK_EN                    0xFF00    /* Int En's   mask.                                     */
#define  LAN91C111_REG_INT_MASK_STATUS                0x00FF    /* Int status mask.                                     */


#define  LAN91C111_REG_INT_MD                     DEF_BIT_07    /*     MD         int status.                           */
#define  LAN91C111_REG_INT_MD_ACK                 DEF_BIT_07    /*     MD         int ack.                              */
#define  LAN91C111_REG_INT_MD_DIS                 DEF_BIT_NONE  /* 0 : MD         int DIS (dflt).                       */
#define  LAN91C111_REG_INT_MD_EN                  DEF_BIT_15    /* 1 : MD         int EN.                               */

#define  LAN91C111_REG_INT_ERCV                   DEF_BIT_06    /*     ERCV       int status.                           */
#define  LAN91C111_REG_INT_ERCV_ACK               DEF_BIT_06    /*     ERCV       int ack.                              */
#define  LAN91C111_REG_INT_ERCV_DIS               DEF_BIT_NONE  /* 0 : ERCV       int DIS (dflt).                       */
#define  LAN91C111_REG_INT_ERCV_EN                DEF_BIT_14    /* 1 : ERCV       int EN.                               */

#define  LAN91C111_REG_INT_EPH                    DEF_BIT_05    /*     EPH        int status.                           */
#define  LAN91C111_REG_INT_ERH_DIS                DEF_BIT_NONE  /* 0 : EPH        int DIS (dflt).                       */
#define  LAN91C111_REG_INT_ERH_EN                 DEF_BIT_13    /* 1 : EPH        int EN.                               */

#define  LAN91C111_REG_INT_RX_OVERRUN             DEF_BIT_04    /*     Rx Overrun int status.                           */
#define  LAN91C111_REG_INT_RX_OVERRUN_ACK         DEF_BIT_04    /*     Rx Overrun int ack.                              */
#define  LAN91C111_REG_INT_RX_OVERRUN_DIS         DEF_BIT_NONE  /* 0 : Rx Overrun int DIS (dflt).                       */
#define  LAN91C111_REG_INT_RX_OVERRUN_EN          DEF_BIT_12    /* 1 : Rx Overrun int EN.                               */

#define  LAN91C111_REG_INT_TX_ALLOC               DEF_BIT_03    /*     Tx Alloc   int status.                           */
#define  LAN91C111_REG_INT_TX_ALLOC_DIS           DEF_BIT_NONE  /* 0 : Tx Alloc   int DIS (dflt).                       */
#define  LAN91C111_REG_INT_TX_ALLOC_EN            DEF_BIT_11    /* 1 : Tx Alloc   int EN.                               */

#define  LAN91C111_REG_INT_TX_EMPTY               DEF_BIT_02    /*     Tx Empty   int status.                           */
#define  LAN91C111_REG_INT_TX_EMPTY_ACK           DEF_BIT_02    /*     Tx Empty   int ack.                              */
#define  LAN91C111_REG_INT_TX_EMPTY_DIS           DEF_BIT_NONE  /* 0 : Tx Empty   int DIS (dflt).                       */
#define  LAN91C111_REG_INT_TX_EMPTY_EN            DEF_BIT_10    /* 1 : Tx Empty   int EN.                               */

#define  LAN91C111_REG_INT_TX                     DEF_BIT_01    /*     Tx         int status.                           */
#define  LAN91C111_REG_INT_TX_ACK                 DEF_BIT_01    /*     Tx         int ack.                              */
#define  LAN91C111_REG_INT_TX_DIS                 DEF_BIT_NONE  /* 0 : Tx         int DIS (dflt).                       */
#define  LAN91C111_REG_INT_TX_EN                  DEF_BIT_09    /* 1 : Tx         int EN.                               */

#define  LAN91C111_REG_INT_RX                     DEF_BIT_00    /*     Rx         int status.                           */
#define  LAN91C111_REG_INT_RX_DIS                 DEF_BIT_NONE  /* 0 : Rx         int DIS (dflt).                       */
#define  LAN91C111_REG_INT_RX_EN                  DEF_BIT_08    /* 1 : Rx         int EN.                               */


                                                                /* ---- MANAGEMENT INTERFACE REGISTER (MGMT) BITS ----- */
#define  LAN91C111_REG_MGMT_MD_PHY_MASK               0x000F

#define  LAN91C111_REG_MGMT_MD_OUT_DIS            DEF_BIT_NONE  /* 0 : MD output pin tri-stated (dflt).                 */
#define  LAN91C111_REG_MGMT_MD_OUT_EN             DEF_BIT_03    /* 1 : MD output pin follows MD output.                 */

#define  LAN91C111_REG_MGMT_MD_CLK_LO             DEF_BIT_NONE  /* 0 : MD clk    LO.                                    */
#define  LAN91C111_REG_MGMT_MD_CLK_HI             DEF_BIT_02    /* 1 : MD clk    HI.                                    */

#define  LAN91C111_REG_MGMT_MD_IN_LO              DEF_BIT_NONE  /* 0 : MD input  LO.                                    */
#define  LAN91C111_REG_MGMT_MD_IN_HI              DEF_BIT_01    /* 1 : MD input  HI.                                    */

#define  LAN91C111_REG_MGMT_MD_OUT_LO             DEF_BIT_NONE  /* 0 : MD output LO.                                    */
#define  LAN91C111_REG_MGMT_MD_OUT_HI             DEF_BIT_00    /* 1 : MD output HI.                                    */


/*$PAGE*/
/*
*********************************************************************************************************
*                                 LAN91C111 PHYSICAL (PHY) REGISTERS
*
* Note(s) : (1) See SMSC LAN91C111, Sections 9 - 9.10 for LAN91C111 Physical register summary.
*********************************************************************************************************
*/

                                                                /* --------------------- PHY REGS --------------------- */
#define  LAN91C111_PHY_REG_CTRL                            0
#define  LAN91C111_PHY_REG_STATUS                          1
#define  LAN91C111_PHY_REG_ID_1                            2
#define  LAN91C111_PHY_REG_ID_2                            3
#define  LAN91C111_PHY_REG_AUTO_NEG_AD                     4
#define  LAN91C111_PHY_REG_AUTO_NEG_REM                    5
#define  LAN91C111_PHY_REG_CFG_1                          16
#define  LAN91C111_PHY_REG_CFG_2                          17
#define  LAN91C111_PHY_REG_STATUS_OUT                     18
#define  LAN91C111_PHY_REG_INT                            19


#define  LAN91C111_PHY_REG_RD                              0
#define  LAN91C111_PHY_REG_WR                              1


#define  LAN91C111_PHY_FRAME_IDLE_BIT_NBR                 32
#define  LAN91C111_PHY_FRAME_PHY_DEV_ADDR_BIT_NBR          5
#define  LAN91C111_PHY_FRAME_PHY_REG_BIT_NBR               5
#define  LAN91C111_PHY_FRAME_DATA_BIT_NBR                 16


/*
*********************************************************************************************************
*                               LAN91C111 PHYSICAL (PHY) REGISTER BITS
*********************************************************************************************************
*/

                                                                /* -- PHYSICAL CONTROL REGISTER (PHY CTRL) BITS ------- */
#define  LAN91C111_PHY_REG_CTRL_RESET             DEF_BIT_15    /* 1 : Initiate PHY reset.                              */

#define  LAN91C111_PHY_REG_CTRL_LPBK_DIS          DEF_BIT_NONE  /* 0 : PHY Lpbk EN (dflt).                              */
#define  LAN91C111_PHY_REG_CTRL_LPBK_EN           DEF_BIT_14    /* 1 : PHY Lpbk DIS.                                    */

#define  LAN91C111_PHY_REG_CTRL_SPD_10            DEF_BIT_NONE  /* 0 :  10 Mbps.                                        */
#define  LAN91C111_PHY_REG_CTRL_SPD_100           DEF_BIT_13    /* 1 : 100 Mbps (dflt).                                 */

#define  LAN91C111_PHY_REG_CTRL_NEG_MAN           DEF_BIT_NONE  /* 0 : Auto Negotiation DIS.                            */
#define  LAN91C111_PHY_REG_CTRL_NEG_AUTO          DEF_BIT_12    /* 1 : Auto Negotiation EN (dflt).                      */

#define  LAN91C111_PHY_REG_CTRL_PWR_ON            DEF_BIT_NONE  /* 0 : PHY ON (dflt).                                   */
#define  LAN91C111_PHY_REG_CTRL_PWR_OFF           DEF_BIT_11    /* 1 : PHY OFF.                                         */

#define  LAN91C111_PHY_REG_CTRL_MII_EN            DEF_BIT_NONE  /* 0 : Internal PHY EN.                                 */
#define  LAN91C111_PHY_REG_CTRL_MII_DIS           DEF_BIT_10    /* 1 : Internal PHY DIS/Isolated (dflt).                */

#define  LAN91C111_PHY_REG_CTRL_NEG_DIS           DEF_BIT_NONE  /* 0 : Indicates Auto Negotiation DIS (dflt).           */
#define  LAN91C111_PHY_REG_CTRL_NEG_AUTO_RESET    DEF_BIT_09    /* 1 : Initiate  Auto Negotiation Reset.                */

#define  LAN91C111_PHY_REG_CTRL_DPLX_HALF         DEF_BIT_NONE  /* 0 : Half Dplx (dflt).                                */
#define  LAN91C111_PHY_REG_CTRL_DPLX_FULL         DEF_BIT_08    /* 1 : Full Dplx.                                       */

#define  LAN91C111_PHY_REG_CTRL_COLL_DIS          DEF_BIT_NONE  /* 0 : Collision Test DIS (dflt).                       */
#define  LAN91C111_PHY_REG_CTRL_COLL_EN           DEF_BIT_07    /* 1 : Collision Test EN.                               */



                                                                /* ---- PHYSICAL STATUS REGISTER (PHY STATUS) BITS ---- */
#define  LAN91C111_PHY_REG_STATUS_T4              DEF_BIT_15    /* 1 : 100 Base-T4           PHY.                       */
                                                               
#define  LAN91C111_PHY_REG_STATUS_TXF             DEF_BIT_14    /* 1 : 100 Base-X  full dplx PHY.                       */

#define  LAN91C111_PHY_REG_STATUS_TXH             DEF_BIT_13    /* 1 : 100 Base-X  half dplx PHY.                       */

#define  LAN91C111_PHY_REG_STATUS_TF              DEF_BIT_12    /* 1 :  10 Base    full dplx PHY.                       */

#define  LAN91C111_PHY_REG_STATUS_TH              DEF_BIT_11    /* 1 :  10 Base    half dplx PHY.                       */

#define  LAN91C111_PHY_REG_STATUS_RX_PREAMBLE     DEF_BIT_NONE  /* 0 : Rx Preamble     required.                        */
#define  LAN91C111_PHY_REG_STATUS_NO_PREAMBLE     DEF_BIT_06    /* 1 : Rx Preamble NOT required.                        */

#define  LAN91C111_PHY_REG_STATUS_NEG_FAIL        DEF_BIT_NONE  /* 0 : Auto Negotiation NOT complete.                   */
#define  LAN91C111_PHY_REG_STATUS_NEG_ACK         DEF_BIT_05    /* 1 : Auto Negotiation     complete/ack.               */

#define  LAN91C111_PHY_REG_STATUS_REM_OK          DEF_BIT_NONE  /* 0 : NO rem flt.                                      */
#define  LAN91C111_PHY_REG_STATUS_REM_FLT         DEF_BIT_04    /* 1 :    Rem flt.                                      */

#define  LAN91C111_PHY_REG_STATUS_NEG_MAN         DEF_BIT_NONE  /* 0 : Auto Negotiation NOT possible.                   */
#define  LAN91C111_PHY_REG_STATUS_NEG_AUTO        DEF_BIT_03    /* 1 : Auto Negotiation     possible.                   */

#define  LAN91C111_PHY_REG_STATUS_LINK_FAIL       DEF_BIT_NONE  /* 0 : Invalid/NO link.                                 */
#define  LAN91C111_PHY_REG_STATUS_LINK_OK         DEF_BIT_02    /* 1 :   Valid    link.                                 */

#define  LAN91C111_PHY_REG_STATUS_NO_JABBER       DEF_BIT_NONE  /* 0 : 10 Mbps jabber NOT detected.                     */
#define  LAN91C111_PHY_REG_STATUS_JABBER          DEF_BIT_01    /* 1 : 10 Mbps jabber     detected.                     */

#define  LAN91C111_PHY_REG_STATUS_REG_NO_EXT      DEF_BIT_NONE  /* 0 : NO extended regs.                                */
#define  LAN91C111_PHY_REG_STATUS_REG_EXT         DEF_BIT_00    /* 1 :    Extended regs.                                */


/*$PAGE*/
/*
*********************************************************************************************************
*                                       LAN91C111 PACKET FRAMES
*
* Note(s) : (1) See SMSC LAN91C111, Section 8.1 for LAN91C111 packet frame summary :
*
*                          Packet
*                          Offset
*                     ---          |-----------------------------------|
*                      ^      0    |            STATUS WORD            |
*                      |           |-----------------------------------|
*                      |      2    | RESERVED |      PACKET SIZE       |
*                      |           |-----------------------------------|
*                      |      4    |                                   |
*                                  |                                   |
*                    2048          |                                   |
*                   octets         /                                   /
*                   maximum                     PACKET DATA             
*                                  /                                   /
*                      |           |                                   |
*                      |           |                                   |
*                      |           |                                   |
*                      |           |-----------------------------------|
*                      v           |     CONTROL     | LAST DATA OCTET |
*                     ---          |-----------------------------------|
*
*
*                   where
*                           STATUS WORD         Reports frame status &/or errors
*                                                   (see SMSC LAN91C111, Sections 8.2 & 8.6).
*
*                           PACKET SIZE         Packet frame size, in octets & MUST be a multiple of 2.
*
*                           LAST DATA OCTET     Last data octet if the total number of data octets is odd
*                                                   (see SMSC LAN91C111, Section 8.1).
*
*                           CONTROL             Packet control status (see SMSC LAN91C111, Sections 8.1).
*********************************************************************************************************
*/

#define  LAN91C111_PKT_FRAME_SIZE_OVRHD                    6    /* Nbr ovrhd octets in frame.                           */

#define  LAN91C111_PKT_FRAME_SIZE_MASK                0x07FE
#define  LAN91C111_PKT_FRAME_CTRL_MASK                0xFF00
#define  LAN91C111_PKT_FRAME_LAST_OCTET_MASK          0x00FF

#define  LAN91C111_PKT_FRAME_IX_STATUS                0x0000
#define  LAN91C111_PKT_FRAME_IX_SIZE                  0x0002
#define  LAN91C111_PKT_FRAME_IX_DATA                  0x0004

#define  LAN91C111_PKT_FRAME_STATUS_NONE              0x0000


#define  LAN91C111_PKT_FRAME_TX_NBR_NONE             (0xFFFF & LAN91C111_REG_PNR_TX_PKT_NBR)


                                                                /* ----------------- FRAME CTRL BITS ------------------ */
#define  LAN91C111_PKT_FRAME_CTRL_RX_ODD          DEF_BIT_13
#define  LAN91C111_PKT_FRAME_CTRL_TX_ODD          DEF_BIT_13
#define  LAN91C111_PKT_FRAME_CTRL_TX_CRC          DEF_BIT_12


/*$PAGE*/
/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


static  void        NetNIC_RxISR_Handler     (void);                    /* Rx ISR signals Net Drv Task of NIC rx pkts.  */


static  void        NetNIC_TxISR_Handler     (void);                    /* Tx ISR signals Net Drv Tx of empty tx FIFO.  */

static  void        NetNIC_TxPktDiscard      (NET_ERR      *perr);


                                                                        /* ------------- LAN91C111 FNCTS -------------- */

static  void        LAN91C111_Init           (void);


                                                                        /* ------------ LAN91C111 RX FNCTS ------------ */

static  void        LAN91C111_RxEn           (void);

static  void        LAN91C111_RxIntDis       (void);

static  void        LAN91C111_RxIntEn        (void);


static  CPU_INT16U  LAN91C111_RxPktGetSize   (void);

static  void        LAN91C111_RxPkt          (void         *ppkt,
                                              CPU_INT16U    size);

static  void        LAN91C111_RxPktDiscard   (CPU_INT16U    size);


                                                                        /* ------------ LAN91C111 TX FNCTS ------------ */

static  void        LAN91C111_TxEn           (void);

static  void        LAN91C111_TxIntEn        (void);


static  void        LAN91C111_TxPkt          (void         *ppkt,
                                              CPU_INT16U    size,
                                              NET_ERR      *perr);

static  void        LAN91C111_TxPktGet       (NET_ERR      *perr);


                                                                        /* ---------- LAN91C111 PHY REG FNCTS --------- */

static  CPU_INT16U  LAN91C111_PhyRdData      (void);

static  CPU_BOOLEAN LAN91C111_PhyRdMDI       (void);

static  void        LAN91C111_PhyWrData      (CPU_INT16U    reg_data);

static  void        LAN91C111_PhyWrIdle      (void);

static  void        LAN91C111_PhyWrPhyAddr   (void);

static  void        LAN91C111_PhyWrPhyReg    (CPU_INT08U    reg);

static  void        LAN91C111_PhyWrBitLow    (void);

static  void        LAN91C111_PhyWrBitHigh   (void);

static  void        LAN91C111_PhyWrBitZ      (void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetNIC_Init()
*
* Description : (1) Initialize Network Interface Card :
*
*                   (a) Perform NIC Layer OS initialization
*                   (b) Initialize NIC status
*                   (c) Initialize NIC statistics & error counters
*                   (d) Initialize LAN91C111
*
*
* Argument(s) : perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                    Network interface card successfully initialized.
*
*                                                                   -------- RETURNED BY NetOS_NIC_Init() : --------
*                               NET_OS_ERR_INIT_NIC_TX_RDY          NIC transmit ready signal NOT successfully
*                                                                       initialized.
*                               NET_OS_ERR_INIT_NIC_TX_RDY_NAME     NIC transmit ready name   NOT successfully
*                                                                       configured.
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetNIC_Init (NET_ERR  *perr)
{
                                                                /* --------------- PERFORM NIC/OS INIT ---------------- */
    NetOS_NIC_Init(perr);                                       /* Create NIC objs.                                     */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

                                                                /* ----------------- INIT NIC STATUS ------------------ */
    NetNIC_ConnStatus           =  DEF_OFF; 


                                                                /* ------------- INIT NIC STAT & ERR CTRS ------------- */
#if (NET_CTR_CFG_STAT_EN        == DEF_ENABLED)
    NetNIC_StatRxPktCtr         =  0;
    NetNIC_StatTxPktCtr         =  0;
#endif

#if (NET_CTR_CFG_ERR_EN         == DEF_ENABLED)
    NetNIC_ErrRxPktDiscardedCtr =  0;
    NetNIC_ErrTxPktDiscardedCtr =  0;
#endif

                                                                /* ----------------- INIT LAN91C111 ------------------- */
    LAN91C111_Init();


   *perr = NET_NIC_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetNIC_IntEn()
*
* Description : Enable NIC interrupts.
*
* Argument(s) : perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                NIC interrupts successfully enabled.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetNIC_IntEn (NET_ERR  *perr)
{
    LAN91C111_RxIntEn();
    LAN91C111_TxIntEn();

   *perr = NET_NIC_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetNIC_ConnStatusChk()
*
* Description : Check the NIC's network connection status.  This function checks the PHY to see if the
*               connection is still established and sets 'NetNIC_ConnStatus' accordingly.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : #### 
*
* Note(s)     : (1) Check NIC connection status :
*
*                   (a) By NIC transmit handler(s)
*                   (b) By NIC status state machine
*                   (c) May be configured with a network timer to execute periodically
*********************************************************************************************************
*/

void  NetNIC_ConnStatusChk (void)
{
    CPU_INT16U  phy_reg_val;
    CPU_INT16U  phy_reg_link_mask;


    phy_reg_val       = LAN91C111_PhyRegRd(LAN91C111_PHY_REG_STATUS);
    phy_reg_link_mask = LAN91C111_PHY_REG_STATUS_NEG_ACK | LAN91C111_PHY_REG_STATUS_LINK_OK;
    if (phy_reg_val & phy_reg_link_mask) {
        NetNIC_ConnStatus = DEF_ON;
    } else {
        NetNIC_ConnStatus = DEF_OFF;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetNIC_ConnStatusGet()
*
* Description : Get NIC's network connection status.
*
*               (1) Obtaining the NIC's network connection status is encapsulated in this function for the 
*                   possibility that obtaining a NIC's connection status requires a non-trivial procedure.
*
*
* Argument(s) : none.
*
* Return(s)   : NIC network connection status :
*
*                   DEF_OFF         Network connection DOWN.
*                   DEF_ON          Network connection UP.
*
* Caller(s)   : NetIF_Pkt_Tx()
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetNIC_ConnStatusGet (void)
{
    return (NetNIC_ConnStatus);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetNIC_ISR_Handler()
*
* Description : (1) Decode ISR & call appropriate ISR handler :
*
*                   (a) LAN91C111 Receive  ISR                  NetNIC_RxISR_Handler()
*                   (b) LAN91C111 Transmit ISR                  NetNIC_TxISR_Handler()
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_ISR() [see Note #2].
*
* Note(s)     : (2) LAN91C111 ISR  MUST call NetNIC_ISR_Handler() & MUST be developer-implemented in
*               
*                       \<Your Product Application>\net_isr*.*
*
*                           where
*                                   <Your Product Application>    directory path for Your Product's Application
*
*               (3) This function clears the interrupt suorce(s) on an external interrupt controller &, if
*                   ENABLED, MUST be developer-implemented in
*               
*                       \<Your Product Application>\net_isr.c
*
*                           where
*                                   <Your Product Application>    directory path for Your Product's Application
*********************************************************************************************************
*/

void  NetNIC_ISR_Handler (void)
{
    CPU_INT16U  int_reg;
    CPU_INT08U  int_en;
    CPU_INT08U  int_act;


                                                                    /* ------------ DETERMINE NIC INT SRC ------------- */
    int_reg = LAN91C111_RegRd(LAN91C111_REG_INT_BANK,
                              LAN91C111_REG_INT_OFFSET);

    int_en  = (int_reg & LAN91C111_REG_INT_MASK_EN    ) >> 8;       /* Get en'd ints.                                   */
    int_act = (int_reg & LAN91C111_REG_INT_MASK_STATUS) & int_en;   /* Determine which en'd ints active.                */


                                                                    /* ------------- HANDLE ENABLED INTS -------------- */
    if (int_act & LAN91C111_REG_INT_RX) {
        NetNIC_RxISR_Handler();
    }

    if (int_act & LAN91C111_REG_INT_TX) {
        NetNIC_TxISR_Handler();
    }


#if (NET_NIC_CFG_INT_CTRL_EN == DEF_ENABLED)
    NetNIC_IntClr();                                                /* Clr int ctrl'r LAN91C111 int (see Note #3).      */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetNIC_RxPktGetSize()
*
* Description : Get network packet size from NIC.
*
* Argument(s) : none.
*
* Return(s)   : Size, in octets, of NIC's next network packet.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16U  NetNIC_RxPktGetSize (void)
{
    CPU_INT16U  size;


    size = LAN91C111_RxPktGetSize();

    return (size);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetNIC_RxPkt()
*
* Description : Read network packet from NIC into buffer.
*
* Argument(s) : ppkt        Pointer to memory buffer to receive NIC packet.
*               ----        Argument validated in NetIF_RxTaskHandler().
*
*               size        Number of packet frame octets to read into buffer.
*               ----        Argument checked   in NetIF_RxTaskHandler().
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Packet successfully read.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
* Note(s)     : (1) NetNIC_RxPkt() blocked until network initialization completes; perform NO action.
*
*               (2) After packet read is complete or on any error, receive interrupts MUST be re-ENABLED
*                   (see also 'NetNIC_RxISR_Handler()  Note #2').
*********************************************************************************************************
*/

void  NetNIC_RxPkt (void        *ppkt,
                    CPU_INT16U   size,
                    NET_ERR     *perr)
{
#if ((NET_CTR_CFG_STAT_EN     == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif


    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit rx (see Note #1).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }


    LAN91C111_RxPkt(ppkt, size);                                /* Rd rx pkt from NIC.                                  */

    LAN91C111_RxIntEn();                                        /* See Note #2.                                         */


    NET_CTR_STAT_INC(NetNIC_StatRxPktCtr);

   *perr = NET_NIC_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetNIC_RxPktDiscard()
*
* Description : Discard network packet from NIC to free NIC packet frames for new receive packets.
*
* Argument(s) : size        Number of packet frame octets.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Packet successfully discarded.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
* Note(s)     : (1) NetNIC_RxPktDiscard() blocked until network initialization completes; perform NO action.
*
*               (2) #### 'perr' may NOT be necessary (remove before product release if unnecessary).
*********************************************************************************************************
*/

void  NetNIC_RxPktDiscard (CPU_INT16U   size,
                           NET_ERR     *perr)
{
#if ((NET_CTR_CFG_ERR_EN      == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif


    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit discard (see Note #1).    */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }


    LAN91C111_RxPktDiscard(size);

    NET_CTR_ERR_INC(NetNIC_ErrRxPktDiscardedCtr);

   *perr = NET_NIC_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             NetNIC_TxPkt()
*
* Description : Transmit data packets from network driver layer to network interface card.
*
* Argument(s) : ppkt        Pointer to memory buffer to transmit NIC packet.
*               ----        Argument validated in NetIF_Pkt_Tx().
*
*               size        Number of packet frame octets to write to frame.
*               ----        Argument checked   in NetIF_Tx().
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Packet successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               - RETURNED BY NetNIC_TxPktDiscard() : -
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx().
*
* Note(s)     : (1) NetNIC_TxPkt() blocked until network initialization completes; perform NO action.
*********************************************************************************************************
*/

void  NetNIC_TxPkt (void        *ppkt,
                    CPU_INT16U   size,
                    NET_ERR     *perr)
{
#if ((NET_CTR_CFG_STAT_EN     == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif


    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit tx (see Note #1).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }


    LAN91C111_TxPkt(ppkt, size, perr);                          /* Tx pkt to LAN91C111.                                 */

    if (*perr != NET_NIC_ERR_NONE) {
        NetNIC_TxPktDiscard(perr);
        return;
    }

    NET_CTR_STAT_INC(NetNIC_StatTxPktCtr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       NetNIC_RxISR_Handler()
*
* Description : Signal Network Interface Receive Task that a receive packet is available.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_ISR_Handler().
*
* Note(s)     : (1) NetNIC_ISR_Handler() decodes LAN91C111 Receive ISR & calls NetNIC_RxISR_Handler().
*
*               (2) Since LAN91C111 receive interrupt is cleared ONLY when the receive FIFO is empty
*                   (see SMSC LAN91C111, Section 8.21 'RCV INT'), receive interrupts MUST be DISABLED
*                   until the receive packet is read from the receive FIFO (see 'NetNIC_RxPkt()  Note #2').
*********************************************************************************************************
*/

static  void  NetNIC_RxISR_Handler (void)
{
    NET_ERR  err;


    NetOS_IF_RxTaskSignal(&err);                                /* Signal Net IF Rx Task of NIC rx pkt.                 */

    switch (err) {
        case NET_IF_ERR_NONE:
             LAN91C111_RxIntDis();                              /* See Note #2.                                         */
             break;


        case NET_IF_ERR_RX_Q_FULL:
        case NET_IF_ERR_RX_Q_POST_FAIL:
        default:
             NetNIC_RxPktDiscard(0, &err);                      /* If any net drv signal err, discard rx pkt.           */
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetNIC_TxISR_Handler()
*
* Description : (1) Clear transmit interrupt &/or transmit errors :
*
*                   (a) Acknowledge transmit interrupt
*                   (b) Re-enable   transmitter; disabled on any transmit errors
*                         (see SMSC LAN91C111, Section 8.5 'TXENA')
*                   (c) Post transmit FIFO ready signal
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_ISR_Handler().
*
* Note(s)     : (2) NetNIC_ISR_Handler() decodes LAN91C111 Transmit ISR & calls NetNIC_TxISR_Handler().
*********************************************************************************************************
*/

static  void  NetNIC_TxISR_Handler (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    CPU_INT16U  reg_val;


                                                                /* -------------------- ACK TX INT -------------------- */
    CPU_CRITICAL_ENTER();                                       /* See 'LAN91C111 REGISTERS  Note #5b'.                 */

    reg_val  =  LAN91C111_RegRd(LAN91C111_REG_INT_BANK,
                                LAN91C111_REG_INT_OFFSET);
    reg_val &=  LAN91C111_REG_INT_MASK_EN;                      /* Mask int status bits.                                */
    reg_val |=  LAN91C111_REG_INT_TX_ACK;

    LAN91C111_RegWr(LAN91C111_REG_INT_BANK,
                    LAN91C111_REG_INT_OFFSET,
                    reg_val);

    CPU_CRITICAL_EXIT();


                                                                /* --------------------- RE-EN TX --------------------- */
    LAN91C111_TxEn();


                                                                /* --------------- POST TX READY SIGNAL --------------- */
    NetOS_NIC_TxRdySignal();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetNIC_TxPktDiscard()
*
* Description : On any Transmit errors, set error.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_TxPkt().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetNIC_TxPktDiscard (NET_ERR  *perr)
{
#if ((NET_CTR_CFG_ERR_EN      == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif


    NET_CTR_ERR_INC(NetNIC_ErrTxPktDiscardedCtr);

   *perr = NET_ERR_TX;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           LAN91C111_Init()
*
* Description : (1) Initialize & start LAN91C111 :
*
*                   (a) Initialize Registers
*                   (b) Initialize MAC  Address
*                   (c) Initialize Auto Negotiation
*                   (d) Initialize Interrupts
*                   (e) Enable     Receiver/Transmitter
*                   (f) Initialize External Interrupt Controller    See Note #4
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_Init().
*
* Note(s)     : (2) See this 'net_nic.h  LAN91C111 REGISTER BITS' for register bit summary.
*
*               (3) (a) Assumes MAC address to set has previously been initialized by
*
*                       (1) LAN91C111's EEPROM          for LAN91C111_MAC_ADDR_SEL_EEPROM
*                       (2) Application code            for LAN91C111_MAC_ADDR_SEL_CFG
*
*                   (b) MAC address configuration takes advantage of LAN91C111's consecutive MAC Address
*                       register offsets (see SMSC LAN91C111, Section 8.13) :
*
*                             LAN91C111 Bank 1
*                                Offset 4               MAC Address Octet #0    Most  Significant Octet
*                                Offset 5               MAC Address Octet #1
*                                Offset 6               MAC Address Octet #2
*                                Offset 7               MAC Address Octet #3
*                                Offset 8               MAC Address Octet #4
*                                Offset 9               MAC Address Octet #5    Least Significant Octet
*
*                   (c) MAC address registers accessed as 16-bit Little-endian words 
*                           (see this 'net_nic.c  Note #3a') :
*
*
*                                          HI octet      LO octet
*                                       -----------------------------
*                                       | MAC Address | MAC Address |
*                                       |  Octet #1   |  Octet #0   |
*                                       -----------------------------
*                                       | MAC Address | MAC Address |
*                                       |  Octet #3   |  Octet #2   |
*                                       -----------------------------
*                                       | MAC Address | MAC Address |
*                                       |  Octet #5   |  Octet #4   |
*                                       -----------------------------
*
*                   (d) Since MAC address register configuration accesses multiple registers in sequence,
*                       critical sections enclosing the entire sequence are REQUIRED (see 'LAN91C111 
*                       REGISTERS  Note #5b').
*
*               (4) Although SMSC documentation states that the PHY should complete Auto-Negotiation
*                   within 1.5 second, this delay ofter is not enough.  Therefore, if it is not done
*                   after this time, another 1.5 second delay in introduced before resetting the process.
*
*               (5) Interrupts MUST be enabled ONLY after ALL network initialization is complete (see also
*                   'net.c  Net_Init()  Note #4d').
*
*               (6) This function initializes an external interrupt controller (if present) &, if ENABLED, 
*                   MUST be developer-implemented in
*
*                       \<Your Product Application>\net_isr.c
*
*                           where
*                                   <Your Product Application>    directory path for Your Product's Application
*********************************************************************************************************
*/
/*$PAGE*/
static  void  LAN91C111_Init (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR       cpu_sr;
#endif
    CPU_INT16U   reg_val;
    CPU_INT08U   i;
    CPU_BOOLEAN  dly_auto_nego;
    CPU_BOOLEAN  link;
    NET_ERR      err;


    LAN91C111_RegBankCur = LAN91C111_REG_BANK_DFLT;

    LAN91C111_PwrCtrl(DEF_ON);                                  /* Pwr LAN91C111 ON.                                    */
    LAN91C111_DlyReset();


                                                                /* --------- INIT SW RESET FOR AUTO NEG INIT ---------- */
                                                                /* See SMSC AN9.6, Section 4.7.1.                       */
    CPU_CRITICAL_ENTER();                                       /* See 'LAN91C111 REGISTERS  Note #5b'.                 */

    reg_val  =  LAN91C111_RegRd(LAN91C111_REG_RCR_BANK,
                                LAN91C111_REG_RCR_OFFSET);

    reg_val |=  LAN91C111_REG_RCR_SW_RESET;                     /* Initiate SW reset.                                   */
    LAN91C111_RegWr(LAN91C111_REG_RCR_BANK, 
                    LAN91C111_REG_RCR_OFFSET,
                    reg_val);
    reg_val &= ~LAN91C111_REG_RCR_SW_RESET;
    LAN91C111_RegWr(LAN91C111_REG_RCR_BANK, 
                    LAN91C111_REG_RCR_OFFSET,
                    reg_val);


    CPU_CRITICAL_EXIT();
    
    LAN91C111_DlyReset();
                                                                /* Resume auto neg init AFTER reg inits.                */



/*$PAGE*/
                                                                /* ------------------- INIT TCR REG ------------------- */
    reg_val = LAN91C111_REG_TCR_SW_DPLX_HALF       |
              LAN91C111_REG_TCR_EPH_LPBK_DIS       |
              LAN91C111_REG_TCR_SQET_STOP_DIS      |
              LAN91C111_REG_TCR_RX_LPBK_DIS        |
              LAN91C111_REG_TCR_MON_CARR_DIS       |
              LAN91C111_REG_TCR_TX_CRC_EN          |
              LAN91C111_REG_TCR_TX_PAD_EN          |
              LAN91C111_REG_TCR_TX_COLL_NOT_FORCED |
              LAN91C111_REG_TCR_LPBK_OUT_LO        |
              LAN91C111_REG_TCR_TX_DIS;

    LAN91C111_RegWr(LAN91C111_REG_TCR_BANK,
                    LAN91C111_REG_TCR_OFFSET,
                    reg_val);


                                                                /* ------------------- INIT RCR REG ------------------- */
    reg_val = LAN91C111_REG_RCR_RX_FLTR_CARR_DIS |
              LAN91C111_REG_RCR_RX_COLL_DIS      |
              LAN91C111_REG_RCR_RX_CRC_DIS       |
              LAN91C111_REG_RCR_RX_DIS           |
              LAN91C111_REG_RCR_RX_MULTICAST_TBL |
              LAN91C111_REG_RCR_RX_PROMISC_DIS;

    LAN91C111_RegWr(LAN91C111_REG_RCR_BANK,
                    LAN91C111_REG_RCR_OFFSET,
                    reg_val);


                                                                /* ------------------- INIT RPCR REG ------------------ */
    reg_val = LAN91C111_REG_RPCR_SPD_10      |
              LAN91C111_REG_RPCR_DPLX_HALF   |
              LAN91C111_REG_RPCR_AUTO_NEG_EN |
              LAN91C111_REG_RPCR_LED_A_LINK  |
              LAN91C111_REG_RPCR_LED_B_RX_TX;

    LAN91C111_RegWr(LAN91C111_REG_RPCR_BANK,
                    LAN91C111_REG_RPCR_OFFSET,
                    reg_val);


                                                                /* ------------------- INIT CFG REG ------------------- */
    reg_val = 0x20B1                            |
              LAN91C111_REG_CFG_EPH_LO_PWR_DIS  |
              LAN91C111_REG_CFG_WAIT_STATES_DIS |
              LAN91C111_REG_CFG_PHY_INT;

    LAN91C111_RegWr(LAN91C111_REG_CFG_BANK,
                    LAN91C111_REG_CFG_OFFSET,
                    reg_val);


                                                                /* ------------------- INIT CTRL REG ------------------ */
    reg_val = 0x1210                            |
              LAN91C111_REG_CTRL_RX_CRC_ERR_DIS |
              LAN91C111_REG_CTRL_TX_REL_MAN     |
              LAN91C111_REG_CTRL_LINK_ERR_DIS   |
              LAN91C111_REG_CTRL_CTR_ERR_DIS    |
              LAN91C111_REG_CTRL_TX_ERR_DIS     |
              LAN91C111_REG_CTRL_EEPROM_DFLT;

    LAN91C111_RegWr(LAN91C111_REG_CTRL_BANK,
                    LAN91C111_REG_CTRL_OFFSET,
                    reg_val);


/*$PAGE*/
                                                                /* ----------------- INIT MAC REG/ADDR ---------------- */
                                                                /* See Note #3.                                         */
    CPU_CRITICAL_ENTER();                                       /* See 'LAN91C111 REGISTERS  Note #5b'.                 */
#if (LAN91C111_CFG_MAC_ADDR_SEL == LAN91C111_MAC_ADDR_SEL_EEPROM)
    for (i = 0; i < sizeof(NetIF_MAC_Addr); i += sizeof(CPU_INT16U)) {
        reg_val               =  LAN91C111_RegRd(LAN91C111_REG_MAC_0_BANK,
                                                 LAN91C111_REG_MAC_0_OFFSET + i);

        NetIF_MAC_Addr[i    ] = (CPU_INT08U) reg_val;
        NetIF_MAC_Addr[i + 1] = (CPU_INT08U)(reg_val  >> DEF_OCTET_NBR_BITS);
    }
#else
    for (i = 0; i < sizeof(NetIF_MAC_Addr); i += sizeof(CPU_INT16U)) {
        reg_val = ((CPU_INT16U)NetIF_MAC_Addr[i    ]) | 
                 (((CPU_INT16U)NetIF_MAC_Addr[i + 1]) << DEF_OCTET_NBR_BITS);

        LAN91C111_RegWr( LAN91C111_REG_MAC_0_BANK,
                        (LAN91C111_REG_MAC_0_OFFSET + i),
                         reg_val);
    }
#endif
    CPU_CRITICAL_EXIT();

    NetIF_MAC_AddrValid = DEF_YES;


                                                                /* ------------------ INIT AUTO NEG ------------------- */
                                                                /* See SMSC AN9.6, Section 4.7.1.                       */
                                                                /* SW reset performed PRIOR to reg inits.               */
   
    reg_val = LAN91C111_PHY_REG_CTRL_RESET;                     /* ... Reset the PHY                                    */
    LAN91C111_PhyRegWr(LAN91C111_PHY_REG_CTRL, reg_val);  

    LAN91C111_DlyReset();

    reg_val = LAN91C111_PHY_REG_CTRL_NEG_AUTO;                  /* ... Set ANEG bit to 1 (turn off isolation mode).     */
    LAN91C111_PhyRegWr(LAN91C111_PHY_REG_CTRL, reg_val);  

    i             = LAN91C111_INIT_AUTO_NEG_RETRIES;
    dly_auto_nego = DEF_YES;
    do {
        LAN91C111_DlyAutoNegAck();                              /* ... Preemptive dly.                                  */
        reg_val =  LAN91C111_PhyRegRd(LAN91C111_PHY_REG_STATUS);
        if (reg_val & (LAN91C111_PHY_REG_STATUS_NEG_ACK | LAN91C111_PHY_REG_STATUS_LINK_OK)) {
            link = DEF_ON;              
        } else if (dly_auto_nego == DEF_YES) {                  /* See note #4.                                         */
            link          = DEF_OFF;
            dly_auto_nego = DEF_NO;
        } else {
            link    = DEF_OFF;                                  /* ... Restart auto-negotiation                         */
            reg_val = LAN91C111_PHY_REG_CTRL_SPD_100        |
                      LAN91C111_PHY_REG_CTRL_NEG_AUTO       |
                      LAN91C111_PHY_REG_CTRL_NEG_AUTO_RESET;
            LAN91C111_PhyRegWr(LAN91C111_PHY_REG_CTRL, reg_val);     
            i--;
            dly_auto_nego = DEF_YES;
        }
    } while ((link == DEF_OFF) && (i > 0));                     /* Attempt auto neg until success ack'd.                */

    NetNIC_ConnStatus = DEF_ON;                                 /* Assume connection is established                     */

/*$PAGE*/
                                                                /* ------------------- INIT INT REG ------------------- */
    reg_val  = LAN91C111_REG_INT_MD_DIS         |
               LAN91C111_REG_INT_ERCV_DIS       |
               LAN91C111_REG_INT_ERH_DIS        |
               LAN91C111_REG_INT_RX_OVERRUN_DIS |
               LAN91C111_REG_INT_TX_ALLOC_DIS   |
               LAN91C111_REG_INT_TX_EMPTY_DIS   |
               LAN91C111_REG_INT_TX_DIS         |               /* Ints en'd at end of Net_Init() [see Note #5].        */
               LAN91C111_REG_INT_RX_DIS         |               /* Ints en'd at end of Net_Init() [see Note #5].        */
               LAN91C111_REG_INT_MD_ACK         |
               LAN91C111_REG_INT_ERCV_ACK       |
               LAN91C111_REG_INT_RX_OVERRUN_ACK |
               LAN91C111_REG_INT_TX_EMPTY_ACK   |
               LAN91C111_REG_INT_TX_ACK;

    LAN91C111_RegWr(LAN91C111_REG_INT_BANK,
                    LAN91C111_REG_INT_OFFSET,
                    reg_val);


                                                                /* --------------------- EN RX/TX --------------------- */
    LAN91C111_RxEn();
    LAN91C111_TxEn();


                                                                /* ---------------- GET GLOBAL TX PKT ----------------- */
    LAN91C111_TxPktGet(&err);
    if (err != NET_NIC_ERR_NONE) {
        NetNIC_ConnStatus = DEF_OFF;
    }


#if (NET_NIC_CFG_INT_CTRL_EN == DEF_ENABLED)
                                                                /* ----------------- INIT INT CTRL'R ------------------ */
    NetNIC_IntInit();                                           /* See Note #6.                                         */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          LAN91C111_RxEn()
*
* Description : Enable LAN91C111 Receiver.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : LAN91C111_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  LAN91C111_RxEn (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    CPU_INT16U  reg_val;


    CPU_CRITICAL_ENTER();                                       /* See 'LAN91C111 REGISTERS  Note #5b'.                 */


    reg_val  =  LAN91C111_RegRd(LAN91C111_REG_RCR_BANK,
                                LAN91C111_REG_RCR_OFFSET);
    reg_val |=  LAN91C111_REG_RCR_RX_EN;

    LAN91C111_RegWr(LAN91C111_REG_RCR_BANK, 
                    LAN91C111_REG_RCR_OFFSET,
                    reg_val);


    CPU_CRITICAL_EXIT();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        LAN91C111_RxIntDis()
*
* Description : Disable LAN91C111 Receiver Interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_RxISR_Handler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  LAN91C111_RxIntDis (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    CPU_INT16U  reg_val;


    CPU_CRITICAL_ENTER();                                       /* See 'LAN91C111 REGISTERS  Note #5b'.                 */

    reg_val  =  LAN91C111_RegRd(LAN91C111_REG_INT_BANK,
                                LAN91C111_REG_INT_OFFSET);
    reg_val &=  LAN91C111_REG_INT_MASK_EN;                      /* Mask int status bits.                                */
    reg_val &= ~LAN91C111_REG_INT_RX_EN;

    LAN91C111_RegWr(LAN91C111_REG_INT_BANK,
                    LAN91C111_REG_INT_OFFSET,
                    reg_val);

    CPU_CRITICAL_EXIT();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         LAN91C111_RxIntEn()
*
* Description : Enable LAN91C111 Receiver Interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_IntEn(),
*               NetNIC_RxPkt(),
*               NetNIC_RxPktDiscard().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  LAN91C111_RxIntEn (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    CPU_INT16U  reg_val;


    CPU_CRITICAL_ENTER();                                       /* See 'LAN91C111 REGISTERS  Note #5b'.                 */


    reg_val  =  LAN91C111_RegRd(LAN91C111_REG_INT_BANK,
                                LAN91C111_REG_INT_OFFSET);
    reg_val &=  LAN91C111_REG_INT_MASK_EN;                      /* Mask int status bits.                                */
    reg_val |=  LAN91C111_REG_INT_RX_EN;

    LAN91C111_RegWr(LAN91C111_REG_INT_BANK,
                    LAN91C111_REG_INT_OFFSET,
                    reg_val);


    CPU_CRITICAL_EXIT();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      LAN91C111_RxPktGetSize()
*
* Description : (1) Get network packet size from NIC :
*
*                   (a) Read packet frame size
*                   (b) Read packet frame's control octet to check for odd packet frame size
*
*
* Argument(s) : none.
*
* Return(s)   : Size, in octets, of NIC's next network packet.
*
* Caller(s)   : NetNIC_RxPktGetSize().
*
* Note(s)     : (2) See 'LAN91C111 PACKET FRAMES' for packet frame format.
*********************************************************************************************************
*/

static  CPU_INT16U  LAN91C111_RxPktGetSize (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    CPU_INT16U  reg_val;
    CPU_INT16U  size;
    CPU_INT16U  ctrl_ix;
    CPU_INT16U  ctrl;


    CPU_CRITICAL_ENTER();                                       /* See 'LAN91C111 REGISTERS  Note #5b'.                 */

                                                                /* ------------- RD SIZE FROM RX PKT FRAME ------------ */
    reg_val  = LAN91C111_REG_PTR_FIFO_RX |
               LAN91C111_REG_PTR_INC_MAN |
               LAN91C111_REG_PTR_RD      |
               LAN91C111_PKT_FRAME_IX_SIZE;

    LAN91C111_RegWr(LAN91C111_REG_PTR_BANK, 
                    LAN91C111_REG_PTR_OFFSET,
                    reg_val);

    size     = LAN91C111_RegRd(LAN91C111_REG_DATA_0_BANK,
                               LAN91C111_REG_DATA_0_OFFSET);
    size    &= LAN91C111_PKT_FRAME_SIZE_MASK;
    size    -= LAN91C111_PKT_FRAME_SIZE_OVRHD;


                                                                /* ------------- RD CTRL FROM RX PKT FRAME ------------ */
    ctrl_ix  = LAN91C111_PKT_FRAME_IX_DATA + size;
    reg_val  = LAN91C111_REG_PTR_FIFO_RX |
               LAN91C111_REG_PTR_INC_MAN |
               LAN91C111_REG_PTR_RD      |
               ctrl_ix;

    LAN91C111_RegWr(LAN91C111_REG_PTR_BANK, 
                    LAN91C111_REG_PTR_OFFSET,
                    reg_val);

    ctrl = LAN91C111_RegRd(LAN91C111_REG_DATA_0_BANK,
                           LAN91C111_REG_DATA_0_OFFSET);
    if (ctrl & LAN91C111_PKT_FRAME_CTRL_RX_ODD) {               /* If odd bit set, inc size.                            */
        size++;
    }


    CPU_CRITICAL_EXIT();

    return (size);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          LAN91C111_RxPkt()
*
* Description : (1) Read network packet from NIC into buffer :
*
*                   (a) Setup receive packet frame pointer
*                   (b) Read  receive packet frame from LAN91C111
*                       (1) Read majority of frame in 16-bit words
*                       (2) Read last frame octet, if any
*                   (c) Release receive packet
*
*
* Argument(s) : ppkt        Pointer to memory buffer to receive NIC packet.
*               ----        Argument checked in NetNIC_RxPkt().
*
*               size        Number of packet frame octets to read into buffer.
*               ----        Argument checked in NetNIC_RxPkt().
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_RxPkt().
*
* Note(s)     : (2) See 'LAN91C111 PACKET FRAMES' for packet frame format.
*
*               (3) The last 16-bit word in each LAN91C111 packet frame is a concatenation of a control octet 
*                   & a possible last data octet.  Thus, this last 16-bit word is accessed & modified similar
*                   to LAN91C111 register accesses.  Therefore, this last 16-bit word MUST NOT be converted
*                   from NIC-order to host-order.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  LAN91C111_RxPkt (void        *ppkt,
                               CPU_INT16U   size)
{
    CPU_INT16U  *p_data;
    CPU_INT16U   size_rem;
    CPU_INT16U   reg_val;
    CPU_INT16U   pkt_data;
    CPU_INT08U   pkt_data_last_octet;
    CPU_INT08U  *pdata_last;


    p_data   = (CPU_INT16U *)ppkt;
    size_rem =  size;

                                                                /* -------- SETUP RX PKT FRAME PTR TO DATA IX --------- */
    reg_val  =  LAN91C111_REG_PTR_FIFO_RX   |
                LAN91C111_REG_PTR_INC_AUTO  |
                LAN91C111_REG_PTR_RD        |
                LAN91C111_PKT_FRAME_IX_DATA;

    LAN91C111_RegWr(LAN91C111_REG_PTR_BANK, 
                    LAN91C111_REG_PTR_OFFSET,
                    reg_val);


                                                                /* ------------------ RD RX PKT FRAME ----------------- */
    while (size_rem > 1) {                                      /* Rd rx pkt as 16-bit words.                           */
        pkt_data  = LAN91C111_RegRd(LAN91C111_REG_DATA_0_BANK,
                                    LAN91C111_REG_DATA_0_OFFSET);
        pkt_data  = LAN91C111_NIC_TO_HOST_16(pkt_data);         /* Convert from NIC-order to host-order.                */
       *p_data    = pkt_data;
        p_data++;
        size_rem -= sizeof(CPU_INT16U);
    }

    if (size_rem > 0) {                                         /* If last octet avail, ... (see Note #3)               */
        pkt_data            =  LAN91C111_RegRd(LAN91C111_REG_DATA_0_BANK,
                                               LAN91C111_REG_DATA_0_OFFSET);
        pkt_data_last_octet = (CPU_INT08U  )pkt_data;           /* ... mask ctrl octet  ...                             */
        pdata_last          = (CPU_INT08U *)p_data;
       *pdata_last          =  pkt_data_last_octet;             /* ... & rd as last octet.                              */
    }


                                                                /* --------------------- REL PKT ---------------------- */
    reg_val = LAN91C111_REG_MMU_CMD_RX_REMOVE_REL;

    LAN91C111_RegWr(LAN91C111_REG_MMU_BANK,
                    LAN91C111_REG_MMU_OFFSET,
                    reg_val);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      LAN91C111_RxPktDiscard()
*
* Description : Discard network packet from NIC to free NIC packet frames for new receive packets.
*
* Argument(s) : size        Number of packet frame octets.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_RxPktDiscard().
*
* Note(s)     : (1) 'size' NOT required for LAN91C111 packet discard.
*
*               (2) After packet discard is complete, receive interrupts MUST be re-ENABLED (see also
*                   'NetNIC_RxISR_Handler()  Note #2').
*********************************************************************************************************
*/

static  void  LAN91C111_RxPktDiscard (CPU_INT16U  size)
{
    size = size;                                                /* See Note #1.                                         */

    LAN91C111_RegWr(LAN91C111_REG_MMU_BANK,
                    LAN91C111_REG_MMU_OFFSET,
                    LAN91C111_REG_MMU_CMD_RX_REMOVE_REL);

    LAN91C111_RxIntEn();                                        /* See Note #2.                                         */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          LAN91C111_TxEn()
*
* Description : Enable LAN91C111 Transmitter.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : LAN91C111_Init(),
*               NetNIC_TxISR_Handler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  LAN91C111_TxEn (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    CPU_INT16U  reg_val;


    CPU_CRITICAL_ENTER();                                       /* See 'LAN91C111 REGISTERS  Note #5b'.                 */


    reg_val  =  LAN91C111_RegRd(LAN91C111_REG_TCR_BANK,
                                LAN91C111_REG_TCR_OFFSET);
    reg_val |=  LAN91C111_REG_TCR_TX_EN;

    LAN91C111_RegWr(LAN91C111_REG_TCR_BANK,
                    LAN91C111_REG_TCR_OFFSET,
                    reg_val);


    CPU_CRITICAL_EXIT();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         LAN91C111_TxIntEn()
*
* Description : Enable LAN91C111 Transmit Interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_IntEn().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  LAN91C111_TxIntEn (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    CPU_INT16U  reg_val;


    CPU_CRITICAL_ENTER();

    reg_val  =  LAN91C111_RegRd(LAN91C111_REG_INT_BANK,
                                LAN91C111_REG_INT_OFFSET);
    reg_val &=  LAN91C111_REG_INT_MASK_EN;                      /* Mask int status bits.                                */
    reg_val |=  LAN91C111_REG_INT_TX_EN;

    LAN91C111_RegWr(LAN91C111_REG_INT_BANK,
                    LAN91C111_REG_INT_OFFSET,
                    reg_val);

    CPU_CRITICAL_EXIT();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          LAN91C111_TxPkt()
*
* Description : (1) Write network packet into LAN91C111 packet frame & transmit :
*
*                   (a) Setup    transmit packet frame
*                   (b) Write network packet data into transmit packet frame
*                       (1) Write majority of frame in 16-bit words
*                       (2) Write last frame octet, if any
*                   (c) Transmit packet frame
*
*
* Argument(s) : ppkt        Pointer to memory buffer to transmit NIC packet.
*               ----        Argument checked in NetNIC_TxPkt().
*
*               size        Number of packet frame octets to write to frame.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Packet successfully transmitted.
*                               LAN91C111_ERR_TX_ALLOC          NO packet frame available for transmit.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_TxPkt().
*
* Note(s)     : (2) See 'LAN91C111 PACKET FRAMES' for packet frame format.
*
*               (3) Backwards-compatibility with LAN91C96 NOT implemented.
*
*               (4) The last 16-bit word in each LAN91C111 packet frame is a concatenation of a control octet 
*                   & a possible last data octet.  Thus, this last 16-bit word is accessed & modified similar
*                   to LAN91C111 register accesses.  Therefore, this last 16-bit word MUST NOT be converted
*                   from host-order to NIC-order.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  LAN91C111_TxPkt (void        *ppkt,
                               CPU_INT16U   size,
                               NET_ERR     *perr)
{
    CPU_INT16U    reg_val;
    CPU_INT16U    tx_pkt_size;
    CPU_INT16U    tx_pkt_size_rem;
    CPU_INT16U    tx_pkt_ctrl_last_data;
    CPU_INT16U   *p_data;
    CPU_INT08U   *pdata_last;
    CPU_INT08U    tx_pkt_data_last_octet;



                                                                /* -------------- SETUP TX PKT ALLOC PTR -------------- */
    reg_val = LAN91C111_TxPktNbr;

    LAN91C111_RegWr(LAN91C111_REG_PNR_BANK,
                    LAN91C111_REG_PNR_OFFSET,
                    reg_val);


    reg_val = LAN91C111_REG_PTR_FIFO_TX         |
              LAN91C111_REG_PTR_INC_AUTO        |
              LAN91C111_REG_PTR_WR              |
              LAN91C111_REG_PTR_TX_UNDERRUN_DIS |
              LAN91C111_PKT_FRAME_IX_STATUS;

    LAN91C111_RegWr(LAN91C111_REG_PTR_BANK, 
                    LAN91C111_REG_PTR_OFFSET,
                    reg_val);


                                                                /* ------------------ WR TX PKT FRAME ----------------- */
                                                                /* Wr null tx status word.                              */
    reg_val = LAN91C111_HOST_TO_NIC_16(LAN91C111_PKT_FRAME_STATUS_NONE);
    LAN91C111_RegWr(LAN91C111_REG_DATA_0_BANK,
                    LAN91C111_REG_DATA_0_OFFSET,
                    reg_val);

                                                                /* Wr tx pkt size.                                      */
    tx_pkt_size_rem  = size;
    tx_pkt_size      = size & LAN91C111_PKT_FRAME_SIZE_MASK;
    tx_pkt_size     +=        LAN91C111_PKT_FRAME_SIZE_OVRHD;
    tx_pkt_size      = LAN91C111_HOST_TO_NIC_16(tx_pkt_size);
    LAN91C111_RegWr(LAN91C111_REG_DATA_0_BANK,
                    LAN91C111_REG_DATA_0_OFFSET,
                    tx_pkt_size);             

                                                                /* Wr tx pkt data.                                      */
    p_data = (CPU_INT16U *)ppkt;
    while (tx_pkt_size_rem > 1) {                               /* Wr tx pkt as 16-bit words.                           */
       *p_data = LAN91C111_NIC_TO_HOST_16(*p_data);             /* Convert from host-order to NIC-order.                */
        LAN91C111_RegWr(LAN91C111_REG_DATA_0_BANK,
                        LAN91C111_REG_DATA_0_OFFSET,
                       *p_data);             
        p_data++;
        tx_pkt_size_rem -= sizeof(CPU_INT16U);
    }

    tx_pkt_ctrl_last_data = LAN91C111_PKT_FRAME_CTRL_TX_CRC;
    if (tx_pkt_size_rem > 0) {                                  /* If last octet avail, wr last octet (see Note #4).    */
        tx_pkt_ctrl_last_data  |=  LAN91C111_PKT_FRAME_CTRL_TX_ODD;
        pdata_last              = (CPU_INT08U *)p_data;
        tx_pkt_data_last_octet  = *pdata_last;
        tx_pkt_ctrl_last_data  |= (CPU_INT16U  )tx_pkt_data_last_octet;
    }
                                                                /* Wr tx ctrl/last octet word.                          */
    LAN91C111_RegWr(LAN91C111_REG_DATA_0_BANK,
                    LAN91C111_REG_DATA_0_OFFSET,
                    tx_pkt_ctrl_last_data);             


                                                                /* ---------------------- TX PKT ---------------------- */
    reg_val = LAN91C111_REG_MMU_CMD_TX_PKT;

    LAN91C111_RegWr(LAN91C111_REG_MMU_BANK,
                    LAN91C111_REG_MMU_OFFSET,
                    reg_val);


   *perr = NET_NIC_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        LAN91C111_TxPktGet()
*
* Description : (1) Get one global LAN91C111 packet frame for all transmit operations :
*
*                   (a) Request  transmit packet frame
*                   (b) Wait for transmit packet frame
*                   (c) Configure global transmit packet frame number
*
*
* Argument(s) : perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Packet successfully transmitted.
*                               LAN91C111_ERR_TX_ALLOC          NO packet frame available for transmit.
*
* Return(s)   : none.
*
* Caller(s)   : LAN91C111_Init();
*
* Note(s)     : (2) See 'LAN91C111 PACKET FRAMES' for packet frame format.
*
*               (3) Backwards-compatibility with LAN91C96 NOT implemented.
*********************************************************************************************************
*/

static  void  LAN91C111_TxPktGet (NET_ERR  *perr)
{
    CPU_BOOLEAN  tx_alloc;
    CPU_INT16U   reg_val;
    CPU_INT16U   tx_pkt_nbr;
    CPU_INT08U   i;
#if 0                                                           /* See Note #3.                                         */
    CPU_INT16U   tx_pkt_pg;
#endif


    LAN91C111_TxPktNbr = LAN91C111_PKT_FRAME_TX_NBR_NONE;


                                                                /* ----------------- REQ TX PKT ALLOC ----------------- */
    reg_val    = LAN91C111_REG_MMU_CMD_TX_ALLOC;
#if 0                                                           /* See Note #3.                                         */
    tx_pkt_pg  = size / LAN91C111_REG_MMD_CMD_TX_PG_SIZE;
    reg_val   |= tx_pkt_pg;
#endif

    LAN91C111_RegWr(LAN91C111_REG_MMU_BANK,
                    LAN91C111_REG_MMU_OFFSET,
                    reg_val);


                                                                /* --------------- WAIT FOR TX PKT ALLOC -------------- */
    i = LAN91C111_TX_ALLOC_INIT_RETRIES;
    do {
        reg_val  = LAN91C111_RegRd(LAN91C111_REG_INT_BANK,
                                   LAN91C111_REG_INT_OFFSET);
        tx_alloc = (reg_val & LAN91C111_REG_INT_TX_ALLOC) ? DEF_YES : DEF_NO;
        i--;
    } while ((tx_alloc == DEF_NO) && (i > 0));                  /* Req tx pkt until alloc'd.                            */

    if (tx_alloc != DEF_YES) {
       *perr = LAN91C111_ERR_TX_ALLOC;
        return;
    }


                                                                /* --------------- CFG TX PKT ALLOC NBR --------------- */
    reg_val              = LAN91C111_RegRd(LAN91C111_REG_PNR_BANK,
                                           LAN91C111_REG_PNR_OFFSET);
    tx_pkt_nbr           = reg_val;
    tx_pkt_nbr          &= LAN91C111_REG_PNR_TX_ALLOC;
    tx_pkt_nbr         >>= LAN91C111_REG_PNR_TX_ALLOC_SHIFT;
    LAN91C111_TxPktNbr   = tx_pkt_nbr;                          /* Get global pkt nbr for ALL tx's.                     */


   *perr                 = NET_NIC_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          LAN91C111_RegRd()
*
* Description : Read data value from LAN91C111 register.
*
* Argument(s) : reg_bank        Register bank select (see Note #1).
*
*               reg_offset      Register address offset.
*
* Return(s)   : Data read from register.
*
* Caller(s)   : Various NetNIC_???()    functions,
*               Various LAN91C111_???() functions.
*
* Note(s)     : (1) 'reg_bank' selects register's specific LAN91C111 register group (see 'LAN91C111 REGISTERS
*                   Note #2').
*
*               (2) Register ALWAYS reads 16-bit data values (see this 'net_nic.c  Note #3a').
*                   (see 'LAN91C111 REGISTERS  Notes').
*********************************************************************************************************
*/

CPU_INT16U  LAN91C111_RegRd (CPU_INT16U  reg_bank,
                             CPU_INT16U  reg_offset)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    CPU_INT16U  val;


    CPU_CRITICAL_ENTER();                                       /* See 'LAN91C111 REGISTERS  Note #5a'.                 */

    if (LAN91C111_RegBankCur != reg_bank) {                     /* If reg offset NOT from cur bank, ...                 */
        LAN91C111_RegBankCur  = reg_bank;
                                                                /* ... switch to reg's bank.                            */
        NetNIC_Wr_16(LAN91C111_REG_BANK_SEL_OFFSET, reg_bank);
    }

    val = NetNIC_Rd_16(reg_offset);

    CPU_CRITICAL_EXIT();

    return (val);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          LAN91C111_RegWr()
*
* Description : Write data value to LAN91C111 register.
*
* Argument(s) : reg_bank        Register bank select (see Note #1).
*
*               reg_offset      Register address offset.
*
*               val             Data to write to register.
*
* Return(s)   : none.
*
* Caller(s)   : Various NetNIC_???()    functions,
*               Various LAN91C111_???() functions.
*
* Note(s)     : (1) 'reg_bank' selects register's specific LAN91C111 register group (see 'LAN91C111 REGISTERS
*                   Note #2').
*
*               (2) Register ALWAYS writes 16-bit data values (see this 'net_nic.c  Note #3a').
*********************************************************************************************************
*/

void  LAN91C111_RegWr (CPU_INT16U  reg_bank, 
                       CPU_INT16U  reg_offset,
                       CPU_INT16U  val)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


    CPU_CRITICAL_ENTER();                                       /* See 'LAN91C111 REGISTERS  Note #5a'.                 */

    if (LAN91C111_RegBankCur != reg_bank) {                     /* If reg offset NOT from cur bank, ...                 */
        LAN91C111_RegBankCur  = reg_bank;
                                                                /* ... switch to reg's bank.                            */
        NetNIC_Wr_16(LAN91C111_REG_BANK_SEL_OFFSET, reg_bank);
    }

    NetNIC_Wr_16(reg_offset, val);

    CPU_CRITICAL_EXIT();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        LAN91C111_PhyRegRd()
*
* Description : (1) Read LAN91C111 Physical Register (see SMSC LAN91C111, Section 9 'PHY Register Description') :
*
* Argument(s) : phy_reg     Physical register to read.
*                               (see this 'net_nic.h  LAN91C111 PHYSICAL (PHY) REGISTERS  PHY REGS').
*
* Return(s)   : Data read from physical register.
*
* Caller(s)   : LAN91C111_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16U  LAN91C111_PhyRegRd (CPU_INT08U  phy_reg)
{
    CPU_INT16U  phy_reg_val;


    LAN91C111_PhyWrIdle();                   /* Send IDLE frame                                        */

    LAN91C111_PhyWrBitLow();                 /* Start <01>                                             */
    LAN91C111_PhyWrBitHigh();

    LAN91C111_PhyWrBitHigh();                /* Read  <10>                                             */
    LAN91C111_PhyWrBitLow();                 

    LAN91C111_PhyWrPhyAddr();                /* Write PHY address <00000> (for internal PHY)           */

    LAN91C111_PhyWrPhyReg(phy_reg);          /* Write PHY register number (5 bits)                     */

    LAN91C111_PhyWrBitZ();                   /* Write turnaround bit (1 bit)                           */

    phy_reg_val = LAN91C111_PhyRdData();     /* Read PHY register data (16 bits)                       */

    LAN91C111_PhyWrBitZ();                   /* Write turnaround bit (1 bit)                           */

    return (phy_reg_val);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        LAN91C111_PhyRegWr()
*
* Description : (1) Write LAN91C111 Physical Register (see SMSC LAN91C111, Section 9 'PHY Register Description') :
*
* Argument(s) : phy_reg     Physical register to write.
*                               (see this 'net_nic.h  LAN91C111 PHYSICAL (PHY) REGISTERS  PHY REGS').
*
*               val         Data to write to physical register.
*
* Return(s)   : none.
*
* Caller(s)   : LAN91C111_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  LAN91C111_PhyRegWr (CPU_INT08U  phy_reg,
                          CPU_INT16U  val)
{
    LAN91C111_PhyWrIdle();                   /* Send IDLE frame                                        */

    LAN91C111_PhyWrBitLow();                 /* Start <01>                                             */
    LAN91C111_PhyWrBitHigh();

    LAN91C111_PhyWrBitLow();                 /* Write <01>                                             */
    LAN91C111_PhyWrBitHigh();

    LAN91C111_PhyWrPhyAddr();                /* Write PHY address <00000> (for internal PHY)           */

    LAN91C111_PhyWrPhyReg(phy_reg);          /* Write PHY register number (5 bits)                     */

    LAN91C111_PhyWrBitHigh();                /* Write turnaround bits <10>                             */
    LAN91C111_PhyWrBitLow();

    LAN91C111_PhyWrData(val);                /* Write PHY register data (16 bits)                      */

    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_DIS + LAN91C111_REG_MGMT_MD_CLK_LO + LAN91C111_REG_MGMT_MD_OUT_LO);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         LAN91C111_PhyRdData()
*
* Description : Read a PHY register
*
* Argument(s) : none
*
* Return(s)   : The current value of the desired PHY register
*********************************************************************************************************
*/

static  CPU_INT16U  LAN91C111_PhyRdData (void)
{
    CPU_INT08U  i;
    CPU_INT16U  reg_data;
    CPU_INT16U  bit;


    reg_data = 0;
    for (i = 0; i < 16; i++) {
        reg_data <<= 1;
        bit        = LAN91C111_PhyRdMDI();
        if (bit == DEF_TRUE) {
            reg_data |= 0x0001;
        }
    }
    return (reg_data);
}

/*
*********************************************************************************************************
*                                         LAN91C111_PhyRdMDI()
*
* Description : Read the MDI input pin on the PHY
*
* Argument(s) : none
*
* Return(s)   : DEF_TRUE    if the pin is high
*               DEF_FALSE   if the pin is low
*********************************************************************************************************
*/

static  CPU_BOOLEAN  LAN91C111_PhyRdMDI (void)
{
    CPU_INT16U  mgmt_val;


    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_DIS + LAN91C111_REG_MGMT_MD_CLK_LO + LAN91C111_REG_MGMT_MD_OUT_LO);

    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_DIS + LAN91C111_REG_MGMT_MD_CLK_HI + LAN91C111_REG_MGMT_MD_OUT_LO);

    mgmt_val = LAN91C111_RegRd(LAN91C111_REG_MGMT_BANK,
                               LAN91C111_REG_MGMT_OFFSET);

    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_DIS + LAN91C111_REG_MGMT_MD_CLK_LO + LAN91C111_REG_MGMT_MD_OUT_LO);

    if (mgmt_val & LAN91C111_REG_MGMT_MD_IN_HI) {
        return (DEF_TRUE);
    } else {
        return (DEF_FALSE);
    }
}

/*
*********************************************************************************************************
*                                       LAN91C111_PhyWrData()
*
* Description : Writes to a PHY register
*
* Argument(s) : reg_data     is the data to write to the PHY register
*
* Return(s)   : none.
*
* Caller(s)   : 
*********************************************************************************************************
*/

static  void  LAN91C111_PhyWrData (CPU_INT16U  reg_data)
{
    CPU_INT08U  i;


    for (i = 0; i < 16; i++) {
        if (reg_data & 0x8000) {
            LAN91C111_PhyWrBitHigh();
        } else {
            LAN91C111_PhyWrBitLow();
        }
        reg_data <<= 1;
    }
}

/*
*********************************************************************************************************
*                                       LAN91C111_PhyWrIdle()
*
* Description : Writes 32 idle bits to the PHY
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : 
*********************************************************************************************************
*/

static  void  LAN91C111_PhyWrIdle (void)
{
    CPU_INT08U  i;


    for (i = 0; i < 32; i++) {
        LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                        LAN91C111_REG_MGMT_OFFSET,
                        0x3330 + LAN91C111_REG_MGMT_MD_OUT_EN + LAN91C111_REG_MGMT_MD_CLK_LO + LAN91C111_REG_MGMT_MD_OUT_HI);

        LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                        LAN91C111_REG_MGMT_OFFSET,
                        0x3330 + LAN91C111_REG_MGMT_MD_OUT_EN + LAN91C111_REG_MGMT_MD_CLK_HI + LAN91C111_REG_MGMT_MD_OUT_HI);
    }
}

/*
*********************************************************************************************************
*                                       LAN91C111_PhyWrPhyAddr()
*
* Description : Writes the PHY address (assumes 0x0000) to the PHY
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : 
*********************************************************************************************************
*/

static  void  LAN91C111_PhyWrPhyAddr (void)
{
    LAN91C111_PhyWrBitLow();
    LAN91C111_PhyWrBitLow();
    LAN91C111_PhyWrBitLow();
    LAN91C111_PhyWrBitLow();
    LAN91C111_PhyWrBitLow();
}

/*
*********************************************************************************************************
*                                       LAN91C111_PhyWrPhyReg()
*
* Description : Writes the PHY register address to the PHY
*
* Argument(s) : reg     is the register number
*
* Return(s)   : none.
*
* Caller(s)   : 
*********************************************************************************************************
*/

static  void  LAN91C111_PhyWrPhyReg (CPU_INT08U  reg)
{
    CPU_INT08U  i;


    for (i = 0; i < 5; i++) {
        if (reg & 0x10) {
            LAN91C111_PhyWrBitHigh();
        } else {
            LAN91C111_PhyWrBitLow();
        }
        reg <<= 1;
    }
}

/*
*********************************************************************************************************
*                                       LAN91C111_PhyWrBitLow()
*
* Description : Writes a LOW to the PHY serial bus
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : 
*********************************************************************************************************
*/

static  void  LAN91C111_PhyWrBitLow (void)
{
    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_EN + LAN91C111_REG_MGMT_MD_CLK_LO + LAN91C111_REG_MGMT_MD_OUT_LO);

    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_EN + LAN91C111_REG_MGMT_MD_CLK_HI + LAN91C111_REG_MGMT_MD_OUT_LO);

    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_EN + LAN91C111_REG_MGMT_MD_CLK_LO + LAN91C111_REG_MGMT_MD_OUT_LO);
}

/*
*********************************************************************************************************
*                                       LAN91C111_PhyWrBitHigh()
*
* Description : Writes a HIGH to the PHY serial bus
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : 
*********************************************************************************************************
*/

static  void  LAN91C111_PhyWrBitHigh (void)
{
    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_EN + LAN91C111_REG_MGMT_MD_CLK_LO + LAN91C111_REG_MGMT_MD_OUT_HI);

    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_EN + LAN91C111_REG_MGMT_MD_CLK_HI + LAN91C111_REG_MGMT_MD_OUT_HI);

    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_EN + LAN91C111_REG_MGMT_MD_CLK_LO + LAN91C111_REG_MGMT_MD_OUT_HI);
}

/*
*********************************************************************************************************
*                                       LAN91C111_PhyWrBitZ()
*
* Description : Places the data bit in high-Z mode
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : 
*********************************************************************************************************
*/

static  void  LAN91C111_PhyWrBitZ (void)
{
    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_DIS + LAN91C111_REG_MGMT_MD_CLK_LO + LAN91C111_REG_MGMT_MD_OUT_LO);

    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_DIS + LAN91C111_REG_MGMT_MD_CLK_HI + LAN91C111_REG_MGMT_MD_OUT_LO);

    LAN91C111_RegWr(LAN91C111_REG_MGMT_BANK,
                    LAN91C111_REG_MGMT_OFFSET,
                    0x3330 + LAN91C111_REG_MGMT_MD_OUT_DIS + LAN91C111_REG_MGMT_MD_CLK_LO + LAN91C111_REG_MGMT_MD_OUT_LO);
}
