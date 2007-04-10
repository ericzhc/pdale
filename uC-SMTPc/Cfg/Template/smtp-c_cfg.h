/*
*********************************************************************************************************
*                                               uC/SMTPc
*                                Simple Mail Transfert Protocol (client)
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
*                                              SMTP CLIENT
*
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename      : smtp-c_cfg.h
* Version       : V1.01
* Programmer(s) : SR
*                 JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 SMTPc
*
* Note(s) : (1) Default TCP port to use when calling SMTPc_Connect() without specifying a port
*               to connect to.  Standard listening port for SMTP servers is 25.
*
*           (2) Corresponds to the maximum length of the displayed name associated with a mailbox,
*               including '\0'.  This length MUST be smaller than 600 in order to respect the same
*               limit.
*
*           (3) SMTPc_CFG_MSG_SUBJECT_LEN is the maximum length of the string containing the message subject,
*               including '\0'.  The length MUST be smaller than 900 characters in order to respect the
*               Internet Message Format line limit.
*
*           (4) Maximum length of the various arrays inside the SMTPc_MSG structure.
*********************************************************************************************************
*/

#define  SMTPc_CFG_IPPORT                                 25    /* SMTP client IP port. Default is 25. See note #1.     */

#define  SMTPc_CFG_MAX_CONN_TIMEOUT_S                      5    /* Maximum inactivity time (s) on CONNECT.              */
#define  SMTPc_CFG_MAX_RX_TIMEOUT_S                        5    /* Maximum inactivity time (s) on RX.                   */
#define  SMTPc_CFG_MAX_TX_TIMEOUT_S                        5    /* Maximum inactivity time (s) on TX.                   */

#define  SMTPc_CFG_MAX_CONN_RETRY                          3    /* Maximum number of retries on CONNECT.                */
#define  SMTPc_CFG_MAX_RX_RETRY                            3    /* Maximum number of retries on RX.                     */
#define  SMTPc_CFG_MAX_TX_RETRY                            3    /* Maximum number of retries on TX.                     */

#define  SMTPc_CFG_MBOX_NAME_DISP_LEN                     50    /* Maximum length of sender's name.       See note #2.  */
#define  SMTPc_CFG_MSG_SUBJECT_LEN                        50    /* Maximum length of message subject.     See note #3.  */

#define  SMTPc_CFG_MSG_MAX_TO                              5    /* Maximum number of TO recipients.       See note #4.  */
#define  SMTPc_CFG_MSG_MAX_CC                              5    /* Maximum number of CC  recipients.      See note #4.  */
#define  SMTPc_CFG_MSG_MAX_BCC                             5    /* Maximum number of BCC recipients.      See note #4.  */
#define  SMTPc_CFG_MSG_MAX_ATTACH                          5    /* Maximum number of message attachments. See note #4.  */


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DBG                                   2

#define  SMTPc_TRACE_LEVEL                      TRACE_LEVEL_INFO
#define  SMTPc_TRACE                            printf
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
