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
* Filename      : dhcp-c.c
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


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define    DHCPc_MODULE
#include  <net.h>
#include  <dhcp-c_cfg.h>
#include  <dhcp-c.h>


/*
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*/

static  CPU_INT08U    DHCPc_PktRx[DHCPc_PKT_BUF_SIZE];
static  DHCP_HDR     *DHCPc_PktRxHdr;
static  CPU_INT08U   *DHCPc_PktRxOpt;

static  CPU_INT08U    DHCPc_PktTx[DHCPc_PKT_BUF_SIZE];
static  DHCP_HDR     *DHCPc_PktTxHdr;
static  CPU_INT08U   *DHCPc_PktTxOpt;

static  CPU_INT08U   *DHCPc_MacAddr;
static  CPU_BOOLEAN   DHCPc_MacAddrSet;

static  CPU_BOOLEAN   DHCPc_ClientIDSet;
static  CPU_INT08U   *DHCPc_ClientID;
static  CPU_INT08U    DHCPc_ClientIDLen;
static  CPU_INT08U    DHCPc_ClientIDType;

static  CPU_BOOLEAN   DHCPc_VendorClassIDSet;
static  CPU_INT08U   *DHCPc_VendorClassID;
static  CPU_INT08U    DHCPc_VendorClassIDLen;

static  CPU_BOOLEAN   DHCPc_ParamRequestListSet;
static  CPU_INT08U   *DHCPc_ParamRequestList;
static  CPU_INT08U    DHCPc_ParamRequestListLen;

static  CPU_INT32U    DHCPc_TransID;

static  NET_IP_ADDR   DHCPc_ServerIPAddr;

static  NET_IP_ADDR   DHCPc_LastIPAddr;


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  NET_ERR     DHCPc_Init               (void);

static  NET_ERR     DHCPc_Renew              (void);

static  NET_ERR     DHCPc_Rebind             (void);

static  CPU_INT16U  DHCPc_BuildDiscover      (void);

static  CPU_INT16U  DHCPc_BuildRequest       (DHCP_STATES  state);

static  NET_ERR     DHCPc_CfgStackVal        (NET_IP_ADDR  ip,
                                              NET_IP_ADDR  msk,
                                              NET_IP_ADDR  gw);

static  void        DHCPc_CompLeaseTime      (CPU_INT32U   start_time,
                                              CPU_INT08U  *perr);

static  NET_ERR     DHCPc_GetReply           (NET_SOCK_ID  sock_id,
                                              CPU_INT08U   timeout_sec,
                                              CPU_INT08U   reply_type);

static  CPU_INT08U  DHCPc_GetRetryDly        (CPU_INT08U   retry);

static  void        DHCPc_SaveParam          (void);


/*
*********************************************************************************************************
*                                             DHCPc_Start()
*
* Description : Initializes DHCPc task and starts DHCP lease negotiation.
*
* Argument(s) : None.
*
* Return(s)   : Error message:
*
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_TASK_INIT                 Error initializing.
*               DHCPc_ERR_MAC_ADDR_NOT_SET          MAC address not set properly.
*
*                                                   - RETURNED BY DHCPc_Init() : -
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_TASK_INIT                 Error initializing.
*               DHCPc_ERR_SOCK_FAIL                 Error opening / binding socket.
*               DHCPc_ERR_TX_FAIL                   Error transmitting.
*               DHCPc_ERR_GIVING_UP                 Maximum retry reached, giving up.
*               DHCPc_ERR_TMR                       Error setting timers.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

NET_ERR  DHCPc_Start (void)
{
    NET_ERR     err;
    CPU_INT08U  err2;


    DHCPc_LastIPAddr = 0;
                                                                /* Create DHCP client task                              */
    DHCPc_OS_Init((void *)0, &err2);
    if (err2 != DHCPc_ERR_NONE) {
        err = DHCPc_ERR_TASK_INIT;
        return (err);
    }


    if (DHCPc_MacAddrSet != DEF_YES) {
        err = DHCPc_ERR_MAC_ADDR_NOT_SET;
        return (err);
    }

                                                                /* Take the three low-significant bytes of the MAC      */
                                                                /* address and a counter for the 4th byte as the        */
                                                                /* Transaction ID.                                      */
    Mem_Copy(&DHCPc_TransID, &DHCPc_MacAddr[3], 3);
    DHCPc_TransID = NET_UTIL_NET_TO_HOST_32(DHCPc_TransID);


    err = DHCPc_Init();

    return (err);
}


/*
*********************************************************************************************************
*                                            DHCPc_CfgStack()
*
* Description : Configures uC/TCP-IP with the IP address, mask and gateway/router with values taken from
*               the last DHCP offer.
*
* Argument(s) : None.
*
* Return(s)   : Error message:
*
*               DHCPc_ERR_NONE                      No error.
*
*                                                   - RETURNED BY DHCPc_CfgStackVal() : -
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_CFG_STACK_FAILED          Error configuring stack.
*
* Caller(s)   : Application
*               DHCPc_Task
*
* Note(s)     : none.
*********************************************************************************************************
*/

NET_ERR  DHCPc_CfgStack (void)
{
    NET_IP_ADDR   ip;
    NET_IP_ADDR   msk;
    NET_IP_ADDR   gw;
    CPU_INT08U   *opt;
    NET_ERR       err;


    ip  = NET_IP_ADDR_THIS_HOST;
    msk = NET_IP_ADDR_NONE;
    gw  = NET_IP_ADDR_NONE;

    err = DHCPc_ERR_NONE;

                                                                /* Must Mem_Copy because header may be misaligned.      */
    Mem_Copy(&ip, &(DHCPc_GetHdr()->yiaddr), sizeof(DHCPc_GetHdr()->yiaddr));

                                                                /* See section 'DHCP OPTIONS' about interpretation of   */
                                                                /* the DHCP option fields.                              */
    opt = DHCPc_GetOpt(DHCP_OPT_SUBNET_MASK);
    if (opt != (CPU_VOID *)0) {
        Mem_Copy(&msk, (CPU_VOID *)(opt + 2), *(CPU_INT08U *)(opt + 1));
    }

    opt = DHCPc_GetOpt(DHCP_OPT_ROUTER);
    if (opt != (CPU_VOID *)0) {
        Mem_Copy(&gw,  (CPU_VOID *)(opt + 2), *(CPU_INT08U *)(opt + 1));
    }

    err = DHCPc_CfgStackVal(ip, msk, gw);

    return (err);
}


