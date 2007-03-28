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
* Filename      : dhcp-c.h
* Version       : V1.85
* Programmer(s) : JJL
*                 JDH
*                 SR
*********************************************************************************************************
* Based upon the RFC 2131 specifications, this client supports both infinite address lease and temporary
* ones, with automatic renewal of lease if necessary.
*
* Sequence of events for this implementation...
*   Client issues a DHCP_DISCOVER, server responds with a DHCP_OFFER,
*   Client issues a DHCP_REQUEST,  server responds with a DHCP_ACK.
*
* DISCOVER:
*   Request by the client to broadcast the fact that it is looking for a DHCP server.
*
* OFFER:
*   Reply from the server when it receives a DISCOVER request from a client.  The offer may contain all
*   the information that the DHCP client needs to bootup, but this is dependent on the configuration of
*   the server.
*
* REQUEST:
*   Request by the client for the server (now known because an OFFER was received) to send it the
*   information it needs.
*
* ACK:
*   Reply from the server with the information requested.
*********************************************************************************************************
*/

#ifndef  __DHCP_C_H__
#define  __DHCP_C_H__


/*
*********************************************************************************************************
*                                              ERROR CODES
*********************************************************************************************************
*/

#define  DHCPc_ERR_NONE                                    0
#define  DHCPc_ERR_SOCK_FAIL                               1
#define  DHCPc_ERR_TX_FAIL                                 2
#define  DHCPc_ERR_RX_FAIL                                 3
#define  DHCPc_ERR_RX_NACK                                 4
#define  DHCPc_ERR_GIVING_UP                               5
#define  DHCPc_ERR_MAC_ADDR_NOT_SET                        6
#define  DHCPc_ERR_CFG_STACK_FAILED                        7
#define  DHCPc_ERR_TASK_INIT                               8
#define  DHCPc_ERR_SIGNAL                                  9
#define  DHCPc_ERR_WAIT                                   10
#define  DHCPc_ERR_NO_SUCH_TMR                            11
#define  DHCPc_ERR_TMR                                    12


/*
*********************************************************************************************************
*                                              TIME VALUES
*********************************************************************************************************
*/

#define   DHCPc_TIME_T1                                    1
#define   DHCPc_TIME_T2                                    2
#define   DHCPc_TIME_LEASE_TIME                            3


/*
*********************************************************************************************************
*                                                 STUFF
*********************************************************************************************************
*/

#define  DHCPc_PKT_BUF_SIZE                              576    /* Buffer size   defined in RFC 2131, p. 10.            */

                                                                /* Magic  cookie defined in RFC 2132, p. 13.            */
#define  DHCP_STD_MAGIC_COOKIE                    0x63825363    /* 99.130.83.99                                         */


#define  DHCP_INFINITE_LEASE_TIME                 0xFFFFFFFF    /* Permanent lease time                                 */

                                                                /* Fraction of lease duration for T1 (0.5)              */
#define  DHCP_T1_FRACTION_NUM                              1
#define  DHCP_T1_FRACTION_DEN                              2

                                                                /* Fraction of lease duration for T2 (0.875)            */
#define  DHCP_T2_FRACTION_NUM                              7
#define  DHCP_T2_FRACTION_DEN                              8

                                                                /* Minimum retransmitting time for DHCPREQUEST in RENEW */
                                                                /* and REBIND states (see RFC 2131, section 4.4.5       */
#define  DHCP_MIN_RETRANSMITTING_TIME                     60

                                                                /* This section defines the possible states for DHCP    */
                                                                /* clients, illustrated in RFC 2131 Section 4.4.1       */
typedef enum {
    DHCP_STATE_INIT,
    DHCP_STATE_SELECT,
    DHCP_STATE_REQUEST,
    DHCP_STATE_BOUND,
    DHCP_STATE_RENEW,
    DHCP_STATE_REBIND,
    DHCP_STATE_REBOOT,
    DHCP_STATE_INIT_REBOOT
} DHCP_STATES;


/*
*********************************************************************************************************
*                                            OPERATION CODES
*********************************************************************************************************
*/

                                                                /* This section defines the DHCP operation codes as     */
                                                                /* defined in RFC 2131, p. 10.                          */
#define  DHCP_OP_REQUEST                                   1
#define  DHCP_OP_REPLY                                     2


