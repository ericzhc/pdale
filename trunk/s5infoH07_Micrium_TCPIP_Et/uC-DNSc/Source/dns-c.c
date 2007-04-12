/*
*********************************************************************************************************
*                                                uC/DNSc
*                                      Domain Name Server (client)
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
*                                              DNS CLIENT
*
* Filename      : dns-c.c
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*
* This file implements a basic DNS client.  The functionality provides the basic ability to retrieve an
* IP address from a domain name.
*
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   DNSc_MODULE
#include  <lib_str.h>
#include  <net.h>
#include  <dns-c.h>


/*
*********************************************************************************************************
*                                           INITIALIZED DATA
*********************************************************************************************************
*/

const  DNSc_ERR_STRUCT  DNSc_Err_Msg[] = {
    {DNS_STATUS_OK,                 "No error"},
    {DNS_STATUS_NO_SERVER,          "No DNS server"},
    {DNS_STATUS_SOCK_FAIL,          "Socket fail"},
    {DNS_STATUS_TX_FAIL,            "Sendto fail"},
    {DNS_STATUS_RX_FAIL,            "Recvfrom fail"},
    {DNS_STATUS_GIVING_UP,          "Can't contact server"},
    {DNS_STATUS_FMT_ERR,            "Format error"},
    {DNS_STATUS_SERVER_FAIL,        "Server fail"},
    {DNS_STATUS_NAME_NOT_EXIST,     "Name not exist"},
    {DNS_STATUS_BAD_RESPONSE_TYPE,  "Bad DNS server response"},
    {DNS_STATUS_BAD_QUESTION_COUNT, "Bad question count"},
    {DNS_STATUS_BAD_ANSWER_COUNT,   "Bad answer count"},
    {DNS_STATUS_NOT_A_RESPONSE,     "Not a DNS response"},
    {DNS_STATUS_BAD_RESPONSE_ID,    "Bad DNS response ID"},
    {DNS_STATUS_INVALID,            "Invalid error ID"}         /* MUST be last entry in table.                         */
};


/*
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*/

static  DNSc_CACHE_STRUCT  DNSc_HostName[DNSc_MAX_CACHED_HOSTNAMES];
static  CPU_INT32U         DNSc_CurOrder;

static  NET_IP_ADDR        DNSc_Srv;
static  CPU_INT16U         DNSc_QueryID;
static  CPU_INT32U         DNSc_LastErr;


/*
*********************************************************************************************************
*                                              DNSc_Init()
*
* Description : Initialize DNSc module.
*
* Argument(s) : srv_addr        DNS server IP address (network order).
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*********************************************************************************************************
*/

void  DNSc_Init (NET_IP_ADDR  srv_addr)
{
    DNSc_Srv     = srv_addr;
    DNSc_QueryID = 0;

    DNSc_InitCache();
}


/*
*********************************************************************************************************
*                                         DNSc_GetHostByName()
*
* Description : Convert a host name to the corresponding IP address using DNS service.
*
* Argument(s) : host_name       host name
*
* Return(s)   : IP address of the host.
*
* Caller(s)   : Application.
*
* Note(s)     : This function is a simplified version of gethostbyname().  Given a domain name, this
*               function will first query a locally maintained cache then, if not found there, it will
*               issue a DNS query to retrieve the hosts IP address.
*********************************************************************************************************
*/

