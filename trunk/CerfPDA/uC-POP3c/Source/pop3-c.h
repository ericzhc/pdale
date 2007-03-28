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
* Filename      : pop3-c.h
* Version       : V1.02
* Programmer(s) : SR
*                 JDH
*********************************************************************************************************
*
* This code implements a subset of the POP3 client protocol (RFC 1939).  More precisely, the following
* commands have been implemented:
*   USER
*   PASS
*   QUIT
*   STAT
*   LIST
*   RETR
*   DELE
*   NOOP
*   RSET
*
*********************************************************************************************************
*/

#ifndef  __POP3_C_H__
#define  __POP3_C_H__


/*
*********************************************************************************************************
*                                                DEFINES
*********************************************************************************************************
*/

#define  POP3c_TAB                                      0x09
#define  POP3c_SP                                       0x20
#define  POP3c_LF                                       0x0A
#define  POP3c_CR                                       0x0D


/*
*********************************************************************************************************
*                                              ERROR CODES
*********************************************************************************************************
*/

#define  POP3c_ERR_BASE_NBR                                           50000

#define  POP3c_ERR_NONE                                                   0
#define  POP3c_ERR_SOCK_OPEN_FAILED             (POP3c_ERR_BASE_NBR +     1)
#define  POP3c_ERR_SOCK_CONN_FAILED             (POP3c_ERR_BASE_NBR +     2)
#define  POP3c_ERR_AUTHENTICATE                 (POP3c_ERR_BASE_NBR +     3)
#define  POP3c_ERR_TX_FAILED                    (POP3c_ERR_BASE_NBR +     4)
#define  POP3c_ERR_RX_FAILED                    (POP3c_ERR_BASE_NBR +     5)
#define  POP3c_ERR_NEG_RESP                     (POP3c_ERR_BASE_NBR +     6)
#define  POP3c_ERR_ARG_TOO_LONG                 (POP3c_ERR_BASE_NBR +     7)
#define  POP3c_ERR_DEST_BUF_TOO_SMALL           (POP3c_ERR_BASE_NBR +     8)
#define  POP3c_ERR_INCOMPLETE_MSG               (POP3c_ERR_BASE_NBR +     9)


/*
*********************************************************************************************************
*                                                 STUFF
*
* Note(s)     : (1) POP3c_RESP_BUF_LEN is the length of the buffer used to receive all responses from the
*                   POP3 server.  As stated in RFC #1939, Section 'Basic Operation' "Responses may be up to
*                   512 characters long, including the terminating CRLF.".
*
*                   This buffer length should therefore be at least 512 + 1 bytes long in order to make room
*                   for the '\0' character.  However, since it is used to receive messages, setting it to a
*                   bigger size than the minimum could improve performances by minimizing the number of call
*                   to NetSock_RxData().
*
*               (2) Note that RFC 2449 (POP3 Extension Mechanism) allows command up to 255 octets.
*
*               (3) As stated in RFC #1939, Section 'Basic Operation', "Each argument may be up to 40
*                   characters long".
*
*               (4) As mentionned in RFC #2821, Section 'The SMTP Specifications, Additionnal
*                   Implementation Issues, Sizes and Timeouts, Size limits and minimums', "The maximum
*                   total length of a user name or other local-part is 64 characters [and] the maximum
*                   total length of a domain name or number is 255 characters.".  Adding 1 for
*                   termination character appended to array (see POP3_MSG structure).
*
*                   Since the address fields are not parsed to extract the actual address in function
*                   MessageRead(), more room has to be allowed in order to provide space for additionnal
*                   information present in those fields.  See RFC #2822 (Internet Message Format),
*                   Section 'Syntax, Address Specification' for more information.
*
*               (5) Size of array containing the origination date and subject of a message received
*                   (see POP3_MSG structure).  If subject and/or date for a particular message exceed
*                   this length limit, only the first POP3c_SIZE_[x] -1 characters will be copied.
*********************************************************************************************************
*/

                                                                /* See note #1.                                         */
#define  POP3c_RESP_BUF_LEN                         (998 + 3)

                                                                /* The command (4 characters) + CRLF.                   */
                                                                /* See note #2.                                         */