/*
*********************************************************************************************************
*                                                 FLAGS
*********************************************************************************************************
*/

                                                                /* This section defines the DHCP flags as defined in    */
                                                                /* RFC 2131, p. 10.                                     */

                                                                /* The broadcast bit is used to instruct the server to  */
                                                                /* use broadcast packets instead of unicast.            */
                                                                /* Useful when the IP stack cannot receive unicast      */
                                                                /* packets when not fully configured.                   */
#define  DHCP_FLAG_BROADCAST                        (1 << 15)


/*
*********************************************************************************************************
*                                             MESSAGE TYPES
*********************************************************************************************************
*/

                                                                /* This section defines the DHCP message types as       */
                                                                /* defined in RFC 2132, p. 27.                          */
#define  DHCP_MSG_DISCOVER                                 1
#define  DHCP_MSG_OFFER                                    2
#define  DHCP_MSG_REQUEST                                  3
#define  DHCP_MSG_DECLINE                                  4
#define  DHCP_MSG_ACK                                      5
#define  DHCP_MSG_NACK                                     6
#define  DHCP_MSG_RELEASE                                  7
#define  DHCP_MSG_INFORM                                   8


/*
*********************************************************************************************************
*                                             DHCP OPTIONS
*********************************************************************************************************
*
* This section defines the DHCP operation codes as defined in RFC 2131, p. 10.
*
* The DHCP options (sometimes called 'vendor options') are variable-length data fields appended to the
* original fixed-length data BOOTP header (defined by the DHCP_HDR structure).
*
* In the received DHCP packets, the BOOTP header is inserted at position 0 and have a length of 236 bytes.
* After the BOOTP header, the server MAY insert DHCP options.  They are recognized by the magic cookie,
* which is a 32-bit value.  After the magic cookie, the options begins, formatted as follows:
*
*    Code   Len   Data
*   +-----+-----+-----+-----+-----+-----+-----+-----+--
*   |  c  |  n  |  d1 |  d2 |  d3 |  d4 |  d5 |  d6 |  ...
*   +-----+-----+-----+-----+-----+-----+-----+-----+--
*
* First, we have the code, which represents the option number, defined later.  After, we have the length
* of the data payload, which can be fixed, variable-length of a variable multiple of a fixed length.
*
* The latter format is, for example, used when the server sent a list of DNS server. This option will be
* coded like this:
*
*   Data (prev.)   Code   Len   Data                                           Code (next option)
*   --+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+--
*     |     |     |  6  |  8  | 192 | 168 |   0 |   1 | 192 | 168 |   0 |   2 |  c  |
*   --+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+--
*
* Here, we have the Option Number 6 (DHCP_OPT_DOMAIN_NAME_SERVER), followed by a length of 8 (2 32-bit
* IP addresse), followed by the 2 name servers IP addresses themselves.
*
* For each option, I have placed in comments the legal value(s) of length of option data.
*
*********************************************************************************************************
*/

                                                                /* OPTION NUMBER  OPTION SIZE (payload only)            */
#define  DHCP_OPT_PAD                                              0    /*  0                                           */

                                                                /* RFC 1497 Vendor Extensions                           */
#define  DHCP_OPT_SUBNET_MASK                                      1    /*  4                                           */
#define  DHCP_OPT_TIME_OFFSET                                      2    /*  4                                           */
#define  DHCP_OPT_ROUTER                                           3    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_TIME_SERVER                                      4    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_NAME_SERVER                                      5    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_DOMAIN_NAME_SERVER                               6    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_LOG_SERVER                                       7    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_COOKIE_SERVER                                    8    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_LPR_SERVER                                       9    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_IMPRESS_SERVER                                  10    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_RESSOURCE_LOCATION_SERVER                       11    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_HOST_NAME                                       12    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_BOOT_FILE_SIZE                                  13    /*  2                                           */
#define  DHCP_OPT_MERIT_DUMP_FILE                                 14    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_DOMAIN_NAME                                     15    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_SWAP_SERVER                                     16    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_ROOT_PATH                                       17    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_EXTENSION_PATH                                  18    /*  n,     1 <= n <= 255                        */

                                                                /* IP Layer Parameters per Host                         */
