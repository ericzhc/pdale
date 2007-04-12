/*
*********************************************************************************************************
*                                               uC/DHCPc
*                             Dynamic Host Configuration Protocol (client)
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
*                                              DHCP CLIENT
*
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename      : dhcp-c.c
* Version       : V1.85
* Programmer(s) : JJL
*                 JDH
*                 SR
*********************************************************************************************************


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

#define  DHCPc_OS_CFG_TASK_NAME               "DHCP (Client)"


/*
*********************************************************************************************************
*                                           TASKS PRIORITIES
*********************************************************************************************************
*/

#define  DHCPc_OS_CFG_TASK_PRIO                           13


/*
*********************************************************************************************************
*                                              STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  DHCPc_OS_CFG_TASK_STK_SIZE                      256


/*
*********************************************************************************************************
*                                                 DHCPc
*********************************************************************************************************
*/

                                                                /* DHCP server IP port. Default is 67.                  */
#define  DHCPc_CFG_IP_PORT_SERVER                         67
                                                                /* DHCP client IP port. Default is 68.                  */
#define  DHCPc_CFG_IP_PORT_CLIENT                         68

                                                                /* Maximum number of attempts to negotiate lease with   */
                                                                /* the DHCP server.                                     */
#define  DHCPc_CFG_MAX_REQ_LEASE_RETRY                     5

                                                                /* The broadcast bit is used to instruct the server to  */
                                                                /* use broadcast packets instead of unicast.            */
                                                                /* Useful when the IP stack cannot receive unicast      */
                                                                /* packets when not fully configured.                   */
                                                                /* This is our case, so the value MUST be DEF_ENABLED.  */
#define  DHCPc_CFG_BROADCAST_BIT                 DEF_ENABLED


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DBG                                   2

#define  DHCPc_TRACE_LEVEL                   TRACE_LEVEL_OFF
#define  DHCPc_TRACE                                  printf
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
