/*
*********************************************************************************************************
*                                                uC/TTCP
*                                  TCP-IP Transfer Measurement Utility
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                  TCP-IP TRANSFER MEASUREMENT UTILITY
*
* Filename      : ttcp.h
* Version       : V1.86
* Programmer(s) : CL
*                 JDH
*                 ITJ
*********************************************************************************************************
* Note(s)       : This application is a Micrium implementation of a publically available TCP test tool: TTCP.
*                 This implementation uses uC/OS-II and uC/TCP-IP.
*                 The tool was adapted to Micrium uC/TCP-IP and coding standards.
**********************************************************************************************************
*/

#ifndef  __TTCP_H__
#define  __TTCP_H__


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

CPU_BOOLEAN  TTCP_Init (void);
void         TTCP_Task (void  *p_arg);


/*
*********************************************************************************************************
*                                       RTOS INTERFACE FUNCTIONS
*                                            (see ttcp_os.c)
*********************************************************************************************************
*/

CPU_BOOLEAN  TTCP_OS_Init (void  *p_arg);


/*
*********************************************************************************************************
*                                              TRACING
*********************************************************************************************************
*/

                                                                /* Trace level, default to TRACE_LEVEL_OFF              */
#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                                 0
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                                1
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                                 2
#endif

#ifndef  TTCP_TRACE_LEVEL
#define  TTCP_TRACE_LEVEL                       TRACE_LEVEL_OFF
#endif

#ifndef  TTCP_TRACE
#define  TTCP_TRACE                             printf
#endif

#define  TTCP_TRACE_INFO(x)                   ((TTCP_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(TTCP_TRACE x) : (void)0)
#define  TTCP_TRACE_DBG(x)                    ((TTCP_TRACE_LEVEL >= TRACE_LEVEL_DBG)  ? (void)(TTCP_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                /* Task name for debugging purposes.                    */
#ifndef  TTCP_OS_CFG_TASK_NAME
#error  "TTCP_OS_CFG_TASK_NAME                  illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif

                                                                /* Task priority.                                       */
#ifndef  TTCP_OS_CFG_TASK_PRIO
#error  "TTCP_OS_CFG_TASK_PRIO                  illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif

                                                                /* Task stack size.                                     */
#ifndef  TTCP_OS_CFG_TASK_STK_SIZE
#error  "TTCP_OS_CFG_TASK_STK_SIZE              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif

                                                                /* Maximum inactivity time (s) on ACCEPT.               */
#ifndef  TTCP_CFG_MAX_ACCEPT_TIMEOUT_S
#error  "TTCP_CFG_MAX_ACCEPT_TIMEOUT_S          illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif

                                                                /* Maximum inactivity time (s) on CONNECT.              */
#ifndef  TTCP_CFG_MAX_CONN_TIMEOUT_S
#error  "TTCP_CFG_MAX_CONN_TIMEOUT_S            illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif

                                                                /* Maximum inactivity time (s) on RX.                   */
#ifndef  TTCP_CFG_MAX_RX_TIMEOUT_S
#error  "TTCP_CFG_MAX_RX_TIMEOUT_S              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif

                                                                /* Maximum inactivity time (s) on TX.                   */
#ifndef  TTCP_CFG_MAX_TX_TIMEOUT_S
#error  "TTCP_CFG_MAX_TX_TIMEOUT_S              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif

                                                                /* Maximum number of retries on ACCEPT.                 */
#ifndef  TTCP_CFG_MAX_ACCEPT_RETRY
#error  "TTCP_CFG_MAX_ACCEPT_RETRY              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif

                                                                /* Maximum number of retries on CONNECT.                */
#ifndef  TTCP_CFG_MAX_CONN_RETRY
#error  "TTCP_CFG_MAX_CONN_RETRY                illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif

                                                                /* Maximum number of retries on RX.                     */
#ifndef  TTCP_CFG_MAX_RX_RETRY
#error  "TTCP_CFG_MAX_RX_RETRY                  illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif

                                                                /* Maximum number of retries on TX.                     */
#ifndef  TTCP_CFG_MAX_TX_RETRY
#error  "TTCP_CFG_MAX_TX_RETRY                  illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ttcp_cfg.h'                "
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif  /* __TTCP_H__ */