/*
*********************************************************************************************************
*                                           DHCPc_CfgStackVal()
*
* Description : Configures uC/TCP-IP with the IP address, mask and gateway/router with specific
*               parameters.
*
* Argument(s) : ip      IP address to set the stack to.
*               msk     Mask to set the stack to.
*               gw      Default gateway/router.
*
* Return(s)   : Error message:
*
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_CFG_STACK_FAILED          Error configuring stack.
*
* Caller(s)   : DHCPc_CfgStack
*               DHCPc_Task
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  NET_ERR  DHCPc_CfgStackVal  (NET_IP_ADDR  ip,
                                     NET_IP_ADDR  msk,
                                     NET_IP_ADDR  gw)
{
    NET_ERR      err;
    CPU_BOOLEAN  rtn_code;
    CPU_CHAR     buf[16];
    NET_ERR      err2;


    err      = DHCPc_ERR_NONE;

                                                                /* IP and mask are mandatory parts of the configuration.*/
    rtn_code = NetIP_CfgAddrThisHost(NET_UTIL_NET_TO_HOST_32(ip), NET_UTIL_NET_TO_HOST_32(msk));
    if (rtn_code == DEF_NO) {
        err = DHCPc_ERR_CFG_STACK_FAILED;
    }

    NetIP_CfgAddrDfltGateway(NET_UTIL_NET_TO_HOST_32(gw));      /* Gateway in not a mandatory part of the configuration.*/

    NetASCII_IP_to_Str(NET_UTIL_NET_TO_HOST_32(ip), buf, DEF_NO, &err2);
    DHCPc_TRACE_INFO(("DHCPc_CfgStack: IP address %s\n", buf));
    NetASCII_IP_to_Str(NET_UTIL_NET_TO_HOST_32(msk), buf, DEF_NO, &err2);
    DHCPc_TRACE_INFO(("DHCPc_CfgStack: Mask       %s\n", buf));
    NetASCII_IP_to_Str(NET_UTIL_NET_TO_HOST_32(gw), buf, DEF_NO, &err2);
    DHCPc_TRACE_INFO(("DHCPc_CfgStack: Gateway    %s\n", buf));

    return (err);
}


/*
*********************************************************************************************************
*                                             DHCPc_Init()
*
* Description : Performs actions associated with the INIT state until end of configuration.
*
*                   (a) Send    DHCPDISCOVER (broadcast)
*                   (b) Receive DHCPOFFER
*                   (c) Send    DHCPREQUEST (broadcast)
*                   (d) Receive DHCPACK
*
* Argument(s) : None.
*
* Return(s)   : Error message:
*
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_TASK_INIT                 Error initializing.
*               DHCPc_ERR_SOCK_FAIL                 Error opening / binding socket.
*               DHCPc_ERR_TX_FAIL                   Error transmitting.
*               DHCPc_ERR_GIVING_UP                 Maximum retry reached, giving up.
*
*                                                   - RETURNED BY DHCPc_CompLeaseTime() : -
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_TMR                       Error setting timers.
*
* Caller(s)   : DHCPc_Start
*               DHCPc_Task
*
* Note(s)     : The DHCPDISCOVER is issued as an Ethernet broadcast.
*
*               Since this implementation is at the sockets API level, the discover and request are sent
*               with the source IP address that is currently being used by the target.  To be compliant
*               with the DHCP protocol, the TCP/IP stack MUST be configured with default addresses
*               (all zeros) during DHCP lease negotiation.
*********************************************************************************************************
*/

static  NET_ERR  DHCPc_Init (void)
{
    NET_SOCK_ID       sock_id;
    NET_SOCK_ADDR_IP  addr_local;
    NET_SOCK_ADDR_IP  addr_server;
    CPU_INT08U        retry;
    CPU_INT08U        timeout_sec;
    CPU_INT16S        size;
    CPU_INT16S        rtn_code;
    NET_ERR           err;
    NET_ERR           err2;
    CPU_INT08U        err3;
    CPU_INT32U        start_nego_time;
    CPU_INT08U       *opt_temp;
    CPU_INT32U        server_identifier;


    DHCPc_OS_Reset(&err3);
    if (err3 == DHCPc_ERR_TASK_INIT) {
        return (err3);
    }


    DHCPc_PktTxHdr     = (DHCP_HDR *)&DHCPc_PktTx[0];
    DHCPc_PktRxHdr     = (DHCP_HDR *)&DHCPc_PktRx[0];
    DHCPc_PktTxOpt     = &DHCPc_PktTx[DHCP_HDR_SIZE];
    DHCPc_PktRxOpt     = &DHCPc_PktRx[DHCP_HDR_SIZE];

    DHCPc_SaveParam();                                          /* Saving former parameters                             */

    Mem_Set(&addr_local, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);
    addr_local.Family  = NET_SOCK_ADDR_FAMILY_IP_V4;
    addr_local.Addr    = NET_UTIL_HOST_TO_NET_32(NET_SOCK_ADDR_IP_WILD_CARD);
    addr_local.Port    = NET_UTIL_HOST_TO_NET_16(DHCPc_CFG_IP_PORT_CLIENT);

    Mem_Set(&addr_server, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);
    addr_server.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
    addr_server.Addr   = NET_UTIL_HOST_TO_NET_32(NET_IP_ADDR_BROADCAST);
    addr_server.Port   = NET_UTIL_HOST_TO_NET_16(DHCPc_CFG_IP_PORT_SERVER);

    retry              = 0;
    timeout_sec        = DHCPc_GetRetryDly(retry);

    sock_id            = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                                       NET_SOCK_TYPE_DATAGRAM,
                                       NET_SOCK_PROTOCOL_UDP,
                                      &err);

    NetSock_Bind(sock_id,
                (NET_SOCK_ADDR *)&addr_local,
                 NET_SOCK_ADDR_SIZE, &err);


    if (sock_id < 0) {
        return (DHCPc_ERR_SOCK_FAIL);
    }

    do {
                                                                /* Discussion about using a new xid on each             */
                                                                /* transmission in (RFC 2131, p. 36).                   */
        DHCPc_TransID++;

        DHCPc_OS_GetElapsedSec(&start_nego_time);               /* Get start of lease time                              */

        size     = DHCPc_BuildDiscover();
        rtn_code = NetSock_TxDataTo( sock_id,
                                     DHCPc_PktTx,
                                     size,
                                     NET_SOCK_FLAG_NONE,
                                    (NET_SOCK_ADDR *)&addr_server,
                                     NET_SOCK_ADDR_SIZE,
                                    &err);

        if (rtn_code != size) {
            err = DHCPc_ERR_TX_FAIL;
            break;
        }

        err = DHCPc_GetReply(sock_id, timeout_sec, DHCP_MSG_OFFER);

        if (err == DHCPc_ERR_RX_FAIL || err == DHCPc_ERR_RX_NACK)
        {
            retry++;
            timeout_sec = DHCPc_GetRetryDly(retry);

            if (timeout_sec == (CPU_INT08U)(~0)) {
                err = DHCPc_ERR_GIVING_UP;
                break;
            }
            continue;
        }

                                                                /* Save server identifier for RENEW                     */
        opt_temp = DHCPc_GetOpt(DHCP_OPT_SERVER_IDENTIFIER);
        if (opt_temp != (CPU_VOID *)0) {
            Mem_Copy(&server_identifier, opt_temp + 2, 4);
        } else {
            server_identifier = 0;
        }
        DHCPc_ServerIPAddr = NET_UTIL_NET_TO_HOST_32(server_identifier);

        size     = DHCPc_BuildRequest(DHCP_STATE_INIT);
        rtn_code = NetSock_TxDataTo( sock_id,
                                     DHCPc_PktTx,
                                     size,
                                     NET_SOCK_FLAG_NONE,
                                    (NET_SOCK_ADDR *)&addr_server,
                                     NET_SOCK_ADDR_SIZE,
                                    &err);

        if (rtn_code != size) {
            err = DHCPc_ERR_TX_FAIL;
            break;
        }

        err = DHCPc_GetReply(sock_id, timeout_sec, DHCP_MSG_ACK);

                                                                /* Only checking for RX_FAIL                            */
        if (err == DHCPc_ERR_RX_FAIL) {                         /* If DHCPc_ERR_RX_NACK, the process restarts           */
            retry++;
            timeout_sec = DHCPc_GetRetryDly(retry);

            if (timeout_sec == (CPU_INT08U)(~0)) {
                err = DHCPc_ERR_GIVING_UP;
                break;
            }
            continue;
        }
    } while (err != NET_SOCK_ERR_NONE);

    NetSock_Close(sock_id, &err2);                              /* Ignore error that may happen here.                   */

    if (err == NET_SOCK_ERR_NONE) {                             /* Set timers and signal task                           */
        DHCPc_CompLeaseTime(start_nego_time, &err3);
        if (err3 != DHCPc_ERR_NONE) {
            err = err3;
        } else {
            DHCPc_OS_SignalBoundState(&err3);
            err = DHCPc_ERR_NONE;
        }
    }

    return (err);
}


