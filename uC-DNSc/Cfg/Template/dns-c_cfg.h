/*
*********************************************************************************************************
*                                                uC/DNSc
*                                      Domain Name Server (client)
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
*                                              DNS CLIENT
*
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename      : dns-c_cfg.h
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 DNSc
*********************************************************************************************************
*/

#define  DNSc_IPPORT                                      53    /* Client IP port. Default is 53.                       */

#define  DNSc_CFG_MAX_RX_TIMEOUT_S                         5    /* Maximum inactivity time (s) on RX.                   */
#define  DNSc_CFG_MAX_TX_TIMEOUT_S                         5    /* Maximum inactivity time (s) on TX.                   */


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DEBUG                                 2

#define  DNSc_TRACE_LEVEL                       TRACE_LEVEL_INFO
#define  DNSc_TRACE                             printf
