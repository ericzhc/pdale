/*
*********************************************************************************************************
*                                               uC/TFTPs
*                                Trivial File Transfer Protocol (server)
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
*                                              TFTP SERVER
*
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename      : tftp-s_cfg.h
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

#define  TFTPs_OS_CFG_TASK_NAME                 "TFTP (Server)"


/*
*********************************************************************************************************
*                                           TASKS PRIORITIES
*********************************************************************************************************
*/

#define  TFTPs_OS_CFG_TASK_PRIO                           20


/*
*********************************************************************************************************
*                                              STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  TFTPs_OS_CFG_TASK_STK_SIZE                      512


/*
*********************************************************************************************************
*                                                 TFTPs
*********************************************************************************************************
*/

#define  TFTPs_CFG_IPPORT                                 69    /* TFTP server IP port.  Default is 69.                 */

#define  TFTPs_CFG_MAX_RX_TIMEOUT_S                        5    /* Maximum inactivity time (s) on RX.                   */
#define  TFTPs_CFG_MAX_TX_TIMEOUT_S                        5    /* Maximum inactivity time (s) on TX.                   */


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DBG                                   2

#define  TFTPs_TRACE_LEVEL                      TRACE_LEVEL_OFF
#define  TFTPs_TRACE                            printf
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