/*
*********************************************************************************************************
*                                             DHCPc_Renew()
*
* Description : Performs actions associated with the RENEW state until end of configuration.
*
*                   (a) Send    DHCPREQUEST (unicast)
*                   (b) Receive DHCPACK
*
* Argument(s) : None.
*
* Return(s)   : Error message:
*
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_SOCK_FAIL                 Error opening / binding socket.
*               DHCPc_ERR_TX_FAIL                   Error transmitting.
*               DHCPc_ERR_RX_NACK                   NACK received.
*
*                                                   - RETURNED BY DHCPc_CompLeaseTime() : -
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_TMR                       Error setting timers.
*
* Caller(s)   : DHCPc_Task
*
* Note(s)     :
*********************************************************************************************************
*/

static  NET_ERR  DHCPc_Renew  (void)
{
    NET_SOCK_ID       sock_id;
    NET_SOCK_ADDR_IP  addr_local;
    NET_SOCK_ADDR_IP  addr_server;
    CPU_INT08U        timeout_sec;
    NET_ERR           err;
    NET_ERR           err2;
    CPU_INT08U        err3;
    CPU_INT32U        start_nego_time;
    CPU_INT16S        size;
    CPU_INT16S        rtn_code;
    CPU_INT08U        buf[16];


    DHCPc_PktTxHdr     = (DHCP_HDR *)&DHCPc_PktTx[0];
    DHCPc_PktRxHdr     = (DHCP_HDR *)&DHCPc_PktRx[0];
    DHCPc_PktTxOpt     = &DHCPc_PktTx[DHCP_HDR_SIZE];
    DHCPc_PktRxOpt     = &DHCPc_PktRx[DHCP_HDR_SIZE];


    Mem_Set(&addr_local, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);
    addr_local.Family  = NET_SOCK_ADDR_FAMILY_IP_V4;
    addr_local.Addr    = NET_UTIL_HOST_TO_NET_32(NET_SOCK_ADDR_IP_WILD_CARD);
    addr_local.Port    = NET_UTIL_HOST_TO_NET_16(DHCPc_CFG_IP_PORT_CLIENT);

    Mem_Set(&addr_server, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);
    addr_server.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
    addr_server.Addr   = NET_UTIL_HOST_TO_NET_32(DHCPc_ServerIPAddr);
    addr_server.Port   = NET_UTIL_HOST_TO_NET_16(DHCPc_CFG_IP_PORT_SERVER);


    sock_id            = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                                       NET_SOCK_TYPE_DATAGRAM,
                                       NET_SOCK_PROTOCOL_UDP,
                                      &err);

    NetSock_Bind(sock_id,
                (NET_SOCK_ADDR *)&addr_local,
                 NET_SOCK_ADDR_SIZE, &err);


    if (sock_id < 0) {
        return (DHCPc_ERR_SOCK_FAIL);
    }


    NetASCII_IP_to_Str(DHCPc_ServerIPAddr, buf, DEF_NO, &err);
    DHCPc_TRACE_INFO(("DHCPc_Renew: Connected to %s\n", buf));

    do {
        DHCPc_OS_GetElapsedSec(&start_nego_time);               /* Get start of lease time                              */

        size     = DHCPc_BuildRequest(DHCP_STATE_RENEW);
        rtn_code = NetSock_TxDataTo( sock_id,
                                     DHCPc_PktTx,
                                     size,
                                     NET_SOCK_FLAG_NONE,
                                    (NET_SOCK_ADDR *)&addr_server,
                                     NET_SOCK_ADDR_SIZE,
                                    &err);

        if (rtn_code != size) {
            err = DHCPc_ERR_TX_FAIL;
            break;
        }

        err = DHCPc_GetReply(sock_id, timeout_sec, DHCP_MSG_ACK);

        if (err == DHCPc_ERR_RX_FAIL || DHCPc_ERR_RX_NACK) {
            break;
        }
    } while (err != NET_SOCK_ERR_NONE);

    NetSock_Close(sock_id, &err2);                              /* Ignore error that may happen here.                   */

    if (err == NET_SOCK_ERR_NONE) {                             /* Set timers and save parameters                       */
        DHCPc_OS_Reset(&err3);
        DHCPc_CompLeaseTime(start_nego_time, &err3);
        if (err3 != DHCPc_ERR_NONE) {
            err = err3;
        } else {
            err = DHCPc_ERR_NONE;
        }
    }

    return (err);
}


/*
*********************************************************************************************************
*                                             DHCPc_Rebind()
*
* Description : Performs actions associated with the REBIND state until end of configuration.
*
*                   (a) Send    DHCPREQUEST (broadcast)
*                   (b) Receive DHCPACK
*
* Argument(s) : None.
*
* Return(s)   : Error message:
*
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_SOCK_FAIL                 Error opening / binding socket.
*               DHCPc_ERR_TX_FAIL                   Error transmitting.
*               DHCPc_ERR_RX_FAIL                   Error receiving.
*               DHCPc_ERR_RX_NACK                   NACK received.
*
*                                                   - RETURNED BY DHCPc_CompLeaseTime() : -
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_TMR                       Error setting timers.
*
* Caller(s)   : DHCPc_Task
*
* Note(s)     :
*********************************************************************************************************
*/