#define  POP3c_COMMAND_MIN_LEN                             6

                                                                /* See note #3.                                         */
#define  POP3c_ARGUMENT_MAX_LEN                           40
                                                                /* Space between every argument.                        */
#define  POP3c_ARGUMENT_ADDITIONNAL_LEN                    1

                                                                /* See note #4.                                         */
#define  POP3c_ADDR_DOMAIN_NAME_MAX_LEN                  255
#define  POP3c_ADDR_LOCAL_PART_MAX_LEN                    64
#define  POP3c_ADDR_DISP_NAME_SIZE                       100
#define  POP3c_MSG_ADDR_SIZE                   (POP3c_ADDR_DOMAIN_NAME_MAX_LEN +    \
                                                POP3c_ADDR_LOCAL_PART_MAX_LEN  +    \
                                                POP3c_ADDR_DISP_NAME_SIZE      + 2)

                                                                /* See note #5.                                         */
#define  POP3c_MSG_DATE_SIZE                              38
#define  POP3c_MSG_SUBJECT_SIZE                          100


/*
*********************************************************************************************************
*                                  STATUS INDICATOR AND HEADER FIELDS
*********************************************************************************************************
*/

#define  POP3c_LEN_POS_STAT_INDICATOR                      3

#define  POP3c_POS_STATUS_INDICATOR             "+OK"
#define  POP3c_NEG_STATUS_INDICATOR             "-ERR"

#define  POP3c_MSG_HDR_FIELDS_QTY                          5
#define  POP3c_HDR_FROM                         "From:"
#define  POP3c_HDR_DATE                         "Date:"
#define  POP3c_HDR_SENDER                       "Sender:"
#define  POP3c_HDR_REPLYTO                      "Reply-to:"
#define  POP3c_HDR_SUBJECT                      "Subject:"

#define  POP3c_HDR_FROM_LEN                                5
#define  POP3c_HDR_DATE_LEN                                5
#define  POP3c_HDR_SENDER_LEN                              7
#define  POP3c_HDR_REPLYTO_LEN                             9
#define  POP3c_HDR_SUBJECT_LEN                             8


/*
*********************************************************************************************************
*                                               FSM STATES
*
* Note(s)     : (1) See function POP3c_RespServerMulti() for details on this state machine.
*********************************************************************************************************
*/

#define  POP3c_STATUS_PROCESSING                           1
#define  POP3c_HDR_PROCESSING                              2
#define  POP3c_DATA_PROCESSING                             3
#define  POP3c_CR_FOUND                                    4
#define  POP3c_CRLF_FOUND                                  5
#define  POP3c_CRLFCR_FOUND                                6
#define  POP3c_CRLFDOT_FOUND                               7
#define  POP3c_CRLFDOTCR_FOUND                             8


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

                                                                /* This section defines the format of messages          */
                                                                /* received by the pop3-c module.                       */
typedef struct pop3c_msg
{
    CPU_CHAR   From[POP3c_MSG_ADDR_SIZE];                       /* Message originator                                   */
    CPU_CHAR   Date[POP3c_MSG_DATE_SIZE];                       /* Origination date                                     */
    CPU_CHAR   Sender[POP3c_MSG_ADDR_SIZE];                     /* Sender of message                                    */
    CPU_CHAR   Reply_to[POP3c_MSG_ADDR_SIZE];                   /* Reply-to address                                     */
    CPU_CHAR   Subject[POP3c_MSG_SUBJECT_SIZE];                 /* Message subject                                      */
    CPU_CHAR  *Body;                                            /* Pointer to message body                              */
} POP3c_MSG;


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                                /* Establish a TCP connection to the POP3 server.       */
NET_SOCK_ID  POP3c_Connect           (NET_IP_ADDR   ip_server,
                                      CPU_INT16U    port,
                                      NET_ERR      *perr);

                                                                /* Log the user into the system.                        */
void         POP3c_Authenticate      (NET_SOCK_ID   sock,
                                      CPU_CHAR     *username,
                                      CPU_CHAR     *pswd,
                                      NET_ERR      *perr);

                                                                /* Get message quantity and mailbox size from server.   */
