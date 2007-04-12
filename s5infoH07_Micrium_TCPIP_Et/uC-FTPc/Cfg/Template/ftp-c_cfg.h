/*
*********************************************************************************************************
*                                                uC/FTPc
*                                    File Transfer Protocol (client)
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
*                                              FTP CLIENT
*
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename      : ftp-c_cfg.h
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 FTPc
*********************************************************************************************************
*/

#define  FTPc_CFG_CTRL_IPPORT                             21    /* FTP Control IP port. Default is 21.                  */
#define  FTPc_CFG_DTP_IPPORT                              20    /* FTP Data IP port.    Default is 20.                  */

#define  FTPc_CFG_CTRL_MAX_CONN_TIMEOUT_S                  5    /* Maximum inactivity time (s) on CONNECT.              */
#define  FTPc_CFG_CTRL_MAX_RX_TIMEOUT_S                    5    /* Maximum inactivity time (s) on RX.                   */
#define  FTPc_CFG_CTRL_MAX_TX_TIMEOUT_S                    5    /* Maximum inactivity time (s) on TX.                   */

#define  FTPc_CFG_DTP_MAX_ACCEPT_TIMEOUT_S                 5    /* Maximum inactivity time (s) on ACCEPT.               */
#define  FTPc_CFG_DTP_MAX_CONN_TIMEOUT_S                   5    /* Maximum inactivity time (s) on CONNECT.              */
#define  FTPc_CFG_DTP_MAX_RX_TIMEOUT_S                     5    /* Maximum inactivity time (s) on RX.                   */
#define  FTPc_CFG_DTP_MAX_TX_TIMEOUT_S                     5    /* Maximum inactivity time (s) on TX.                   */

#define  FTPc_CFG_CTRL_MAX_CONN_RETRY                      3    /* Maximum number of retries on CONNECT.                */
#define  FTPc_CFG_CTRL_MAX_RX_RETRY                        3    /* Maximum number of retries on RX.                     */
#define  FTPc_CFG_CTRL_MAX_TX_RETRY                        3    /* Maximum number of retries on TX.                     */

#define  FTPc_CFG_DTP_MAX_ACCEPT_RETRY                     3    /* Maximum number of retries on ACCEPT.                 */
#define  FTPc_CFG_DTP_MAX_CONN_RETRY                       3    /* Maximum number of retries on CONNECT.                */
#define  FTPc_CFG_DTP_MAX_RX_RETRY                         3    /* Maximum number of retries on RX.                     */
#define  FTPc_CFG_DTP_MAX_TX_RETRY                         3    /* Maximum number of retries on TX.                     */

#define  FTPc_CFG_MAX_USER_LEN                            32    /* Maximum length for user name.                        */
#define  FTPc_CFG_MAX_PASS_LEN                            32    /* Maximum length for password.                         */

#define  FTPc_CFG_USE_FS                                   1    /* If 1, interface functions using FS are enabled.      */
                                                                /* If 0, only functions using RAM buffers are enabled.  */


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DBG                                   2

#define  FTPc_TRACE_LEVEL                       TRACE_LEVEL_INFO
#define  FTPc_TRACE                             printf
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