static  NET_ERR  DHCPc_Rebind  (void)
{
    NET_SOCK_ID       sock_id;
    NET_SOCK_ADDR_IP  addr_local;
    NET_SOCK_ADDR_IP  addr_server;
    CPU_INT08U        timeout_sec;
    NET_ERR           err;
    NET_ERR           err2;
    CPU_INT08U        err3;
    CPU_INT32U        start_nego_time;
    CPU_INT16S        size;
    CPU_INT16S        rtn_code;


    DHCPc_PktTxHdr     = (DHCP_HDR *)&DHCPc_PktTx[0];
    DHCPc_PktRxHdr     = (DHCP_HDR *)&DHCPc_PktRx[0];
    DHCPc_PktTxOpt     = &DHCPc_PktTx[DHCP_HDR_SIZE];
    DHCPc_PktRxOpt     = &DHCPc_PktRx[DHCP_HDR_SIZE];


    Mem_Set(&addr_local, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);
    addr_local.Family  = NET_SOCK_ADDR_FAMILY_IP_V4;
    addr_local.Addr    = NET_UTIL_HOST_TO_NET_32(NET_SOCK_ADDR_IP_WILD_CARD);
    addr_local.Port    = NET_UTIL_HOST_TO_NET_16(DHCPc_CFG_IP_PORT_CLIENT);

    Mem_Set(&addr_server, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);
    addr_server.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
    addr_server.Addr   = NET_UTIL_HOST_TO_NET_32(NET_IP_ADDR_BROADCAST);
    addr_server.Port   = NET_UTIL_HOST_TO_NET_16(DHCPc_CFG_IP_PORT_SERVER);

    timeout_sec        = 8;                                     /* Set timeout to 8 seconds (arbitraty)                 */

    sock_id            = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                                       NET_SOCK_TYPE_DATAGRAM,
                                       NET_SOCK_PROTOCOL_UDP,
                                      &err);

    NetSock_Bind(sock_id,
                (NET_SOCK_ADDR *)&addr_local,
                 NET_SOCK_ADDR_SIZE, &err);


    if (sock_id < 0) {
        return (DHCPc_ERR_SOCK_FAIL);
    }


    DHCPc_TRACE_INFO(("DHCPc_Rebind: REBIND state\n"));


    do {
        DHCPc_OS_GetElapsedSec(&start_nego_time);               /* Get start of lease time                              */

        size     = DHCPc_BuildRequest(DHCP_STATE_REBIND);
        rtn_code = NetSock_TxDataTo( sock_id,
                                     DHCPc_PktTx,
                                     size,
                                     NET_SOCK_FLAG_NONE,
                                    (NET_SOCK_ADDR *)&addr_server,
                                     NET_SOCK_ADDR_SIZE,
                                    &err);

        if (rtn_code != size) {
            err = DHCPc_ERR_TX_FAIL;
            break;
        }

        err = DHCPc_GetReply(sock_id, timeout_sec, DHCP_MSG_ACK);

        if (err == DHCPc_ERR_RX_FAIL || DHCPc_ERR_RX_NACK) {
            break;
        }
    } while (err != NET_SOCK_ERR_NONE);

    NetSock_Close(sock_id, &err2);                              /* Ignore error that may happen here.                   */

    if (err == NET_SOCK_ERR_NONE) {                             /* Set timers and save parameters                       */
        DHCPc_OS_Reset(&err3);
        DHCPc_CompLeaseTime(start_nego_time, &err3);
        if (err3 != DHCPc_ERR_NONE) {
            err = err3;
        } else {
            err = DHCPc_ERR_NONE;
        }
    }

    return (err);
}


/*
*********************************************************************************************************
*                                         DHCPc_BuildDiscover()
*
* Description : Builds the DHCPDISCOVER packet.
*
* Argument(s) : None.
*
* Return(s)   : Size of the DHCPREQUEST packet.
*
* Caller(s)   : DHCPc_Init
*
* Note(s)     : The DHCPDISCOVER is issued as an Ethernet broadcast.  If the broadcast flag is non-zero
*               then the server will reply by a broadcast.  Useful when the IP stack cannot receive
*               unicast packets when not fully configured.
*********************************************************************************************************
*/

static  CPU_INT16U  DHCPc_BuildDiscover (void)
{
    CPU_INT08U  *opt_iter;
    CPU_INT32U   opt_len;
    CPU_INT32U   magic_cookie;
    CPU_INT32U   net_addr;


    Mem_Set(DHCPc_PktTxHdr, (CPU_CHAR)0, DHCP_HDR_SIZE);

    DHCPc_PktTxHdr->op    = DHCP_OP_REQUEST;
    DHCPc_PktTxHdr->htype = 1;                                  /* Hardware type           = Ethernet.                  */
    DHCPc_PktTxHdr->hlen  = NET_IF_ADDR_SIZE;                   /* Hardware address length = 6 (48-bit Ethernet).       */
    DHCPc_PktTxHdr->xid   = NET_UTIL_HOST_TO_NET_32(DHCPc_TransID);
    DHCPc_PktTxHdr->secs  = 0;

#if (DHCP_CFG_BROADCAST_BIT == DEF_ENABLED)
    DHCPc_PktTxHdr->flags = NET_UTIL_HOST_TO_NET_16(DHCP_FLAG_BROADCAST);
#endif

    Mem_Copy(DHCPc_PktTxHdr->chaddr, DHCPc_MacAddr, NET_IF_ADDR_SIZE);

                                                                /* DHCP Options.                                        */
    opt_iter         = DHCPc_PktTxOpt;

                                                                /* DHCP Magic cookie.                                   */
    magic_cookie     = NET_UTIL_HOST_TO_NET_32(DHCP_STD_MAGIC_COOKIE);
    Mem_Copy(opt_iter, &magic_cookie, 4);
    opt_iter        += 4;

    *opt_iter++      = DHCP_OPT_DHCP_MESSAGE_TYPE;              /* DHCP Message Type = DHCP_DISCOVER.                   */
    *opt_iter++      = 1;
    *opt_iter++      = DHCP_MSG_DISCOVER;

    if (DHCPc_ClientIDSet == DEF_YES) {
        *opt_iter++  = DHCP_OPT_CLIENT_IDENTIFIER;              /* Client ID.                                           */
        *opt_iter++  = DHCPc_ClientIDLen + 1;
        *opt_iter++  = DHCPc_ClientIDType;
        Mem_Copy(opt_iter, DHCPc_ClientID, DHCPc_ClientIDLen);
        opt_iter    += DHCPc_ClientIDLen;
    }

    if (DHCPc_VendorClassIDSet == DEF_YES) {
        *opt_iter++  = DHCP_OPT_VENDOR_CLASS_IDENTIFIER;        /* Vendor Class ID.                                     */
        *opt_iter++  = DHCPc_VendorClassIDLen;
        Mem_Copy(opt_iter, DHCPc_VendorClassID, DHCPc_VendorClassIDLen);
        opt_iter    += DHCPc_VendorClassIDLen;
    }

    if (DHCPc_LastIPAddr != 0) {
        net_addr     = NET_UTIL_HOST_TO_NET_32(DHCPc_LastIPAddr);
        *opt_iter++  = DHCP_OPT_REQUESTED_IP_ADDRESS;
        *opt_iter++  = sizeof(NET_IP_ADDR);
        Mem_Copy(opt_iter, &net_addr, sizeof(NET_IP_ADDR));
        opt_iter    += sizeof(NET_IP_ADDR);
    }

    *opt_iter++      = DHCP_OPT_END;

                                                                /* Vendor-specific options must be => 64 bytes.         */
    opt_len = opt_iter - DHCPc_PktTxOpt;
    if (opt_len < 64) {
        Mem_Set(opt_iter, (CPU_CHAR)0, 64 - opt_len);
        opt_len = 64;
    }

    return (DHCP_HDR_SIZE + opt_len);
}


