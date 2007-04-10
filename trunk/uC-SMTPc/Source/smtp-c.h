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
* Filename      : smtp-c.h
* Version       : V1.02
* Programmer(s) : SR
*                 JDH
*********************************************************************************************************
*
* This code implements a subset of the SMTP protocol (RFC 2821).  More precisely, the following
* commands have been implemented:
*   HELO
*   MAIL
*   RCPT
*   DATA
*   RSET
*   NOOP
*   QUIT
*
*********************************************************************************************************
*/

#ifndef  __SMTP_C_H__
#define  __SMTP_C_H__


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <lib_str.h>
#include  <smtp-c_ADT.h>


/*
*********************************************************************************************************
*                                              ERROR CODES
*********************************************************************************************************
*/

#define  SMTPc_ERR_BASE_NBR                                                       51000

#define  SMTPc_ERR_NONE                         (NET_ERR)(                            0)
#define  SMTPc_ERR_SOCK_OPEN_FAILED             (NET_ERR)(SMTPc_ERR_BASE_NBR +        1)
#define  SMTPc_ERR_SOCK_CONN_FAILED             (NET_ERR)(SMTPc_ERR_BASE_NBR +        2)
#define  SMTPc_ERR_NO_SMTP_SERV                 (NET_ERR)(SMTPc_ERR_BASE_NBR +        3)
#define  SMTPc_ERR_RX_FAILED                    (NET_ERR)(SMTPc_ERR_BASE_NBR +        4)
#define  SMTPc_ERR_TX_FAILED                    (NET_ERR)(SMTPc_ERR_BASE_NBR +        5)
#define  SMTPc_ERR_NULL_ARG                     (NET_ERR)(SMTPc_ERR_BASE_NBR +        6)
#define  SMTPc_ERR_LINE_TOO_LONG                (NET_ERR)(SMTPc_ERR_BASE_NBR +        7)
#define  SMTPc_ERR_REP                          (NET_ERR)(SMTPc_ERR_BASE_NBR +        8)

#define  SMTPc_ERR_REP_POS                      (NET_ERR)(SMTPc_ERR_NONE               )
#define  SMTPc_ERR_REP_INTER                    (NET_ERR)(SMTPc_ERR_BASE_NBR +       10)
#define  SMTPc_ERR_REP_NEG                      (NET_ERR)(SMTPc_ERR_BASE_NBR +       11)
#define  SMTPc_ERR_REP_TOO_SHORT                (NET_ERR)(SMTPc_ERR_BASE_NBR +       12)


/*
*********************************************************************************************************
*                                              DEFINITIONS
*********************************************************************************************************
*/

#define  SMTPc_LINE_LEN_LIM                             1000    /* Line length limit, as defined in RFC #2822.          */
                                                                /* SMTPc_CFG_COMM_BUF_LEN is the length of the buffer   */
                                                                /* used to receive replies from the SMTP server.        */
                                                                /* As stated in RFC #2821, Section 'The SMTP            */
                                                                /* Specifications, Additional Implementation Issues,    */
                                                                /* Sizes and Timeouts, Size limits and minimums'        */
                                                                /* "The maximum total length of a reply code and the    */
                                                                /* <CRLF> is 512 characters".                           */

#define  SMTPc_COMM_BUF_LEN                             1024    /* This buffer is also used to build outgoing messages  */
                                                                /* and SHOULD NOT be smaller than 1000.                 */


/*
*********************************************************************************************************
*                                            SERVER REPLIES
*
* Note(s): (1) Reply codes are defined here and classified by category.  Note that this list is far
*              from being exhaustive.
*********************************************************************************************************
*/

                                                                /* ------------ POSITIVE PRELIMINARY REPLY ------------ */
#define  SMTPc_REP_POS_PRELIM_GRP                          1

                                                                /* ------------ POSITIVE COMPLETION REPLY ------------- */
