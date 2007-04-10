/*
*********************************************************************************************************
*                                               uC/SNTPc
*                                 Simple Network Time Protocol (client)
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
*                                              SNTP CLIENT
*
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename     : sntp-c_cfg.h
* Version      : V1.85
* Programmer(s): JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 SNTPc
*********************************************************************************************************
*/

#define  SNTPc_CFG_IPPORT                                123    /* SNTP client port. Default is 123.                    */

#define  SNTPc_CFG_MAX_RX_TIMEOUT_S                        5    /* Maximum inactivity time (s) on RX.                   */
#define  SNTPc_CFG_MAX_TX_TIMEOUT_S                        5    /* Maximum inactivity time (s) on TX.                   */


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DBG                                   2

#define  SNTPc_TRACE_LEVEL                      TRACE_LEVEL_INFO
#define  SNTPc_TRACE                            printf
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