/*
*********************************************************************************************************
*                                          DHCPc_BuildRequest()
*
* Description : Builds the DHCPREQUEST packet.
*
* Argument(s) : state       DHCP client state.
*
* Return(s)   : Size of the DHCPREQUEST packet.
*
* Caller(s)   : DHCPc_Init
*               DHCPc_Renew
*               DHCPc_Rebind
*
* Note(s)     : The DHCPREQUEST message is broadcast back with the "server identifier" option set to
*               indicate which server has been selected (in case more than one has offered).
*********************************************************************************************************
*/

static  CPU_INT16U  DHCPc_BuildRequest (DHCP_STATES  state)
{
    CPU_INT08U  *opt_iter;
    CPU_INT32U   opt_len;
    CPU_INT08U  *opt_temp;
    CPU_INT32U   magic_cookie;
    NET_IP_ADDR  current_addr;
    CPU_INT32U   current_addr_net;


    Mem_Set(DHCPc_PktTxHdr, (CPU_CHAR)0, DHCP_HDR_SIZE);

    DHCPc_PktTxHdr->op     = DHCP_OP_REQUEST;
    DHCPc_PktTxHdr->htype  = 1;                                 /* Hardware type           = Ethernet.                  */
    DHCPc_PktTxHdr->hlen   = NET_IF_ADDR_SIZE;                  /* Hardware address length = 6 (48-bit Ethernet).       */

                                                                /* Use the same xid for the request as was used for the */
                                                                /* discover (RFC 2131, p. 36).                          */
    DHCPc_PktTxHdr->xid    = NET_UTIL_HOST_TO_NET_32(DHCPc_TransID);
    DHCPc_PktTxHdr->secs   = 0;
    DHCPc_PktTxHdr->flags  = DHCPc_PktRxHdr->flags;
    DHCPc_PktTxHdr->yiaddr = 0;
    Mem_Copy(DHCPc_PktTxHdr->chaddr, DHCPc_MacAddr, NET_IF_ADDR_SIZE);

    if (state == DHCP_STATE_BOUND ||
        state == DHCP_STATE_RENEW ||
        state == DHCP_STATE_REBIND) {
        current_addr = NetIP_GetAddrThisHost();
        current_addr_net = NET_UTIL_HOST_TO_NET_32(current_addr);
        Mem_Copy(&DHCPc_PktTxHdr->ciaddr, &current_addr_net, sizeof(NET_IP_ADDR));
    }


                                                                /* DHCP Options.                                        */
    opt_iter = DHCPc_PktTxOpt;

                                                                /* DHCP Magic cookie.                                   */
    magic_cookie = NET_UTIL_HOST_TO_NET_32(DHCP_STD_MAGIC_COOKIE);
    Mem_Copy(opt_iter, &magic_cookie, 4);
    opt_iter        += 4;

    *opt_iter++      = DHCP_OPT_DHCP_MESSAGE_TYPE;              /* DHCP Message Type = DHCP_REQUEST.                    */
    *opt_iter++      = 1;
    *opt_iter++      = DHCP_MSG_REQUEST;

                                                                /* DHCP Server ID (multiple servers discriminator).     */
    if (state == DHCP_STATE_INIT) {
        opt_temp = DHCPc_GetOpt(DHCP_OPT_SERVER_IDENTIFIER);
        if (opt_temp != (CPU_VOID *)0) {
            *opt_iter++  = DHCP_OPT_SERVER_IDENTIFIER;
            *opt_iter++  = 4;
            Mem_Copy(opt_iter, opt_temp + 2, 4);
            opt_iter    += 4;
        }

        *opt_iter++      = DHCP_OPT_REQUESTED_IP_ADDRESS;       /* Requested IP address.                                */
        *opt_iter++      = 4;
        Mem_Copy(opt_iter, &DHCPc_PktRxHdr->yiaddr, 4);
        opt_iter        += 4;
    }

    if (DHCPc_ClientIDSet == DEF_YES) {
        *opt_iter++  = DHCP_OPT_CLIENT_IDENTIFIER;              /* Client ID.                                           */
        *opt_iter++  = DHCPc_ClientIDLen + 1;
        *opt_iter++  = DHCPc_ClientIDType;
        Mem_Copy(opt_iter, DHCPc_ClientID, DHCPc_ClientIDLen);
        opt_iter    += DHCPc_ClientIDLen;
    }

    if (DHCPc_VendorClassIDSet == DEF_YES) {
        *opt_iter++  = DHCP_OPT_VENDOR_CLASS_IDENTIFIER;        /* Vendor Class ID.                                     */
        *opt_iter++  = DHCPc_VendorClassIDLen;
        Mem_Copy(opt_iter, DHCPc_VendorClassID, DHCPc_VendorClassIDLen);
        opt_iter    += DHCPc_VendorClassIDLen;
    }

                                                                /* Parameter request list.                              */
    *opt_iter++      = DHCP_OPT_PARAMETER_REQUEST_LIST;
    *opt_iter++      = 5 + DHCPc_ParamRequestListLen;
    *opt_iter++      = DHCP_OPT_SUBNET_MASK;
    *opt_iter++      = DHCP_OPT_ROUTER;
    *opt_iter++      = DHCP_OPT_IP_ADDRESS_LEASE_TIME;
    *opt_iter++      = DHCP_OPT_RENEWAL_TIME_VALUE;
    *opt_iter++      = DHCP_OPT_REBINDING_TIME_VALUE;

    if (DHCPc_ParamRequestListSet == DEF_YES) {
        Mem_Copy(opt_iter, DHCPc_ParamRequestList, DHCPc_ParamRequestListLen);
         opt_iter   += DHCPc_ParamRequestListLen;
    }

    *opt_iter++      = DHCP_OPT_END;


    opt_len          = opt_iter - DHCPc_PktTxOpt;               /* Vendor-specific options must be => 64 bytes.         */
    if (opt_len < 64) {
        Mem_Set(opt_iter, (CPU_CHAR)0, 64 - opt_len);
        opt_len = 64;
    }

    return (DHCP_HDR_SIZE + opt_len);
}