void         POP3c_MboxStat          (NET_SOCK_ID   sock,
                                      CPU_INT32U   *msg_qty,
                                      CPU_INT32U   *mbox_size,
                                      NET_ERR      *perr);

                                                                /* Get specific message size from server.               */
void         POP3c_MsgStat           (NET_SOCK_ID   sock,
                                      CPU_INT32U    msg_nbr,
                                      CPU_INT32U   *msg_size,
                                      NET_ERR      *perr);

                                                                /* Retrieve the message specified by the index and      */
                                                                /* copy it into the buffer pointed by dest_buf.         */
                                                                /* If the message is larger than buf_size, it might     */
                                                                /* cause runtime problem.                               */
void         POP3c_MsgRetrieve       (NET_SOCK_ID   sock,
                                      CPU_INT32U    msg_nbr,
                                      CPU_CHAR     *dest_buf,
                                      CPU_INT32U    buf_size,
                                      CPU_BOOLEAN   del_msg,
                                      NET_ERR      *perr);

                                                                /* Read a message by filling POP3_MSG structure.        */
void         POP3c_MsgRead           (CPU_CHAR     *msg_buf,
                                      CPU_INT32U    buf_size,
                                      POP3c_MSG    *msg,
                                      NET_ERR      *perr);

                                                                /* Delete the message specified by the index from the   */
                                                                /* server.                                              */
void         POP3c_MsgDel            (NET_SOCK_ID   sock,
                                      CPU_INT32U    msg_nbr,
                                      NET_ERR      *perr);

                                                                /* Issue the QUIT command which updates the mailbox.    */
                                                                /* Once this is done, terminate the TCP connection.     */
void         POP3c_Disconnect        (NET_SOCK_ID   sock,
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

#ifndef  POP3c_TRACE_LEVEL
#define  POP3c_TRACE_LEVEL                      TRACE_LEVEL_OFF
#endif

#ifndef  POP3c_TRACE
#define  POP3c_TRACE                            printf
#endif

#define  POP3c_TRACE_INFO(x)                  ((POP3c_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(POP3c_TRACE x) : (void)0)
#define  POP3c_TRACE_DBG(x)                   ((POP3c_TRACE_LEVEL >= TRACE_LEVEL_DBG)  ? (void)(POP3c_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                /* Define the POP3 client IP port.  Default is 110.     */
#ifndef  POP3c_CFG_IPPORT
#error  "POP3c_CFG_IPPORT                       illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'pop3-c_cfg.h'              "
#endif

                                                                /* Maximum inactivity time (s) on CONNECT.              */
#ifndef  POP3c_CFG_MAX_CONN_TIMEOUT_S
#error  "POP3c_CFG_MAX_CONN_TIMEOUT_S           illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'pop3-c_cfg.h'              "
#endif

                                                                /* Maximum inactivity time (s) on RX.                   */
#ifndef  POP3c_CFG_MAX_RX_TIMEOUT_S
#error  "POP3c_CFG_MAX_RX_TIMEOUT_S             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'pop3-c_cfg.h'              "
#endif

                                                                /* Maximum inactivity time (s) on TX.                   */
#ifndef  POP3c_CFG_MAX_TX_TIMEOUT_S
#error  "POP3c_CFG_MAX_TX_TIMEOUT_S             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'pop3-c_cfg.h'              "
#endif

                                                                /* Maximum number of retries on CONNECT.                */
#ifndef  POP3c_CFG_MAX_CONN_RETRY
#error  "POP3c_CFG_MAX_CONN_RETRY               illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'pop3-c_cfg.h'                "
#endif

                                                                /* Maximum number of retries on RX.                     */
#ifndef  POP3c_CFG_MAX_RX_RETRY
#error  "POP3c_CFG_MAX_RX_RETRY                 illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'pop3-c_cfg.h'                "
#endif

                                                                /* Maximum number of retries on TX.                     */
#ifndef  POP3c_CFG_MAX_TX_RETRY
#error  "POP3c_CFG_MAX_TX_RETRY                 illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'pop3-c_cfg.h'                "
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif  /* __POP3_C_H__ */
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
