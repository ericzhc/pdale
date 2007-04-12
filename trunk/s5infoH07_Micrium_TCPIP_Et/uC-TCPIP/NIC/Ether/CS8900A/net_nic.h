/* net_nic.h for CS8900 ethernet driver and MPC565 processor
 * Autonomous Solutions, Inc.
 */


#ifdef   NET_NIC_MODULE
#define  NET_NIC_EXT
#else
#define  NET_NIC_EXT  extern
#endif

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
NET_NIC_EXT  CPU_INT16U   CS8900_RegBankCur;                 /* Maintain state of cur reg bank.                      */

NET_NIC_EXT  CPU_INT16U   CS8900_TxPktNbr;                   /* Maintain one global pkt frame for ALL tx's.          */



void         NetNIC_Init             (void);

void         NetNIC_IntEn            (void);

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


void         NetOS_NIC_Init          (void);                    /* Create Drv objs & start Drv tasks.                   */


void         NetOS_NIC_TxRdyWait     (NET_ERR      *perr);      /* Wait on drv tx rdy signal from NIC.                  */

void         NetOS_NIC_TxRdySignal   (void);                    /* Post    drv tx rdy signal from NIC.                  */

                                                                /* See this 'net_nic.h  Notes #3b & #3c'.               */
CPU_INT16U   NetNIC_Rd16             (CPU_INT16U    reg_offset);

void         NetNIC_Wr16             (CPU_INT16U    reg_offset,
                                      CPU_INT16U    val);

void NetNIC_ISR (unsigned char Level);

#if (NET_NIC_CFG_INT_CTRL_EN == DEF_ENABLED)
void         NetNIC_IntInit          (void);                    /* Init int ctrl'r        [see Note #1].                */

void         NetNIC_IntClr           (void);                    /* Clr  int ctrl'r src(s) [see Note #2].                */
#endif

#ifndef  NET_NIC_CFG_INT_CTRL_EN
#error   NET_NIC_CFG_INT_CTRL_EN           not #define'd in 'net_cfg.h'
#error                               [MUST be  DEF_DISABLED]
#error                               [     ||  DEF_ENABLED ]
#elif  ((NET_NIC_CFG_INT_CTRL_EN != DEF_DISABLED) && \
        (NET_NIC_CFG_INT_CTRL_EN != DEF_ENABLED ))
#error   NET_NIC_CFG_INT_CTRL_EN     illegally #define'd in 'net_cfg.h'
#error                               [MUST be  DEF_DISABLED]
#error                               [     ||  DEF_ENABLED ]
#endif


void        NetNIC_RxISR_Handler     (void);                    /* Rx ISR signals Net Drv Task of NIC rx pkts.  */
void        NetNIC_TxISR_Handler     (void);                    /* Tx ISR signals Net Drv Tx of empty tx FIFO.  */
// CS8900 functions

void CS8900_Init(void);
void CS8900_Shutdown(void);
void CS8900_PwrCtrl(CPU_BOOLEAN pwr);
void CS8900_WaitForReset(void);
void CS8900_RxIntEn(void);
void CS8900_TxIntEn(void);
static CPU_INT16U CS8900_RxPktGetSize(void);
static void CS8900_RxPkt(void *ppkt, CPU_INT16U size);
static void CS8900_RxPktDiscard(CPU_INT16U size);
static void CS8900_TxEn(void);
void CS8900_RxIntDis(void);
static void CS8900_TxPkt(void *ppkt, CPU_INT16U size, NET_ERR *perr);
unsigned short CS8900_ReadFrame(void);
void CS8900_WriteFrame(unsigned short Data);
void CS8900_CopyToFrame(void *Source, unsigned short Size);
