/*
*********************************************************************************************************
*                                               uC/HTTPs
*                                 HyperText Transfer Protocol (server)
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
*                                              HTTP SERVER
*
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename      : http-s_cfg.h
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

#define  HTTPs_OS_CFG_TASK_NAME                 "HTTP (Server)"


/*
*********************************************************************************************************
*                                           TASKS PRIORITIES
*********************************************************************************************************
*/

#define  HTTPs_OS_CFG_TASK_PRIO                           13


/*
*********************************************************************************************************
*                                              STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  HTTPs_OS_CFG_TASK_STK_SIZE                     2048


/*
*********************************************************************************************************
*                                                 HTTPs
*********************************************************************************************************
*/

#define  HTTPs_CFG_IPPORT                                 80    /* HTTP server port. Default is 80.                     */

#define  HTTPs_CFG_MAX_ACCEPT_TIMEOUT_S                   -1    /* Maximum inactivity time (s) on ACCEPT.               */
#define  HTTPs_CFG_MAX_RX_TIMEOUT_S                        5    /* Maximum inactivity time (s) on RX.                   */
#define  HTTPs_CFG_MAX_TX_TIMEOUT_S                        5    /* Maximum inactivity time (s) on TX.                   */

#define  HTTPs_CFG_MAX_ACCEPT_RETRY                       -1    /* Maximum number of retries on ACCEPT.                 */
#define  HTTPs_CFG_MAX_RX_RETRY                            3    /* Maximum number of retries on RX.                     */
#define  HTTPs_CFG_MAX_TX_RETRY                            3    /* Maximum number of retries on TX.                     */

                                                                /* Root path for HTTP documents in filesystem.          */
#define  HTTPs_CFG_FS_ROOT                      "/HTTPRoot"     /* Filesystem-specific symbols can be used.             */
#define  HTTPs_CFG_DFLT_FILE                    "index.htm"     /* Default file to load if no filename specified in URL.*/

                                                                /* Maximum variable length in HTML pages for dynamic    */
#define  HTTPs_CFG_MAX_VAR_LEN                           255    /* replacement (GET) or for POST requests.              */
                                                                /* Maximum value length in HTML pages for dynamic       */
#define  HTTPs_CFG_MAX_VAL_LEN                           255    /* replacement (GET) or for POST requests.              */

                                                                /* Default HTML document returned when the requested    */
                                                                /* HTML document is not found (HTTP error #404).        */
#define  HTTPs_CFG_ERR_MSG_HTML_NOT_FOUND       "<HTML>\r\n" \
                                                "<BODY>\r\n" \
                                                "<HEAD><TITLE>SYSTEM ERROR</TITLE></HEAD>\r\n" \
                                                "<H1>NOT FOUND</H1>\r\n" \
                                                "The requested object does not exist on this server.\r\n" \
                                                "</BODY>\r\n" \
                                                "</HTML>\r\n"


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DBG                                   2

#define  HTTPs_TRACE_LEVEL                      TRACE_LEVEL_INFO
#define  HTTPs_TRACE                            printf
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
