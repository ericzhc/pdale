/*
*********************************************************************************************************
*                                               uC/POP3c
*                                    Post Office Protocol 3 (client)
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
*                                              POP3 CLIENT
*
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename      : pop3-c_cfg.h
* Version       : V1.02
* Programmer(s) : SR
*                 JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 POP3c
*
* Note(s) : (1) Default TCP port to use when calling POP3c_Connect() without specifying a port
*               to connect to.  Standard listening port for POP3 servers is 110.
*
*********************************************************************************************************
*/

#define  POP3c_CFG_IPPORT                                110    /* POP3 client IP port. Default is 110. See note #1.    */

#define  POP3c_CFG_MAX_CONN_TIMEOUT_S                      5    /* Maximum inactivity time (s) on CONNECT.              */
#define  POP3c_CFG_MAX_RX_TIMEOUT_S                        5    /* Maximum inactivity time (s) on RX.                   */
#define  POP3c_CFG_MAX_TX_TIMEOUT_S                        5    /* Maximum inactivity time (s) on TX.                   */

#define  POP3c_CFG_MAX_CONN_RETRY                          3    /* Maximum number of retries on CONNECT.                */
#define  POP3c_CFG_MAX_RX_RETRY                            3    /* Maximum number of retries on RX.                     */
#define  POP3c_CFG_MAX_TX_RETRY                            3    /* Maximum number of retries on TX.                     */


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DBG                                   2

#define  POP3c_TRACE_LEVEL                      TRACE_LEVEL_INFO
#define  POP3c_TRACE                            printf
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
