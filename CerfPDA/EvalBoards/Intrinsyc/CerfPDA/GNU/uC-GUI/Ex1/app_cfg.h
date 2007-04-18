 /*
*********************************************************************************************************
*                                    APPLICATION SPECIFIC RTOS SETUP
*
*                             (c) Copyright 2005, Micrium, Inc., Weston, FL
*                                          All Rights Reserved
*
*                                          CONFIGURATION FILE
*
* File : app_os_cfg.h
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#define  LIB_STR_CFG_FP_EN                      DEF_ENABLED

#define  VIC_VECT_OS_DMA                           0
#define  VIC_VECT_OS_TICK                          1
#define  VIC_VECT_NIC                              2
#define  VIC_VECT_OS_UART1                         3
#define  VIC_VECT_OS_UART2                         4

#define	 DEBUG  1

/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

// #define  HTTPS_OS_TASK_NAME                     "HTTP (Server)"              /* defined in http-s_cfg.h                  */


/*
*********************************************************************************************************
*                                           TASKS PRIORITIES
*********************************************************************************************************
*/

#define TASK_START_APP_PRIO						   5
#define TASK_RFSERIAL_PRIO						   6
#define TASK_RFTASK_PRIO						   7

#define TASK_SERIAL_PRIO						   8

#define TASK_GPS_PRIO							   10
#define TASK_GPS_SEND_PRIO						  11
#define TASK_BCR_PRIO							  12
#define TASK_MAP_PRIO							  15
#define TASK_GUI_PRIO							   19 

//#define  MSTREAMRX_TASK_PRIO                      11
//#define  SIPEVNT_PIP                              12
//#define  SIPTRANSACT_PIO                          14
//#define  SIP_MAIN_TASK_PRIO                       16
//#define  UA_TASK_PRIO                             18
//#define  TU_TASK_PRIO                             20
//// #define  HTTPs_OS_TASK_PRIO                       19             /* defined in http-s_cfg.h                  */
//
//// #define  DHCPc_OS_CFG_TASK_PRIO                   18             /* defined in http-s_cfg.h                  */
//#define  SIP_RXMSSG_TASK_PRIO                     22                /* SIP transaction FSM timer adjust         */
//#define  TRANSACT_TASK_PRIO                       26                /* SIP transaction FSM                      */
//#define  TFTPs_OS_TASK_PRIO                       28
//#define  TMR_CTRL_TASK_PRIO                       30                /* sip timer control                        */
//#define  OS_TASK_TMR_PRIO                         33
//#define  SIPTRANSPORT_TASK_PRIO                   34
//#define  MINITERM_TASK_PRIO                       38                /* marka: must be a low level priority task */
//#define  NET_OS_CFG_IF_RX_TASK_PRIO               50
//#define  NET_OS_CFG_TMR_TASK_PRIO                 51
//

 
/*
*********************************************************************************************************
*                                              STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  SIP_MAIN_TASK_STK_SIZE                 4096
#define  MINITERM_TASK_STK_SIZE                 2048
#define  MEDIASTREAMMNG_TASK_STK_SIZE           4096
#define  MSTREAMRX_TASK_STK_SIZE                2048
#define  SIP_RXMSSG_TASK_STK_SIZE               2048
#define  TRANSACT_TASK_STK_SIZE                 2048
#define  TMR_CTRL_TASK_STK_SIZE                 1024
#define  TU_TASK_STK_SIZE                       1024
#define  UA_TASK_STK_SIZE                       1024
#define  SIPTRANSPORT_TASK_STK_SIZE             1024

#define  APP_START_TASK_STK_SIZE                1024
//#define  HTTPs_OS_TASK_STK_SIZE                 2048                      /* defined in http-s_cfg.h                  */
#define  TFTPs_OS_TASK_STK_SIZE                 1024
#define  APP_1_TASK_STK_SIZE                    1024

#define  NET_OS_CFG_TMR_TASK_STK_SIZE           1024
#define  NET_OS_CFG_IF_RX_TASK_STK_SIZE         1024

#define TASK_SERIAL_SIZE						1024

/*
*********************************************************************************************************
*                                              DHCP 
*                             
*********************************************************************************************************
*/

#define DHCP_CFG_BROADCAST_BIT                  DEF_ENABLED


/*
*********************************************************************************************************
*                                                 DNSc
*********************************************************************************************************
*/

#define  DNSc_CFG_IPPORT                                   53    /* Client IP port. Default is 53.                       */

#define  DNSc_CFG_MAX_RX_TIMEOUT_S                         5    /* Maximum inactivity time (s) on RX.                   */
#define  DNSc_CFG_MAX_TX_TIMEOUT_S                         5    /* Maximum inactivity time (s) on TX.                   */

