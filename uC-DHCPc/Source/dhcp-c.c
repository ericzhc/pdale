/*
*********************************************************************************************************
*                                               uC/DHCPc
*                             Dynamic Host Configuration Protocol (client)
*
*                          (c) Copyright 2003-2005; Micrium, Inc.; Weston, FL
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
* Version       : V1.80
* Programmer(s) : JJL
*                 JDH
*********************************************************************************************************
*
* This code implements a subset of the DHCP client protocol.
*
* Based on RFC2131 spec, the "automatic allocation" mode, in which DHCP assigns a permanent lease
* to a client, is the only mode supported.
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


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  CPU_INT16U  DHCPc_BuildDiscover  (void);

static  CPU_INT16U  DHCPc_BuildRequest   (void);

static  NET_ERR     DHCPc_GetReply       (NET_SOCK_ID  sock_id,
                                          CPU_INT08U   timeout_sec,
                                          CPU_INT08U   reply_type);

static  CPU_INT08U  DHCPc_GetRetryDly    (CPU_INT08U   retry);


/*
*********************************************************************************************************
*                                             DHCPc_Start()
*
* Description : Start DHCP lease negociation.
*
* Argument(s) : None.
*
* Return(s)   : Error message.
*
* Caller(s)   : Application.
*
* Note(s)     : Since this implemenation is at the sockets API level, the discover and request are sent
*               with the source IP address that is currently being used by the target.  To be compliant
*               with the DHCP protocol, the TCP/IP stack MUST be configured with default addresses
*               (all zeroes) during DHCP lease negociation.
*********************************************************************************************************
*/

NET_ERR  DHCPc_Start (void)
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


    DHCPc_PktTxHdr = (DHCP_HDR *)&DHCPc_PktTx[0];
    DHCPc_PktRxHdr = (DHCP_HDR *)&DHCPc_PktRx[0];
    DHCPc_PktTxOpt = &DHCPc_PktTx[DHCP_HDR_SIZE];
    DHCPc_PktRxOpt = &DHCPc_PktRx[DHCP_HDR_SIZE];

    if (DHCPc_MacAddrSet != DEF_YES) {
        err = DHCPc_ERR_MAC_ADDR_NOT_SET;
        return (err);
    }

                                                                /* Take the three low-significant bytes of the MAC      */
                                                                /* address and a counter for the 4th byte as the        */
                                                                /* Transaction ID.                                      */
    Mem_Copy(&DHCPc_TransID, &DHCPc_MacAddr[3], 3);
    DHCPc_TransID      = NET_UTIL_NET_TO_HOST_32(DHCPc_TransID);

    Mem_Set(&addr_local, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);
    addr_local.Family  = NET_SOCK_ADDR_FAMILY_IP_V4;
    addr_local.Addr    = NET_UTIL_HOST_TO_NET_32(NET_SOCK_ADDR_IP_WILD_CARD);
    addr_local.Port    = NET_UTIL_HOST_TO_NET_16(DHCP_IP_PORT_CLIENT);

    Mem_Set(&addr_server, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);
    addr_server.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
    addr_server.Addr   = NET_UTIL_HOST_TO_NET_32(NET_IP_ADDR_BROADCAST);
    addr_server.Port   = NET_UTIL_HOST_TO_NET_16(DHCP_IP_PORT_SERVER);

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

        if (err == DHCPc_ERR_RX_FAIL)
        {
            retry++;
            timeout_sec = DHCPc_GetRetryDly(retry);

            if (timeout_sec == (CPU_INT08U)(~0)) {
                err = DHCPc_ERR_GIVING_UP;
                break;
            }
            continue;
        }

                                                                /* Discussion about using a new xid on each             */
                                                                /* transmission in (RFC 2131, p. 36).                   */
        DHCPc_TransID++;

        size     = DHCPc_BuildRequest();
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

        if (err == DHCPc_ERR_RX_FAIL)
        {
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

    if (err == NET_SOCK_ERR_NONE) {
        err =  DHCPc_ERR_NONE;
    }

    return (err);
}