NET_IP_ADDR  DNSc_GetHostByName (CPU_CHAR  *host_name)
{
    CPU_INT16U         pkt_buf[DNSc_PKT_BUF_SIZE / sizeof(CPU_INT16U)];
    CPU_CHAR          *pkt_ptr;
    CPU_CHAR          *name_ptr;
    CPU_CHAR          *dot_ptr;
    CPU_INT32U         pkt_len;
    CPU_INT32U         name_len;

    NET_SOCK_ADDR_IP   srv_addr;
    NET_SOCK_ADDR_IP   from_addr;
    NET_IP_ADDR        host_addr;

    CPU_INT32S         sock_id;
    CPU_INT32U         rx_retry;
    CPU_INT16S         host_addr_len;

    CPU_INT32U         fixed_hdr_len;
    CPU_INT32U         rx_type;
    CPU_INT32U         q_tot;
    CPU_INT32U         a_tot;
    CPU_INT16U         entry_type;
    CPU_INT16U         entry_len;

    CPU_INT32U         ix;
    CPU_INT32U         net_len;
    NET_ERR            net_err;


    fixed_hdr_len = 6 * sizeof(CPU_INT16U);
    DNSc_LastErr  = DNS_STATUS_OK;
                                                                /* First check to see if the incoming host name is      */
                                                                /* simply a decimal-dot-formatted IP address.  If it    */
                                                                /* is, then just convert it to a 32-bit long and return.*/
    host_addr = NetASCII_Str_to_IP(host_name, &net_err);
    if (net_err  == NET_ASCII_ERR_NONE) {
        host_addr = NET_UTIL_HOST_TO_NET_32(host_addr);
        return (host_addr);
    }

                                                                /* First try to find the hostname in our local cache.   */
    for (ix = 0; ix < DNSc_MAX_CACHED_HOSTNAMES; ix++) {
        if (Str_Cmp(DNSc_HostName[ix].Name, host_name) == 0) {
            return (DNSc_HostName[ix].Addr);
        }
    }

                                                                /* If not in the cache, we query the network.           */

                                                                /* Get a socket.                                        */
    sock_id = NetSock_Open(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &net_err);
    if (net_err != NET_SOCK_ERR_NONE) {
        DNSc_LastErr = DNS_STATUS_SOCK_FAIL;
        return ((NET_IP_ADDR)0);
    }

    Mem_Set(&srv_addr, (CPU_CHAR)0, sizeof(srv_addr));
    srv_addr.Family = AF_INET;
    srv_addr.Addr   = NET_UTIL_HOST_TO_NET_32(DNSc_Srv);
    srv_addr.Port   = NET_UTIL_HOST_TO_NET_16(DNSc_CFG_IPPORT);
    from_addr.Port  = NET_UTIL_HOST_TO_NET_16(INADDR_ANY);

                                                                /* Build the message.  This query supports a single     */
                                                                /* internet host-address question.                      */

                                                                /* The fixed header.                                    */
    DNSc_QueryID++;
    pkt_buf[0] = NET_UTIL_HOST_TO_NET_16(DNSc_QueryID);         /* ... Unique ID                                        */
    pkt_buf[1] = NET_UTIL_HOST_TO_NET_16(0x0100);               /* ... Parameter field                                  */
    pkt_buf[2] = NET_UTIL_HOST_TO_NET_16(1);                    /* ... # of questions                                   */
    pkt_buf[3] = 0;                                             /* ... # of answers                                     */
    pkt_buf[4] = 0;                                             /* ... # of authority                                   */
    pkt_buf[5] = 0;                                             /* ... # of additional                                  */

                                                                /* The formatted name list.                             */

                                                                /* Each name is preceded by a single-byte length, so    */
                                                                /* for our query, the list is "LNNNLNNNLNNN0", where:   */
                                                                /*   'L' is the single-byte length of the name.         */
                                                                /*   'NN..N' is variable-lenght domain.                 */
                                                                /*   '0' is the length of the next name in the list;    */
                                                                /*       hence, indicating the end of the list.         */
                                                                /*                                                      */
                                                                /* For each '.' (dot) in the hostname, there is a LNNN  */
                                                                /* pair.                                                */
    pkt_ptr  = (CPU_CHAR *)&pkt_buf[fixed_hdr_len / sizeof(CPU_INT16U)];
    name_ptr = host_name;
    name_len = Str_Len(host_name);
    do {
        dot_ptr = Str_Char(name_ptr, '.');
        if (dot_ptr != (CPU_CHAR *)0) {
            *pkt_ptr++ = dot_ptr-name_ptr;
            Mem_Copy(pkt_ptr, name_ptr, dot_ptr - name_ptr);
            pkt_ptr   += (dot_ptr - name_ptr);
            name_ptr   = dot_ptr + 1;
        } else {
            *pkt_ptr++ = Str_Len(name_ptr);
            Str_Copy(pkt_ptr, name_ptr);
        }
    } while (dot_ptr);

                                                                /* Since the size of the name can be arbitrary (not     */
                                                                /* aligned), we must populate the TYPE and CLASS fields */
                                                                /* one byte at a time.                                  */
    pkt_ptr    += Str_Len(name_ptr) + 1;
    *pkt_ptr++  = 0;
    *pkt_ptr++  = 1;                                            /* type  = 'A'  (host address)                          */
    *pkt_ptr++  = 0;
    *pkt_ptr    = 1;                                            /* class = 'IN' (the internet)                          */

                                                                /* Send the DNS query.                                  */

                                                                /* Total message size is:                               */
                                                                /* FIXED_HDR SIZE + NAME SIZE + TYPE SIZE + CLASS SIZE  */
                                                                /*     + (NAME_LEN SIZE * 2)                            */
                                                                /* where:                                               */
                                                                /*     FIXED_HDR SIZE  = 12                             */
                                                                /*     NAME SIZE       = strlen(name)       = name_len  */
                                                                /*     TYPE SIZE       = sizeof(CPU_INT16U) = 2         */
                                                                /*     CLASS SIZE      = sizeof(CPU_INT16U) = 2         */
                                                                /*     NAME_LEN SIZE   = sizeof(CPU_CHAR)   = 1         */

                                                                /* There are 2 name lenghts.  The first one is the size */
                                                                /* of the host name we are querying for and the second  */
                                                                /* one is zero (indicating no more names in the list).  */
                                                                /* So, the total length of the packet is <NAME_LEN+18>. */
    pkt_len = name_len + fixed_hdr_len + sizeof(CPU_INT16U) * 2 + sizeof(CPU_CHAR) * 2;
    net_len = NetSock_TxDataTo( sock_id,
                                pkt_buf,
                                pkt_len,
                                NET_SOCK_FLAG_NONE,
                               (NET_SOCK_ADDR *)&srv_addr,
                                NET_SOCK_ADDR_SIZE,
                               &net_err);
    if ((net_err != NET_SOCK_ERR_NONE) ||
        (net_len != pkt_len)) {
        DNSc_LastErr = DNS_STATUS_TX_FAIL;
        NetSock_Close(sock_id, &net_err);
        return ((NET_IP_ADDR)0);
    }

    NetOS_Sock_RxQ_TimeoutSet(sock_id, 4 * DEF_TIME_NBR_mS_PER_SEC, &net_err);

    rx_retry = 0;
    do {
        net_len = NetSock_RxDataFrom( sock_id,
                                      pkt_buf,
                                      sizeof(pkt_buf),
                                      NET_SOCK_FLAG_NONE,
                                     (NET_SOCK_ADDR *)&from_addr,
                                     &host_addr_len,
                                      0,
                                      0,
                                      0,
                                     &net_err);
        if (net_err == NET_SOCK_ERR_RX_Q_EMPTY) {               /* Timeout occured.                                     */
		    rx_retry++;
            if (rx_retry >= DNSc_MAX_RETRY) {
                DNSc_LastErr = DNS_STATUS_GIVING_UP;
                NetSock_Close(sock_id, &net_err);
                return ((NET_IP_ADDR)0);
            }
            net_len = NetSock_TxDataTo( sock_id,
                                        pkt_buf,
                                        pkt_len,
                                        NET_SOCK_FLAG_NONE,
                                       (NET_SOCK_ADDR *)&srv_addr,
                                        NET_SOCK_ADDR_SIZE,
                                       &net_err);
            if ((net_err != NET_SOCK_ERR_NONE) ||
                (net_len != pkt_len)) {
                DNSc_LastErr = DNS_STATUS_TX_FAIL;
                NetSock_Close(sock_id, &net_err);
                return ((NET_IP_ADDR)0);
            }
        }
        if (net_err != NET_SOCK_ERR_NONE) {
            DNSc_LastErr = DNS_STATUS_RX_FAIL;
            NetSock_Close(sock_id, &net_err);
            return ((NET_IP_ADDR)0);
        }
    } while ((rx_retry < DNSc_MAX_RETRY) && (net_err != NET_SOCK_ERR_NONE));

    NetSock_Close(sock_id, &net_err);

                                                                /* At this point we can assume that a packet was        */
                                                                /* received, so now we verify the returned packet.      */

    if ((pkt_buf[1] & NET_UTIL_HOST_TO_NET_16(0x8000)) == 0) {  /* Response?                                            */
        DNSc_LastErr = DNS_STATUS_NOT_A_RESPONSE;
        return ((NET_IP_ADDR)0);
    }
    if (pkt_buf[0] != NET_UTIL_HOST_TO_NET_16(DNSc_QueryID)) {  /* Correct ID?                                          */
        DNSc_LastErr = DNS_STATUS_BAD_RESPONSE_ID;
        return ((NET_IP_ADDR)0);
    }

    rx_type = pkt_buf[1] & NET_UTIL_HOST_TO_NET_16(0x000F);
    if (rx_type != 0) {                                         /* Response normal?                                     */
        switch (NET_UTIL_NET_TO_HOST_16(rx_type)) {
            case 1:
                 DNSc_LastErr = DNS_STATUS_FMT_ERR;
                 break;

            case 2:
                 DNSc_LastErr = DNS_STATUS_SERVER_FAIL;
                 break;

            case 3:
                 DNSc_LastErr = DNS_STATUS_NAME_NOT_EXIST;
                 break;

            default:
                 DNSc_LastErr = DNS_STATUS_BAD_RESPONSE_TYPE;
                 break;
        }
        return ((NET_IP_ADDR)0);
    }

    q_tot = NET_UTIL_NET_TO_HOST_16(pkt_buf[2]);
    if (q_tot != 1) {                                           /* Question count = 1?                                  */
        DNSc_LastErr = DNS_STATUS_BAD_QUESTION_COUNT;
        return ((NET_IP_ADDR)0);
    }
    a_tot = NET_UTIL_NET_TO_HOST_16(pkt_buf[3]);
    if (a_tot < 1) {                                            /* Answer count >= 1?                                   */
        DNSc_LastErr = DNS_STATUS_BAD_ANSWER_COUNT;
        return ((NET_IP_ADDR)0);
    }

                                                                /* At this point we can assume that the received packet */
                                                                /* format is ok.  Now we need to parse the packet for   */
                                                                /* the "answer" to our query.                           */

                                                                /* Set 'pkt_ptr' to point to the start of the question  */
                                                                /* list. There should only be one question in the       */
                                                                /* response.                                            */
    pkt_ptr = (CPU_CHAR *)&pkt_buf[fixed_hdr_len / sizeof(CPU_INT16U)];

                                                                /* Skip over the questions.                             */
    for (ix = 0; ix < q_tot; ix++) {
        while (*pkt_ptr != (CPU_INT16U)0) {                     /* While 'L' is nonzero.                                */
            pkt_ptr += (*pkt_ptr + sizeof(CPU_CHAR));
        }
        pkt_ptr += 5;                                           /* Account for last 'L' plus TYPE/CLASS.                */
    }

                                                                /* The 'pkt_ptr' pointer is now pointing a list of      */
                                                                /* resource entry that correspond to the answer list.   */
                                                                /* It is from this list that we must retrieve the       */
                                                                /* information we are looking for.                      */
    for (ix = 0; ix < a_tot; ix++) {
                                                                /* The first portion of the entry is the resource       */
                                                                /* domain name and it may be literal string (a 1-octet  */
                                                                /* count followed by characters that make up the name)  */
                                                                /* or a pointer to a literal string.                    */
        if ((*pkt_ptr & 0xC0) == 0xC0) {                        /* Compressed?                                          */
            pkt_ptr += 2;
        } else {
            while (*pkt_ptr != (CPU_CHAR)0) {                   /* While 'L' is nonzero.                                */
                pkt_ptr += (*pkt_ptr + sizeof(CPU_CHAR));
            }
            pkt_ptr += sizeof(CPU_CHAR);
        }

        Mem_Copy(&entry_type, pkt_ptr, sizeof(entry_type));
        entry_type = NET_UTIL_NET_TO_HOST_16(entry_type);
        pkt_ptr   += 8;
        Mem_Copy(&entry_len, pkt_ptr, sizeof(entry_len));
        entry_len  = NET_UTIL_NET_TO_HOST_16(entry_len);

        if ((entry_type == DNS_ENTRY_TYPE_A) && (entry_len == sizeof(host_addr))) {
            pkt_ptr += sizeof(entry_len);
            Mem_Copy(&host_addr, pkt_ptr, sizeof(host_addr));
            DNSc_AddCacheEntry(host_name, host_addr);
            return (host_addr);
        } else {
            pkt_ptr += (entry_len + sizeof(entry_len));
        }
    }
    return ((NET_IP_ADDR)0);
}