#define  SMTPc_REP_POS_COMPLET_GRP                         2
#define  SMTPc_REP_220                                   220    /* Service ready                                        */
#define  SMTPc_REP_221                                   221    /* System status, or system help ReplyMessage           */
#define  SMTPc_REP_250                                   250    /* Requested mail action okay, completed                */
#define  SMTPc_REP_251                                   251    /* User not local; will forward to <forward-path>       */

                                                                /* ------------ POSITIVE INTERMEDIATE REPLY ----------- */
#define  SMTPc_REP_POS_INTER_GRP                           3
#define  SMTPc_REP_354                                   354    /* Start mail input; end with <CRLF>.<CRLF>             */

                                                                /* -------- TRANSIENT NEGATIVE COMPLETION REPLY ------- */
#define  SMTPc_REP_NEG_TRANS_COMPLET_GRP                   4

                                                                /* -------- PERMANENT NEGATIVE COMPLETION REPLY ------- */
#define  SMTPc_REP_NEG_COMPLET_GRP                         5
#define  SMTPc_REP_503                                   503    /* Bad sequence of commands                             */
#define  SMTPc_REP_504                                   504    /* Command parameter not implemented                    */
#define  SMTPc_REP_550                                   550    /* Requested action not taken: mailbox unavailable      */
#define  SMTPc_REP_554                                   554    /* Transaction failed                                   */


/*
*********************************************************************************************************
*                                         COMMANDS AND STRINGS
*********************************************************************************************************
*/

#define  SMTPc_CMD_HELO                         "HELO"
#define  SMTPc_CMD_MAIL                         "MAIL"
#define  SMTPc_CMD_RCPT                         "RCPT"
#define  SMTPc_CMD_DATA                         "DATA"
#define  SMTPc_CMD_RSET                         "RSET"
#define  SMTPc_CMD_NOOP                         "NOOP"
#define  SMTPc_CMD_QUIT                         "QUIT"

#define  SMTPc_CRLF                             "\x0D\x0A"
#define  SMTPc_EOM                              "\x0D\x0A.\x0D\x0A"

#define  SMTPc_HDR_FROM                         "From: "
#define  SMTPc_HDR_SENDER                       "Sender: "
#define  SMTPc_HDR_TO                           "To: "
#define  SMTPc_HDR_REPLYTO                      "Reply-to: "
#define  SMTPc_HDR_CC                           "Cc: "
#define  SMTPc_HDR_SUBJECT                      "Subject: "


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                                /* Establish a TCP connection to the SMTP server        */
                                                                /* and initiate client session                          */
NET_SOCK_ID  SMTPc_Connect         (NET_IP_ADDR   ip_server,
                                    CPU_INT16U    port,
                                    NET_IP_ADDR   client_addr,
                                    CPU_BOOLEAN   init_extended,
                                    NET_ERR      *perr);

                                                                /* Send the message (instance of SMTPc_MSG structure to */
                                                                /* the SMTP server                                      */
void         SMTPc_SendMsg         (NET_SOCK_ID   sock,
                                    SMTPc_MSG    *msg,
                                    NET_ERR      *perr);

                                                                /* Issue the QUIT command and close the TCP connection  */
void         SMTPc_Disconnect      (NET_SOCK_ID   sock,
                                    NET_ERR      *perr);


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                        DEFINED IN smtp-c_ADT.c
*********************************************************************************************************
*/

                                                                /* Populate a SMTPc_MBOX structure                       */
void         SMTPcADT_SetSMTPcMbox (SMTPc_MBOX   *mbox,
                                    CPU_CHAR     *name,
                                    CPU_CHAR     *addr,
                                    NET_ERR      *perr);

                                                                /* Initialize a SMTPc_MSG structure                     */
void         SMTPcADT_InitSMTPcMsg (SMTPc_MSG    *msg,
                                    SMTPc_MBOX   *from,
                                    SMTPc_MBOX   *sender,
                                    NET_ERR      *perr);


