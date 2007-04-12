/*
*********************************************************************************************************
*                                                uC/FTPs
*                                    File Transfer Protocol (server)
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
*                                              FTP SERVER
*
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename      : ftp-s_cfg.h
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

#define  FTPs_SRV_OS_CFG_TASK_NAME              "FTP (Server)"
#define  FTPs_CTRL_OS_CFG_TASK_NAME             "FTP (Control)"


/*
*********************************************************************************************************
*                                           TASKS PRIORITIES
*********************************************************************************************************
*/

#define  FTPs_SRV_OS_CFG_TASK_PRIO                        14
#define  FTPs_CTRL_OS_CFG_TASK_PRIO                       15


/*
*********************************************************************************************************
*                                              STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  FTPs_SRV_OS_CFG_TASK_STK_SIZE                  1024
#define  FTPs_CTRL_OS_CFG_TASK_STK_SIZE                 4096


/*
*********************************************************************************************************
*                                                 FTPs
*********************************************************************************************************
*/

#define  FTPs_CFG_CTRL_IPPORT                             21    /* FTP Control Port. Default is 21.                     */
#define  FTPs_CFG_DTP_IPPORT                              20    /* FTP Data Port.    Default is 20.                     */

#define  FTPs_CFG_CTRL_MAX_ACCEPT_TIMEOUT_S               -1    /* Maximum inactivity time (s) on ACCEPT.               */
#define  FTPs_CFG_CTRL_MAX_RX_TIMEOUT_S                   30    /* Maximum inactivity time (s) on RX.                   */
#define  FTPs_CFG_CTRL_MAX_TX_TIMEOUT_S                    5    /* Maximum inactivity time (s) on TX.                   */

#define  FTPs_CFG_DTP_MAX_ACCEPT_TIMEOUT_S                 5    /* Maximum inactivity time (s) on ACCEPT.               */
#define  FTPs_CFG_DTP_MAX_CONN_TIMEOUT_S                   5    /* Maximum inactivity time (s) on CONNECT.              */
#define  FTPs_CFG_DTP_MAX_RX_TIMEOUT_S                     5    /* Maximum inactivity time (s) on RX.                   */
#define  FTPs_CFG_DTP_MAX_TX_TIMEOUT_S                     5    /* Maximum inactivity time (s) on TX.                   */

#define  FTPs_CFG_CTRL_MAX_ACCEPT_RETRY                   -1    /* Maximum number of retries on ACCEPT.                 */
#define  FTPs_CFG_CTRL_MAX_RX_RETRY                        3    /* Maximum number of retries on RX.                     */
#define  FTPs_CFG_CTRL_MAX_TX_RETRY                        3    /* Maximum number of retries on TX.                     */

#define  FTPs_CFG_DTP_MAX_ACCEPT_RETRY                     3    /* Maximum number of retries on ACCEPT.                 */
#define  FTPs_CFG_DTP_MAX_CONN_RETRY                       3    /* Maximum number of retries on CONNECT.                */
#define  FTPs_CFG_DTP_MAX_RX_RETRY                         3    /* Maximum number of retries on RX.                     */
#define  FTPs_CFG_DTP_MAX_TX_RETRY                         3    /* Maximum number of retries on TX.                     */

#define  FTPs_CFG_MAX_USER_LEN                            32    /* Maximum length for user name.                        */
#define  FTPs_CFG_MAX_PASS_LEN                            32    /* Maximum length for password.                         */


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DBG                                   2

#define  FTPs_TRACE_LEVEL                       TRACE_LEVEL_INFO
#define  FTPs_TRACE                             printf
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