/*
*********************************************************************************************************
*                                            DHCPc_CfgStack()
*
* Description : Configure uC/TCP-IP with the IP address, mask and gateway/router.
*
* Argument(s) : None.
*
* Return(s)   : Error message.
*
* Caller(s)   : Application.
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
    CPU_BOOLEAN   rtn_code;
    NET_ERR       err;


    ip  = NET_IP_ADDR_THIS_HOST;
    msk = NET_IP_ADDR_NONE;
    gw  = NET_IP_ADDR_NONE;
    err = DHCPc_ERR_NONE;

                                                                /* Must Mem_Copy because header may be misaliged.       */
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

                                                                /* IP and mask are mandatory parts of the configuration.*/
    rtn_code = NetIP_CfgAddrThisHost(NET_UTIL_HOST_TO_NET_32(ip), NET_UTIL_HOST_TO_NET_32(msk));
    if (rtn_code == DEF_NO) {
        err = DHCPc_ERR_CFG_STACK_FAILED;
    }

    NetIP_CfgAddrDfltGateway(NET_UTIL_HOST_TO_NET_32(gw));      /* Gateway in not a mandatory part of the configuration.*/

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
* Caller(s)   : DHCPc_Start().
*
* Note(s)     : The DHCPDISCOVER is issued as an ethernet broadcast.  If the broadcast flag is non-zero
*               then the server will reply by a broadcast.  Usefull when the IP stack cannot receive
*               unicast packets when not fully configured.
*********************************************************************************************************
*/