#define  DHCP_OPT_IP_FORWARDING                                   19    /*  1 (value = 0/1)                             */
#define  DHCP_OPT_NON_LOCAL_SOURCE_ROUTING                        20    /*  1 (value = 0/1)                             */
#define  DHCP_OPT_POLICY_FILTER                                   21    /*  n * 8, 1 <= n <= 255                        */
#define  DHCP_OPT_MAXIMUM_DATAGRAM_REASSEMBLY_SIZE                22    /*  2                                           */
#define  DHCP_OPT_DEFAULT_IP_TIME_TO_LIVE                         23    /*  1                                           */
#define  DHCP_OPT_PATH_MTU_AGING_TIMEOUT                          24    /*  4                                           */
#define  DHCP_OPT_PATH_MTU_PLATEAU_TABLE                          25    /*  n * 2, 1 <= n <= 255                        */

                                                                /* IP Layer Parameters per Interface                    */
#define  DHCP_OPT_INTERFACE_MTU                                   26    /*  2                                           */
#define  DHCP_OPT_ALL_SUBNETS_ARE_LOCAL                           27    /*  1 (value = 0/1)                             */
#define  DHCP_OPT_BROADCAST_ADDRESS                               28    /*  4                                           */
#define  DHCP_OPT_PERFORM_MASK_DISCOVERY                          29    /*  1 (value = 0/1)                             */
#define  DHCP_OPT_MASK_SUPPLIER                                   30    /*  1 (value = 0/1)                             */
#define  DHCP_OPT_PERFORM_ROUTER_DISCOVERY                        31    /*  1 (value = 0/1)                             */
#define  DHCP_OPT_ROUTER_SOLLICITATION_ADDRESS                    32    /*  4                                           */
#define  DHCP_OPT_STATIC_ROUTE                                    33    /*  n * 8, 1 <= n <= 255                        */

                                                                /* Link Layer Parameters per Interface                  */
#define  DHCP_OPT_TRAILER ENCAPSULATION                           34    /*  1 (value = 0/1)                             */
#define  DHCP_OPT_ARP_CACHE_TIMEOUT                               35    /*  4                                           */
#define  DHCP_OPT_ETHERNET ENCAPSULATION                          36    /*  1 (value = 0/1)                             */

                                                                /* TCP Parameters                                       */
#define  DHCP_OPT_TCP_DEFAULT_TTL                                 37    /*  1                                           */
#define  DHCP_OPT_TCP_KEEPALIVE_INTERVAL                          38    /*  4                                           */
#define  DHCP_OPT_TCP_KEEPALIVE_GARBAGE                           39    /*  1 (value = 0/1)                             */

                                                                /* Application and Service Parameters                   */
#define  DHCP_OPT_NETWORK_INFORMATION_SERVICE_DOMAIN              40    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_NETWORK_INFORMATION_SERVER                      41    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_NETWORK_TIME_PROTOCOL_SERVER                    42    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_VENDOR_SPECIFIC_INFORMATION                     43    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_NETBIOS_OVER_TCPIP_NAME_SERVER                  44    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_NETBIOS_OVER_TCPIP_DATAGRAM_DISTRIBUTION_SERVER 45    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_NETBIOS_OVER_TCPIP_NODE_TYPE                    46    /*  1                                           */
#define  DHCP_OPT_NETBIOS_OVER_TCPIP_SCOPE                        47    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_X_WINDOW_SYSTEM_FONT_SERVER                     48    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_X_WINDOW_SYSTEM_DISPLAY_MANAGER                 49    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_NETWORK_INFORMATION_SERVICE_PLUS_DOMAIN         64    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_NETWORK_INFORMATION_SERVICE_PLUS_SERVER         65    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_MOBILE_IP_HOME_AGENT                            68    /*  n * 4, 0 <= n <= 255                        */
#define  DHCP_OPT_SIMPLE_MAIL_TRANSPORT_PROTOCOL_SERVER           69    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_POST_OFFICE_PROTOCOL_SERVER                     70    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_NETWORK_NEWS_TRANSPORT_PROTOCOL_SERVER          71    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_DEFAULT_WORLD_WIDE_WEB_SERVER                   72    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_DEFAULT_FINGER_SERVER                           73    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_DEFAULT_INTERNET_RELAY_CHAT_SERVER              74    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_STREETTALK_SERVER                               75    /*  n * 4, 1 <= n <= 255                        */
#define  DHCP_OPT_STREETTALK_DIRECTORY_ASSISTANCE_SERVER          76    /*  n * 4, 1 <= n <= 255                        */

                                                                /* DHCP Extensions                                      */
#define  DHCP_OPT_REQUESTED_IP_ADDRESS                            50    /*  4                                           */
#define  DHCP_OPT_IP_ADDRESS_LEASE_TIME                           51    /*  4                                           */
#define  DHCP_OPT_OPTION_OVERLOAD                                 52    /*  1 (value = 1/2/3)                           */
                                                                        /*    (See 'MESSAGE TYPES')                     */