/*
*********************************************************************************************************
*                                          DNSc_GetLastError()
*
* Description : Return the last error code.
*
* Argument(s) : None.
*
* Return(s)   : Last error code.
*
* Caller(s)   : Application.
*********************************************************************************************************
*/

CPU_INT32U  DNSc_GetLastError (void)
{
    return (DNSc_LastErr);
}


/*
*********************************************************************************************************
*                                         DNSc_GetLastErrMsg()
*
* Description : Return the last error massage.
*
* Argument(s) : None.
*
* Return(s)   : Read-only string containing the last error message.
*
* Caller(s)   : Application.
*********************************************************************************************************
*/

CPU_CHAR  *DNSc_GetLastErrMsg (void)
{
    const  DNSc_ERR_STRUCT  *err_msg_ptr;


    err_msg_ptr = DNSc_Err_Msg;
    while (err_msg_ptr->Code != DNS_STATUS_INVALID) {
        if (err_msg_ptr->Code == DNSc_LastErr) {
            return (err_msg_ptr->Msg);
        }
        err_msg_ptr++;
    }
    return (err_msg_ptr->Msg);
}


/*
*********************************************************************************************************
*                                           DNSc_InitCache()
*
* Description : Initialize DNS cache.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : Various.
*********************************************************************************************************
*/