/*
*********************************************************************************************************
*                                          DHCPc_CompLeaseTime()
*
* Description : Computes lease time and sets timers accordingly.
*
* Argument(s) : start_time              Time at which negotiation began.
*               perr                    Pointer to variable that will hold the return error code from
*                                       this function :
*
*                                       DHCPc_ERR_NONE              No error.
*                                       DHCPc_ERR_TMR               Error setting timers.
*
* Return(s)   : None.
*
* Caller(s)   : DHCPc_Init
*               DHCPc_Renew
*               DHCPc_Rebind
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  DHCPc_CompLeaseTime  (CPU_INT32U  start_time,
                                    CPU_INT08U *perr)
{
    CPU_INT08U   err;
    CPU_INT08U   err2;
    CPU_INT08U   err3;
    CPU_INT08U  *opt;
    CPU_INT32U   opt_val;
    CPU_INT32U   lease_time;
    CPU_INT32U   t1;
    CPU_INT32U   t2;
    CPU_INT32U   elapsed_time_sec;


    *perr = DHCPc_ERR_NONE;

                                                                /* Record lease time                                    */
    opt   = DHCPc_GetOpt(DHCP_OPT_IP_ADDRESS_LEASE_TIME);
    if (opt != (void *)0) {
        Mem_Copy((void *)&opt_val, (void *)(opt + 2), *(CPU_INT08U *)(opt + 1));
        lease_time = NET_UTIL_NET_TO_HOST_32(opt_val);
    } else {
        return;                                                 /* If no lease time specified, presume infinity         */
    }

    if (lease_time == DHCP_INFINITE_LEASE_TIME) {               /* Infinite lease, no need for the timers               */
        return;
    }

                                                                /* Record t2                                            */
    opt = DHCPc_GetOpt(DHCP_OPT_REBINDING_TIME_VALUE);
    t2  = 0;
    if (opt != (void *)0) {
        Mem_Copy((void *)&opt_val, (void *)(opt + 2), *(CPU_INT08U *)(opt + 1));
        t2 = NET_UTIL_NET_TO_HOST_32(opt_val);
    }
    if (t2 == 0 || t2 > lease_time) {
        t2 = lease_time * DHCP_T2_FRACTION_NUM / DHCP_T2_FRACTION_DEN;
    }

                                                                /* Record t1                                            */
    opt = DHCPc_GetOpt(DHCP_OPT_RENEWAL_TIME_VALUE);
    t1  = 0;
    if (opt != (void *)0) {
        Mem_Copy((void *)&opt_val, (void *)(opt + 2), *(CPU_INT08U *)(opt + 1));
        t1 = NET_UTIL_NET_TO_HOST_32(opt_val);
    }
    if (t1 == 0 || t1 > t2) {
        t1 = lease_time * DHCP_T1_FRACTION_NUM / DHCP_T1_FRACTION_DEN;
    }


    elapsed_time_sec = DHCPc_OS_GetElapsedSec(&start_time);     /* Computing elapsed time since beginning of nego       */

    if (elapsed_time_sec < t1) {
        lease_time -= elapsed_time_sec;                         /* Subtracting negotiation time                         */
        t1         -= elapsed_time_sec;
        t2         -= elapsed_time_sec;
    }

    DHCPc_TRACE_INFO(("DHCPc_CompLeaseTime: T1 = %d sec\n", t1));
    DHCPc_TRACE_INFO(("DHCPc_CompLeaseTime: T2 = %d sec\n", t2));
    DHCPc_TRACE_INFO(("DHCPc_CompLeaseTime: LT = %d sec\n", lease_time));

                                                                /* Start timers                                         */
    DHCPc_OS_StartLeaseTmr(DHCPc_TIME_T1,
                           t1,
                          &err);

    DHCPc_OS_StartLeaseTmr(DHCPc_TIME_T2,
                           t2,
                          &err2);

    DHCPc_OS_StartLeaseTmr(DHCPc_TIME_LEASE_TIME,
                           lease_time,
                          &err3);

    if (err != DHCPc_ERR_NONE || err2 != DHCPc_ERR_NONE || err2 != DHCPc_ERR_NONE) {
        DHCPc_OS_Reset(&err);
        *perr = DHCPc_ERR_TMR;
    }
}


/*
*********************************************************************************************************
*                                            DHCPc_GetReply()
*
* Description : Gets DHCP reply packet
*
* Argument(s) : sock_id         RX socket ID.
*               timeout_sec     maximum time to wait for reply.
*               reply_type      DHCP reply type.
*
* Return(s)   : Error message :
*
*               DHCPc_ERR_NONE                      No error.
*               DHCPc_ERR_RX_FAIL                   Error receiving.
*               DHCPc_ERR_RX_NACK                   NACK received.
*
* Caller(s)   : DHCPc_Init
*               DHCPc_Renew
*               DHCPc_Rebind
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  NET_ERR  DHCPc_GetReply (NET_SOCK_ID  sock_id,
                                 CPU_INT08U   timeout_sec,
                                 CPU_INT08U   reply_type)
{
    NET_SOCK_ADDR_IP    addr_remote;
    NET_SOCK_ADDR_LEN   addr_remote_len;
    CPU_INT08U         *opt;
    CPU_BOOLEAN         addr_match;
    NET_ERR             err;


    err = DHCPc_ERR_NONE;

    NetOS_Sock_RxQ_TimeoutSet(sock_id, timeout_sec * DEF_TIME_NBR_mS_PER_SEC, &err);

    do {
        if (err == DHCPc_ERR_RX_NACK) {                         /* NACK received in previous iteration                  */
            break;
        }

        addr_remote_len = NET_SOCK_ADDR_SIZE;
        NetSock_RxDataFrom( sock_id,
                            DHCPc_PktRx,
                            DHCPc_PKT_BUF_SIZE,
                            NET_SOCK_FLAG_NONE,
                           (NET_SOCK_ADDR *)&addr_remote,
                           &addr_remote_len,
                            0,
                            0,
                            0,
                           &err);

        if (err != NET_SOCK_ERR_NONE) {
            err  = DHCPc_ERR_RX_FAIL;
            break;
        }

        addr_match = Mem_Cmp(DHCPc_PktRxHdr->chaddr, DHCPc_MacAddr, NET_IF_ADDR_SIZE);
        opt        = DHCPc_GetOpt(DHCP_OPT_DHCP_MESSAGE_TYPE);

        DHCPc_TRACE_DEBUG(("DHCPc_GetReply: Reply type = %d\n", *(opt + 2)));

                                                                /* Setting err to DHCPc_ERR_RX_NACK for next iteration  */
        if (*(opt + 2) != DHCP_MSG_NACK && reply_type == DHCP_MSG_NACK) {
            err = DHCPc_ERR_RX_NACK;
        }

    } while ((addr_match          == DEF_NO)              ||
             (DHCPc_PktRxHdr->xid != DHCPc_PktTxHdr->xid) ||
             (opt                 == (CPU_VOID *)0)       ||
             (*(opt + 2)          != reply_type));

    return (err);
}