#define  DHCP_OPT_DHCP_MESSAGE_TYPE                               53    /*  1 (value = 1-9)                             */
#define  DHCP_OPT_SERVER_IDENTIFIER                               54    /*  4                                           */
#define  DHCP_OPT_PARAMETER_REQUEST_LIST                          55    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_ERROR_MESSAGE                                   56    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_MAXIMUM_DHCP_MESSAGE_SIZE                       57    /*  2                                           */
#define  DHCP_OPT_RENEWAL_TIME_VALUE                              58    /*  4                                           */
#define  DHCP_OPT_REBINDING_TIME_VALUE                            59    /*  4                                           */
#define  DHCP_OPT_VENDOR_CLASS_IDENTIFIER                         60    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_CLIENT_IDENTIFIER                               61    /*  n,     2 <= n <= 255                        */
#define  DHCP_OPT_TFTP_SERVER_NAME                                66    /*  n,     1 <= n <= 255                        */
#define  DHCP_OPT_BOOTFILE_NAME                                   67    /*  n,     1 <= n <= 255                        */

#define  DHCP_OPT_END                                            255    /*  0                                           */


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

                                                                /* This section defines the format of DHCP messages as  */
                                                                /* defined in RFC 2131, p. 9.                           */
typedef  struct {
    CPU_INT08U  op;                                             /* Operation code.                                      */
    CPU_INT08U  htype;                                          /* Hardware address type. Set to '1'   (Ethernet).      */
    CPU_INT08U  hlen;                                           /* Hardware address length. Set to '6' (Ethernet).      */
    CPU_INT08U  hops;                                           /* Client sets to zero, optionally used by relay agents.*/
    CPU_INT32U  xid;                                            /* Transaction ID, a random number chosen by the client.*/
    CPU_INT16U  secs;                                           /* Filled in by client, seconds elapsed since client    */
                                                                /* began address acquisition.                           */
    CPU_INT16U  flags;                                          /* See 'FLAGS' section.                                 */
    CPU_INT32U  ciaddr;                                         /* Client IP address, only filled in if client is in    */
                                                                /* BOUND, RENEW or REBINDING state.                     */
    CPU_INT32U  yiaddr;                                         /* New IP address, returned by the server.              */
    CPU_INT32U  siaddr;                                         /* IP address of next server to use in bootstrap.       */
    CPU_INT32U  giaddr;                                         /* Relay agent IP address, used in booting via a relay  */
                                                                /* agent.                                               */
    CPU_INT08U  chaddr[16];                                     /* Client hardware address.                             */
    CPU_INT08U  sname[64];                                      /* Optional server host name, null terminated string.   */
    CPU_INT08U  file[128];                                      /* Optional boot file name, null terminated string.     */

                                                                /* DHCP options starts here in the DHCP packets.        */
} DHCP_HDR;

#define  DHCP_HDR_SIZE  (sizeof(DHCP_HDR))                      /* Buffer size defined in RFC 2131, p. 9.               */


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                                /* Start DHCP lease negotiation.                        */
                                                                /* MAC address, Client ID, Vendor Class ID and Param    */
                                                                /* request list MUST be set BEFORE use this function.   */
NET_ERR      DHCPc_Start                (void);

NET_ERR      DHCPc_CfgStack             (void);                 /* Configure IP, mask and gateway to the stack.         */

                                                                /* Configure MAC address.  MAC address is mandatory for */
                                                                /* DHCP protocol.                                       */
void         DHCPc_SetMacAddr           (CPU_INT08U   *mac_addr);

                                                                /* Configure Client ID.  Client ID is used instead of   */
                                                                /* MAC address for uniquely identify the client.        */
void         DHCPc_SetClientID          (CPU_INT08U    client_id_type,
                                         CPU_INT08U   *client_id,
                                         CPU_INT08U    client_id_len);

                                                                /* Configure Vendor Class ID.  Vendor Class ID is used  */
                                                                /* to discriminate DHCP configuration by product vendor.*/
void         DHCPc_SetVendorClassID     (CPU_INT08U   *vendor_class_id,
                                         CPU_INT08U    vendor_class_id_len);

                                                                /* Allow user to request more options to the DHCP       */
                                                                /* server. Get the option value using DHCPc_GetOpt().   */