/*
*********************************************************************************************************
*                                              TRACING
*********************************************************************************************************
*/

                                                                /* Trace level, default to TRACE_LEVEL_OFF              */
#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                                 0
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                                1
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                                 2
#endif

#ifndef  SMTPc_TRACE_LEVEL
#define  SMTPc_TRACE_LEVEL                      TRACE_LEVEL_OFF
#endif

#ifndef  SMTPc_TRACE
#define  SMTPc_TRACE                            printf
#endif

#define  SMTPc_TRACE_INFO(x)                  ((SMTPc_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(SMTPc_TRACE x) : (void)0)
#define  SMTPc_TRACE_DBG(x)                   ((SMTPc_TRACE_LEVEL >= TRACE_LEVEL_DBG)  ? (void)(SMTPc_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/


                                                                /* Define the SMTP client IP port.  Default is 25.      */
#ifndef  SMTPc_CFG_IPPORT
#error  "SMTPc_CFG_IPPORT                       illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'              "
#endif

                                                                /* Maximum inactivity time (s) on CONNECT.              */
#ifndef  SMTPc_CFG_MAX_CONN_TIMEOUT_S
#error  "SMTPc_CFG_MAX_CONN_TIMEOUT_S           illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'              "
#endif

                                                                /* Maximum inactivity time (s) on RX.                   */
#ifndef  SMTPc_CFG_MAX_RX_TIMEOUT_S
#error  "SMTPc_CFG_MAX_RX_TIMEOUT_S             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'              "
#endif

                                                                /* Maximum inactivity time (s) on TX.                   */
#ifndef  SMTPc_CFG_MAX_TX_TIMEOUT_S
#error  "SMTPc_CFG_MAX_TX_TIMEOUT_S             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'              "
#endif

                                                                /* Maximum number of retries on CONNECT.                */
#ifndef  SMTPc_CFG_MAX_CONN_RETRY
#error  "SMTPc_CFG_MAX_CONN_RETRY               illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'                "
#endif

                                                                /* Maximum number of retries on RX.                     */
#ifndef  SMTPc_CFG_MAX_RX_RETRY
#error  "SMTPc_CFG_MAX_RX_RETRY                 illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'                "
#endif

                                                                /* Maximum number of retries on TX.                     */
#ifndef  SMTPc_CFG_MAX_TX_RETRY
#error  "SMTPc_CFG_MAX_TX_RETRY                 illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'                "
#endif

                                                                /* Maximum length of sender's name.                     */
#ifndef  SMTPc_CFG_MBOX_NAME_DISP_LEN
#error  "SMTPc_CFG_MBOX_NAME_DISP_LEN           illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'                "
#endif

                                                                /* Maximum length of message subject.                   */
#ifndef  SMTPc_CFG_MSG_SUBJECT_LEN
#error  "SMTPc_CFG_MSG_SUBJECT_LEN              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'                "
#endif

                                                                /* Maximum number of TO recipients.                     */
#ifndef  SMTPc_CFG_MSG_MAX_TO
#error  "SMTPc_CFG_MSG_MAX_TO                   illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'                "
#endif

                                                                /* Maximum number of CC  recipients.                    */
#ifndef  SMTPc_CFG_MSG_MAX_CC
#error  "SMTPc_CFG_MSG_MAX_CC                   illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'                "
#endif

                                                                /* Maximum number of BCC recipients.                    */
#ifndef  SMTPc_CFG_MSG_MAX_BCC
#error  "SMTPc_CFG_MSG_MAX_BCC                  illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'                "
#endif

                                                                /* Maximum number of message attachments.               */
#ifndef  SMTPc_CFG_MSG_MAX_ATTACH
#error  "SMTPc_CFG_MSG_MAX_ATTACH               illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'smtp-c_cfg.h'                "
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif  /* __SMTP_C_H__ */
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