/*
*********************************************************************************************************
*                                          DHCPc_GetRetryDly()
*
* Description : This function is an implementation "similar" to the recommendation made in the RFC 2131
*               (DHCP) section 4.1 paragraph #8.
*
*               The delay before the first retransmission is 4 seconds randomized by the value of a
*               uniform random number chosen from the range -1 to +2.
*
*               The delay before the next retransmission is 8 seconds randomized by the same number as
*               previous randomization.  Each subsequent retransmission delay is doubled up to a maximum
*               of 64 seconds + randomization.
*
*               The value of rnd_delta will be 2, 1, 0 or -1 depending on the target's MAC address.
*
* Argument(s) : retry       number of retries already done.
*
* Return(s)   : Delay before next retry.
*
* Caller(s)   : DHCPc_Init()
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_INT08U  DHCPc_GetRetryDly (CPU_INT08U  retry)
{
    CPU_INT08U  dly;
    CPU_INT08S  rnd_delta;


    if (retry >= DHCPc_CFG_MAX_REQ_LEASE_RETRY) {
        return (CPU_INT08U)(~0);
    }

                                                                /* Retransmission delays defined in (RFC 2131, p. 24).  */
    switch(retry) {
        case 0:
             dly =  4;
             break;

        case 1:
             dly =  8;
             break;

        case 2:
             dly = 16;
             break;

        case 3:
             dly = 32;
             break;

        default:
             dly = 64;
             break;
    }

                                                                /* Generate a number from -1 to +2 from least-          */
    rnd_delta = (DHCPc_MacAddr[5] & 3) - 1;                     /* significant byte of the MAC address.                 */
    return ((CPU_INT08U)(dly + rnd_delta));
}


/*
*********************************************************************************************************
*                                           DHCPc_SaveParam()
*
* Description : Saves allocated IP address.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : DHCPc_Init
*               DHCPc_Renew
*               DHCPc_Rebind
*
* Note(s)     : None.
*********************************************************************************************************
*/
static  void  DHCPc_SaveParam (void)
{
    DHCP_HDR     *DHCP_hdr;


    DHCP_hdr = DHCPc_GetHdr();
    Mem_Copy(&DHCPc_LastIPAddr, &(DHCP_hdr->yiaddr), sizeof(DHCP_hdr->yiaddr));
}


/*
*********************************************************************************************************
*                                           DHCPc_SetMacAddr()
*
* Description : Sets MAC Address.
*
* Argument(s) : MacAddress      MAC address to set.
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : MAC address is mandatory for DHCP protocol.
*********************************************************************************************************
*/

CPU_VOID  DHCPc_SetMacAddr (CPU_INT08U  *mac_addr)
{
    if (mac_addr != (CPU_VOID *)0) {
        DHCPc_MacAddrSet = DEF_YES;
        DHCPc_MacAddr    = mac_addr;
    }
}


/*
*********************************************************************************************************
*                                          DHCPc_SetClientID()
*
* Description : Sets DHCP client ID.
*
* Argument(s) : client_id_type      Client ID type,
*               client_id           Client ID,
*               client_id_len       Client ID length.
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_VOID  DHCPc_SetClientID (CPU_INT08U   client_id_type,
                             CPU_INT08U  *client_id,
                             CPU_INT08U   client_id_len)
{
    if (client_id != (CPU_VOID *)0) {
        DHCPc_ClientIDSet  = DEF_YES;
        DHCPc_ClientIDType = client_id_type;
        DHCPc_ClientID     = client_id;
        DHCPc_ClientIDLen  = client_id_len;
    }
}


/*
*********************************************************************************************************
*                                        DHCPc_SetVendorClassID()
*
* Description : Sets DHCP vendor class ID.
*
* Argument(s) : vendor_class_id         Vendor class ID,
*               vendor_class_id_len     Vendor class ID length.
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_VOID  DHCPc_SetVendorClassID (CPU_INT08U  *vendor_class_id,
                                  CPU_INT08U   vendor_class_id_len)
{
    if (vendor_class_id != (CPU_VOID *)0) {
        DHCPc_VendorClassIDSet = DEF_YES;
        DHCPc_VendorClassID    = vendor_class_id;
        DHCPc_VendorClassIDLen = vendor_class_id_len;
    }
}


/*
*********************************************************************************************************
*                                      DHCPc_SetParamRequestList()
*
* Description : Sets DHCP parameter request list.
*
* Argument(s) : param_request_list          Parameter request list,
*               param_request_list_len      Parameter request list length.
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_VOID  DHCPc_SetParamRequestList (CPU_INT08U  *param_request_list,
                                     CPU_INT08U   param_request_list_len)
{
    if (param_request_list != (CPU_VOID *)0) {
        DHCPc_ParamRequestListSet = DEF_YES;
        DHCPc_ParamRequestList    = param_request_list;
        DHCPc_ParamRequestListLen = param_request_list_len;
    }
}


/*
*********************************************************************************************************
*                                             DHCPc_GetHdr()
*
* Description : Gets header of DHCP rx packet.
*
* Argument(s) : None.
*
* Return(s)   : Header of DHCP acknowledge packet.
*
* Caller(s)   : DHCPc_CfgStack
*               DHCPc_SaveParam
*
* Note(s)     : None.
*********************************************************************************************************
*/

DHCP_HDR  *DHCPc_GetHdr (void)
{
    return (DHCPc_PktRxHdr);
}


/*
*********************************************************************************************************
*                                             DHCPc_GetOpt()
*
* Description : Based on the incoming option pointer and a specified option value, searches through the
*               options list for the value and returns a pointer to that option.
*
* Argument(s) : opt_val     option value to search.
*
* Return(s)   : Pointer to the option.
*
* Caller(s)   : DHCPc_CfgStack
*               DHCPc_Init
*               DHCPc_BuildRequest
*               DHCPc_CompLeaseTime
*               DHCPc_GetReply
*               Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_INT08U  *DHCPc_GetOpt (CPU_INT08U  opt_val)
{
    CPU_INT08U   *opt;
    CPU_INT16U    safe_cnt;
    CPU_INT32U    magic_cookie;
    CPU_BOOLEAN   rtn_code;


    opt          = DHCPc_PktRxOpt;
    safe_cnt     = 1000;

                                                                /* Check for the magic cookie.                          */
    magic_cookie = NET_UTIL_HOST_TO_NET_32(DHCP_STD_MAGIC_COOKIE);
    rtn_code     = Mem_Cmp(opt, &magic_cookie, 4);
    if (rtn_code == DEF_NO) {
        return (0);                                             /* Bad magic cookie, bail out.                          */
    }
    opt += 4;                                                   /* Skip magic cookie.                                   */

    while ((*opt != 0xFF) &&
           (safe_cnt--)) {                                      /* Search until the End Option or safety counter == 0.  */
        if (*opt == 0) {                                        /* Skip the Padding Option.                             */
            opt++;
            continue;
        }

        if (*opt == opt_val) {                                  /* Found!                                               */
            return (opt);
        }

        opt += ((*(opt + 1)) + 2);                              /* Skip to next option.                                 */
    }

    return (0);
}


