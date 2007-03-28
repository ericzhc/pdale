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
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename      : ttcp_cfg.h
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

#define  TTCP_OS_CFG_TASK_NAME                  "TTCP"


/*
*********************************************************************************************************
*                                           TASKS PRIORITIES
*********************************************************************************************************
*/

#define  TTCP_OS_CFG_TASK_PRIO                            16


/*
*********************************************************************************************************
*                                              STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  TTCP_OS_CFG_TASK_STK_SIZE                      1024


/*
*********************************************************************************************************
*                                                 TTCP
*********************************************************************************************************
*/

#define  TTCP_CFG_MAX_ACCEPT_TIMEOUT_S                     5    /* Maximum inactivity time (s) on ACCEPT.               */
#define  TTCP_CFG_MAX_CONN_TIMEOUT_S                       5    /* Maximum inactivity time (s) on CONNECT.              */
#define  TTCP_CFG_MAX_RX_TIMEOUT_S                         5    /* Maximum inactivity time (s) on RX.                   */
#define  TTCP_CFG_MAX_TX_TIMEOUT_S                         5    /* Maximum inactivity time (s) on TX.                   */

#define  TTCP_CFG_MAX_ACCEPT_RETRY                         3    /* Maximum number of retries on ACCEPT.                 */
#define  TTCP_CFG_MAX_CONN_RETRY                           3    /* Maximum number of retries on CONNECT.                */
#define  TTCP_CFG_MAX_RX_RETRY                             3    /* Maximum number of retries on RX.                     */
#define  TTCP_CFG_MAX_TX_RETRY                             3    /* Maximum number of retries on TX.                     */


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DBG                                   2

#define  TTCP_TRACE_LEVEL                       TRACE_LEVEL_INFO
#define  TTCP_TRACE                             printf
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