static  CPU_INT16U  DHCPc_BuildDiscover (void)
{
    CPU_INT08U  *opt_iter;
    CPU_INT32U   opt_len;
    CPU_INT32U   magic_cookie;


    Mem_Set(DHCPc_PktTxHdr, (CPU_CHAR)0, DHCP_HDR_SIZE);

    DHCPc_PktTxHdr->op    = DHCP_OP_REQUEST;
    DHCPc_PktTxHdr->htype = 1;                                  /* Hardware type           = ethernet.                  */
    DHCPc_PktTxHdr->hlen  = NET_IF_ADDR_SIZE;                   /* Hardware address length = 6 (48-bit ethernet).       */
    DHCPc_PktTxHdr->xid   = NET_UTIL_HOST_TO_NET_32(DHCPc_TransID);
    DHCPc_PktTxHdr->secs  = 0;

#if (DHCP_CFG_BROADCAST_BIT == DEF_ENABLED)
    DHCPc_PktTxHdr->flags = NET_UTIL_HOST_TO_NET_16(DHCP_FLAG_BROADCAST);
#endif

    Mem_Copy(DHCPc_PktTxHdr->chaddr, DHCPc_MacAddr, NET_IF_ADDR_SIZE);

                                                                /* DHCP Options.                                        */
    opt_iter = DHCPc_PktTxOpt;

                                                                /* DHCP Magic cookie.                                   */
    magic_cookie     = NET_UTIL_HOST_TO_NET_32(DHCP_STD_MAGIC_COOKIE);
    Mem_Copy(opt_iter, &magic_cookie, 4);
    opt_iter        += 4;

    *opt_iter++      = DHCP_OPT_DHCP_MESSAGE_TYPE;              /* DHCP Message Type = DHCP_REQUEST.                    */
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
* Argument(s) : None.
*
* Return(s)   : Size of the DHCPREQUEST packet.
*
* Caller(s)   : DHCPc_Start().
*
* Note(s)     : The DHCPREQUEST message is broadcast back with the "server identifier" option set to
*               indicate which server has been selected (in case more than one has offered).
*********************************************************************************************************
*/

static  CPU_INT16U  DHCPc_BuildRequest (void)
{
    CPU_INT08U  *opt_iter;
    CPU_INT32U   opt_len;
    CPU_INT08U  *opt_temp;
    CPU_INT32U   magic_cookie;


    Mem_Set(DHCPc_PktTxHdr, (CPU_CHAR)0, DHCP_HDR_SIZE);

    DHCPc_PktTxHdr->op     = DHCP_OP_REQUEST;
    DHCPc_PktTxHdr->htype  = 1;                                 /* Hardware type           = ethernet.                  */
    DHCPc_PktTxHdr->hlen   = NET_IF_ADDR_SIZE;                  /* Hardware address length = 6 (48-bit ethernet).       */

                                                                /* Use the same xid for the request as was used for the */
                                                                /* discover (RFC 2131, p. 36).                          */
    DHCPc_PktTxHdr->xid    = NET_UTIL_HOST_TO_NET_32(DHCPc_TransID);
    DHCPc_PktTxHdr->secs   = 0;
    DHCPc_PktTxHdr->flags  = DHCPc_PktRxHdr->flags;
    DHCPc_PktTxHdr->yiaddr = DHCPc_PktRxHdr->yiaddr;
    Mem_Copy(DHCPc_PktTxHdr->chaddr, DHCPc_PktRxHdr->chaddr, NET_IF_ADDR_SIZE);


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
    opt_temp = DHCPc_GetOpt(DHCP_OPT_SERVER_IDENTIFIER);
    if (opt_temp != (CPU_VOID *)0) {
        *opt_iter++  = DHCP_OPT_SERVER_IDENTIFIER;
        *opt_iter++  = 4;
        Mem_Copy(opt_iter, opt_temp + 2, 4);
        opt_iter    += 4;
    }

    *opt_iter++      = DHCP_OPT_REQUESTED_IP_ADDRESS;           /* Requested IP address.                                */
    *opt_iter++      = 4;
    Mem_Copy(opt_iter, &DHCPc_PktRxHdr->yiaddr, 4);
    opt_iter        += 4;

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
    *opt_iter++      = 2 + DHCPc_ParamRequestListLen;
    *opt_iter++      = DHCP_OPT_SUBNET_MASK;
    *opt_iter++      = DHCP_OPT_ROUTER;

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
*                                            DHCPc_GetReply()
*
* Description : Get DHCP Reply packet
*
* Argument(s) : sock_id         RX socket ID.
*               timeout_sec     maximum time to wait for reply.
*               reply_type      DHCP reply type.
*
* Return(s)   : Error message.
*
* Caller(s)   : DHCPc_Start.
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
    CPU_INT16S          rtn_code;
    NET_ERR             err;


    NetOS_Sock_RxQ_TimeoutSet(sock_id, timeout_sec * DEF_TIME_NBR_mS_PER_SEC, &err);

    do {
        addr_remote_len = NET_SOCK_ADDR_SIZE;
        rtn_code        = NetSock_RxDataFrom( sock_id,
                                              DHCPc_PktRx,
                                              DHCPc_PKT_BUF_SIZE,
                                              NET_SOCK_FLAG_NONE,
                                             (NET_SOCK_ADDR *)&addr_remote,
                                             &addr_remote_len,
                                              0,
                                              0,
                                              0,
                                             &err);

        if (rtn_code <= 0) {
            err = DHCPc_ERR_RX_FAIL;
        }

        opt = DHCPc_GetOpt(DHCP_OPT_DHCP_MESSAGE_TYPE);
    } while ((opt == (CPU_VOID *)0)                       &&
             (*(opt + 2) != reply_type)                   &&
             (DHCPc_PktRxHdr->xid != DHCPc_PktTxHdr->xid) &&
             (err == NET_SOCK_ERR_NONE));

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
*               of 64 seconds + ramdomization.
*
*               The value of rnd_delta will be 2, 1, 0 or -1 depending on the target's MAC address.
*
* Argument(s) : retry       number of retries already done.
*
* Return(s)   : Delay before next retry.
*
* Caller(s)   : DHCPc_Start().
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_INT08U  DHCPc_GetRetryDly (CPU_INT08U  retry)
{
    CPU_INT08U  dly;
    CPU_INT08S  rnd_delta;


    if (retry >= DHCPc_RETRY_MAX) {
        return (CPU_INT08U)(~0);
    }

                                                                /* Retransmission delays defined in (RFC 2131, p. 24).  */
    switch(retry) {
        case 0:
             dly = 4;
             break;

        case 1:
             dly = 8;
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
*                                           DHCPc_SetMacAddr()
*
* Description : Set MAC Address.
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
* Description : Set DHCP client ID.
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
* Description : Set DHCP vendor class ID.
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
* Description : Set DHCP parameter request list.
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
* Description : Get header of DHCP acknowledge packet.
*
* Argument(s) : None.
*
* Return(s)   : Header of DHCP acknowledge packet.
*
* Caller(s)   : Application.
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
* Description : Based on the incoming option pointer and a specified option value, search through the
*               options list for the value and return a pointer to that option.
*
* Argument(s) : opt_val     option value to search.
*
* Return(s)   : Pointer to the option.
*
* Caller(s)   : DHCPc_BuildRequest(), application.
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
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	   		   	 			       	  	 	   	   	  		 				 		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