/*
*********************************************************************************************************
*                                             DHCPc_Task()
*
* Description : DHCP client main loop.  Calls appropriate functions when lease time or associated timers
*               expire in order to re-negociate address attribution.
*
* Argument(s) : p_arg       argument passed to the task (ignored).
*
* Return(s)   : None.
*
* Caller(s)   : None.
*
* Note(s)     : 1) From RFC 2131, section 4.4.5 'Reacquisition and expiration', "If the client receives
*                  a DHCPACK allocating that client its previous network address, the client SHOULD
*                  continue network processing.  If the client is given a new network address, it MUST
*                  NOT continue using the previous network address and SHOULD notify the local users of
*                  the problem".
*
*                  Instead of stopping network processing when entering the INIT state, this particular
*                  implementation of the DHCP client sets the IP address to 0.0.0.0 while re-negociating
*                  the lease.  Once this process is over, the IP address is set to the one received from
*                  the DHCP server.  If, for any reason, the DHCP server could not grant us the
*                  previously allocated address, a call to NetConn_CloseAllConns() is made to make sure
*                  all open connections with the former settings are reset.
*
*               2) From RFC 2131, section 4.4.5 'Reacquisition and expiration', "In both RENEWING and
*                  REBINDING states, if the client receives not response to its DHCPREQUEST message, the
*                  client SHOULD wait one-half of the remaining time until T2 (in REBINDING stage) and
*                  one-half of the remaining lease time (in REBINDING state), down to a minimum of 60
*                  seconds, before retransmitting the DHCPREQUEST message".
*********************************************************************************************************
*/

void  DHCPc_Task (void  *p_arg)
{
    CPU_INT08U   err;
    DHCP_STATES  state;
    CPU_INT32U   remaining_time_sec;


    DHCPc_OS_WaitBoundState(&err);                              /* Wait until configured (BOUND state)                  */

    state = DHCP_STATE_BOUND;                                   /* Start in state BOUND                                 */
    DHCPc_TRACE_INFO(("DHCPc_Task: DHCPc_Task started in BOUND state\n"));

    while (DEF_TRUE) {

        switch (state) {
            case DHCP_STATE_INIT:                               /* See note #1                                        */
                 DHCPc_TRACE_INFO(("DHCPc_Task: Entering INIT state\n"));
                 DHCPc_CfgStackVal(NET_IP_ADDR_NONE, NET_IP_ADDR_NONE, NET_IP_ADDR_NONE);
                 NetConn_CloseAllConns();
                 err = DHCPc_Init();

                 if (err == DHCPc_ERR_NONE) {
                    DHCPc_CfgStack();
                    NetConn_CloseAllConns();
                    DHCPc_TRACE_INFO(("DHCPc_Task: DHCP initialized, going into BOUND state\n"));
                    state = DHCP_STATE_BOUND;
                 }
                 break;

            case DHCP_STATE_BOUND:
                 DHCPc_TRACE_INFO(("DHCPc_Task: Entering BOUND state, waiting for T1\n"));
                 DHCPc_OS_WaitLeaseTime(DHCPc_TIME_T1, &err);   /* Wait until T1 expires                                */
                 DHCPc_TRACE_INFO(("DHCPc_Task: T1 expired, going into RENEW state\n"));
                 state = DHCP_STATE_RENEW;
                 break;

            case DHCP_STATE_RENEW:
                 DHCPc_TRACE_INFO(("DHCPc_Task: Entering RENEW state\n"));
                 err = DHCPc_Renew();

                 if (err == DHCPc_ERR_NONE) {
                     DHCPc_TRACE_INFO(("DHCPc_Task: Lease renewed\n"));
                    state = DHCP_STATE_BOUND;
                 } else if (err == DHCPc_ERR_RX_NACK) {
                     DHCPc_TRACE_INFO(("DHCPc_Task: NACK received, going into INIT\n"));
                    state = DHCP_STATE_INIT;
                 } else {                                       /* See note #2                                         */
                    DHCPc_TRACE_INFO(("DHCPc_Task: Nothing received, "));
                    remaining_time_sec = DHCPc_OS_GetRemainingTime(DHCPc_TIME_T2, &err);
                    if (remaining_time_sec <= DHCP_MIN_RETRANSMITTING_TIME * 2) {
                        DHCPc_TRACE_INFO(("waiting for T2...\n"));
                        DHCPc_OS_WaitLeaseTime(DHCPc_TIME_T2, &err);
                        DHCPc_TRACE_INFO(("DHCPc_Task: T2 expired, going into REBIND state\n"));
                        state = DHCP_STATE_REBIND;
                    } else {
                        DHCPc_TRACE_INFO(("delaying for %d sec\n", remaining_time_sec / 2));
                        DHCPc_OS_TimeDly(remaining_time_sec / 2);
                    }
                 }
                 break;

            case DHCP_STATE_REBIND:
                 DHCPc_TRACE_INFO(("DHCPc_Task: Entering REBIND state\n"));
                 err = DHCPc_Rebind();

                 if (err == DHCPc_ERR_NONE) {
                    state = DHCP_STATE_BOUND;
                 } else if (err == DHCPc_ERR_RX_NACK) {
                    state = DHCP_STATE_INIT;
                 } else {                                       /* See note #2                                         */
                    remaining_time_sec = DHCPc_OS_GetRemainingTime(DHCPc_TIME_LEASE_TIME, &err);
                    if (remaining_time_sec <= DHCP_MIN_RETRANSMITTING_TIME * 2) {
                        DHCPc_TRACE_INFO(("DHCPc_Task: Waiting for lease time...\n"));
                        DHCPc_OS_WaitLeaseTime(DHCPc_TIME_LEASE_TIME, &err);
                        DHCPc_TRACE_INFO(("DHCPc_Task: Lease time expired, going into INIT state\n"));
                        state = DHCP_STATE_INIT;
                    } else {
                        DHCPc_OS_TimeDly(remaining_time_sec / 2);
                    }
                 }
                 break;

            default:
                 DHCPc_TRACE_INFO(("DHCPc_Task: Entering default, going into INIT state\n"));
                 state = DHCP_STATE_INIT;
                 break;
        }
    }
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