void         DHCPc_SetParamRequestList  (CPU_INT08U   *param_request_list,
                                         CPU_INT08U    param_request_list_len);


                                                                /* ---- STATUS FNCTS ---------------------------------- */
DHCP_HDR    *DHCPc_GetHdr               (void);                 /* Get DHCP ACK packet header.                          */

                                                                /* Get an option value in the DHCP ACK packet.          */
CPU_INT08U  *DHCPc_GetOpt               (CPU_INT08U    opt_val);

                                                                /* DHCP client task managing lease time and renewal     */
void         DHCPc_Task                 (void         *p_arg);


/*
*********************************************************************************************************
*                                       RTOS INTERFACE FUNCTIONS
*                                           (see dhcp-c_os.c)
*********************************************************************************************************
*/

void        DHCPc_OS_Init               (void        *p_arg,
                                         CPU_INT08U  *perr);

void        DHCPc_OS_Reset              (CPU_INT08U  *perr);

CPU_INT32U  DHCPc_OS_GetElapsedSec      (CPU_INT32U  *start_time);

CPU_INT32U  DHCPc_OS_GetRemainingTime   (CPU_INT08U   timer,
                                         CPU_INT08U  *perr);

void        DHCPc_OS_SignalBoundState   (CPU_INT08U  *perr);

void        DHCPc_OS_TimeDly            (CPU_INT32U   sec);

void        DHCPc_OS_WaitBoundState     (CPU_INT08U  *perr);

void        DHCPc_OS_WaitLeaseTime      (CPU_INT08U   timer,
                                         CPU_INT08U  *perr);

void        DHCPc_OS_StartLeaseTmr      (CPU_INT08U   timer,
                                         CPU_INT32U   sec,
                                         CPU_INT08U  *perr);


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

#ifndef  TRACE_LEVEL_DEBUG
#define  TRACE_LEVEL_DEBUG                               2
#endif

#ifndef  DHCPc_TRACE_LEVEL
#define  DHCPc_TRACE_LEVEL                      TRACE_LEVEL_OFF
#endif

#ifndef  DHCPc_TRACE
#define  DHCPc_TRACE                            printf
#endif

#define  DHCPc_TRACE_INFO(x)                  ((DHCPc_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(DHCPc_TRACE x) : (void)0)
#define  DHCPc_TRACE_DEBUG(x)                 ((DHCPc_TRACE_LEVEL >= TRACE_LEVEL_DEBUG) ? (void)(DHCPc_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                /* Task name for debugging purposes.                    */
#ifndef  DHCPc_OS_CFG_TASK_NAME
#error  "DHCPc_OS_CFG_TASK_NAME                 illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'dhcp-c_cfg.h'              "
#endif

                                                                /* Task priority.                                       */
#ifndef  DHCPc_OS_CFG_TASK_PRIO
#error  "DHCPc_OS_CFG_TASK_PRIO                 illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'dhcp-c_cfg.h'              "
#endif

                                                                /* Task stack size.                                     */
#ifndef  DHCPc_OS_CFG_TASK_STK_SIZE
#error  "DHCPc_OS_CFG_TASK_STK_SIZE             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'dhcp-c_cfg.h'              "
#endif

                                                                /* DHCP server IP port. Default is 67.                  */
#ifndef  DHCPc_CFG_IP_PORT_SERVER
#error  "DHCPc_CFG_IP_PORT_SERVER               illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'dhcp-c_cfg.h'              "
#endif

                                                                /* DHCP client IP port. Default is 68.                  */
#ifndef  DHCPc_CFG_IP_PORT_CLIENT
#error  "DHCPc_CFG_IP_PORT_CLIENT               illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'dhcp-c_cfg.h'              "
#endif

                                                                /* Maximum number of lease request retries              */
#ifndef  DHCPc_CFG_MAX_REQ_LEASE_RETRY
#error  "DHCPc_CFG_MAX_REQ_LEASE_RETRY          illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'dhcp-c_cfg.h'              "
#endif

                                                                /* The broadcast bit is used to instruct the server to  */
                                                                /* use broadcast packets instead of unicast.            */
                                                                /* Useful when the IP stack cannot receive unicast      */
                                                                /* packets when not fully configured.                   */
#ifndef  DHCPc_CFG_BROADCAST_BIT
#error  "DHCPc_CFG_BROADCAST_BIT                illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'dhcp-c_cfg.h'              "
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif  /* __DHCP_C_H__ */
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