static  void  DNSc_InitCache (void)
{
    CPU_INT32U  ix;


    for (ix = 0; ix < DNSc_MAX_CACHED_HOSTNAMES; ix++) {
        DNSc_HostName[ix].Order   = 0;
        DNSc_HostName[ix].Addr    = (NET_IP_ADDR)0;
        DNSc_HostName[ix].Name[0] = (CPU_CHAR)0;
    }
    DNSc_CurOrder = 0;
}


/*
*********************************************************************************************************
*                                         DNSc_AddCacheEntry()
*
* Description : Add an entry in the cache.
*
* Argument(s) : host_name       host name to add.
*               host_addr       host IP address to add (network order).
*
* Return(s)   : DEF_OK          entry found and removed.
*               DEF_FAIL        entry not found.
*
* Caller(s)   : DNSc_GetHostByName().
*********************************************************************************************************
*/

static  CPU_BOOLEAN  DNSc_AddCacheEntry (CPU_CHAR     *host_name,
                                         NET_IP_ADDR   host_addr)
{
    CPU_INT32U  ix;
    CPU_INT32U  lowest;
    CPU_INT32U  lowest_ix;
    CPU_INT32U  str_result;


                                                                /* Validate incoming name size.                         */
    str_result = Str_Len(host_name);
    if ((str_result >= DNSc_MAX_HOSTNAME_SIZE) || (host_addr == (NET_IP_ADDR)0)) {
        return (DEF_FAIL);
    }

                                                                /* Look for an empty slot.                              */
    lowest = DEF_INT_32U_MAX_VAL;
    for (ix = 0; ix < DNSc_MAX_CACHED_HOSTNAMES; ix++) {
        if (DNSc_HostName[ix].Order == 0) {
            DNSc_HostName[ix].Order = ++DNSc_CurOrder;
            DNSc_HostName[ix].Addr  = host_addr;
            Str_Copy(DNSc_HostName[ix].Name, host_name);
            return (DEF_OK);
        } else {
            if (DNSc_HostName[ix].Order < lowest) {
                lowest    = DNSc_HostName[ix].Order;
                lowest_ix = ix;
            }
        }
    }

                                                                /* If all slots are filled, use the slot that had the   */
                                                                /* the lowest Order value (this would be the oldest     */
                                                                /* entry).                                              */
	DNSc_CurOrder++;
    DNSc_HostName[lowest_ix].Order = DNSc_CurOrder;
    DNSc_HostName[lowest_ix].Addr  = host_addr;
    Str_Copy(DNSc_HostName[lowest_ix].Name, host_name);
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                      DNSc_DelCacheEntryByName()
*
* Description : Find a host name in the cache and remove the entry.
*
* Argument(s) : host_name       host name to find.
*
* Return(s)   : DEF_OK          entry found and removed.
*               DEF_FAIL        entry not found.
*
* Caller(s)   : Various
*********************************************************************************************************
*/

#if 0
static  CPU_BOOLEAN  DNSc_DelCacheEntryByName (CPU_CHAR  *host_name)
{
    CPU_INT32U  ix;
    CPU_INT32U  str_result;


    for (ix = 0; ix < DNSc_MAX_CACHED_HOSTNAMES; ix++) {
        str_result = Str_Cmp(DNSc_HostName[ix].Name, host_name);
        if (str_result == 0) {
            DNSc_HostName[ix].Order   = 0;
            DNSc_HostName[ix].Addr    = (NET_IP_ADDR)0;
            DNSc_HostName[ix].Name[0] = (CPU_CHAR)0;
            return (DEF_OK);
        }
    }
    return (DEF_FAIL);
}
#endif


/*
*********************************************************************************************************
*                                      DNSc_DelCacheEntryByAddr()
*
* Description : Find an address in the cache and remove the entry.
*
* Argument(s) : host_addr       host IP address to find (network order).
*
* Return(s)   : DEF_OK          entry found and removed.
*               DEF_FAIL        entry not found.
*
* Caller(s)   : Various
*********************************************************************************************************
*/

#if 0
static  CPU_BOOLEAN  DNSc_DelCacheEntryByAddr (NET_IP_ADDR  host_addr)
{
    CPU_INT32U  ix;


    for (ix = 0; ix < DNSc_MAX_CACHED_HOSTNAMES; ix++) {
        if (DNSc_HostName[ix].Addr == host_addr) {
            DNSc_HostName[ix].Order   = 0;
            DNSc_HostName[ix].Addr    = (NET_IP_ADDR)0;
            DNSc_HostName[ix].Name[0] = (CPU_CHAR)0;
            return (DEF_OK);
        }
    }
    return (DEF_FAIL);
}
#endif
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
