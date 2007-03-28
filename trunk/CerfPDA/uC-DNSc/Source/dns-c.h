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
* Filename      : dns-c.h
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*
* This file implements a basic DNS client.  The functionality provides the basic ability to retrieve an
* IP address from a domain name.
*
*********************************************************************************************************
*/

#ifndef  __DNS_C_H__
#define  __DNS_C_H__


/*
*********************************************************************************************************
*                                                DEFINES
*********************************************************************************************************
*/

                                                                /* ---- DNSc Module Configuration --------------------- */
#define  DNSc_MAX_HOSTNAME_SIZE                          255
#define  DNSc_MAX_CACHED_HOSTNAMES                        32
#define  DNSc_MAX_RETRY                                    5
#define  DNSc_PKT_BUF_SIZE                               512

                                                                /* ---- DNS Entry Type Definitions -------------------- */
#define  DNS_ENTRY_TYPE_A                                  1

                                                                /* ---- DNS Status Definitions ------------------------ */
#define  DNS_STATUS_OK                                     0
#define  DNS_STATUS_NO_SERVER                              1
#define  DNS_STATUS_SOCK_FAIL                              2
#define  DNS_STATUS_TX_FAIL                                3
#define  DNS_STATUS_RX_FAIL                                4
#define  DNS_STATUS_GIVING_UP                              5
#define  DNS_STATUS_FMT_ERR                                6
#define  DNS_STATUS_SERVER_FAIL                            7
#define  DNS_STATUS_NAME_NOT_EXIST                         8
#define  DNS_STATUS_BAD_RESPONSE_TYPE                      9
#define  DNS_STATUS_BAD_QUESTION_COUNT                    10
#define  DNS_STATUS_BAD_ANSWER_COUNT                      11
#define  DNS_STATUS_NOT_A_RESPONSE                        12
#define  DNS_STATUS_BAD_RESPONSE_ID                       13
#define  DNS_STATUS_INVALID                              255    /* MUST be last entry in table.                         */


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

typedef  struct  DNSc_CacheStruct {
          CPU_INT32U   Order;
          NET_IP_ADDR  Addr;
          CPU_CHAR     Name[DNSc_MAX_HOSTNAME_SIZE + 1];
} DNSc_CACHE_STRUCT;


typedef  struct  DNSc_ErrStruct {
          CPU_INT32U   Code;
          CPU_CHAR    *Msg;
} DNSc_ERR_STRUCT;


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

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

void  DNSc_Init  (NET_IP_ADDR  srv_addr);


/*
*********************************************************************************************************
*                                         DNSc_GetHostByName()
*
* Description : Convert a host name to the corresponding IP address using DNS service.
*
* Argument(s) : host_name       host name to find.
*
* Return(s)   : IP address of the host (network order).
*
* Caller(s)   : Application.
*
* Note(s)     : This function is a simplified version of gethostbyname().  Given a domain name, this
*               function will first query a locally maintained cache then, if not found there, it will
*               issue a DNS query to retrieve the hosts IP address.
*********************************************************************************************************
*/

NET_IP_ADDR  DNSc_GetHostByName  (CPU_CHAR  *host_name);


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

CPU_INT32U  DNSc_GetLastError  (void);


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

CPU_CHAR  *DNSc_GetLastErrMsg  (void);


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

static  void  DNSc_InitCache  (void);


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

static  CPU_BOOLEAN  DNSc_AddCacheEntry  (CPU_CHAR     *host_name,
                                          NET_IP_ADDR   host_addr);


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
* Caller(s)   : Various.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  DNSc_DelCacheEntryByName  (CPU_CHAR  *host_name);


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
* Caller(s)   : Various.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  DNSc_DelCacheEntryByAddr  (NET_IP_ADDR  host_addr);


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

#ifndef  DNSc_TRACE_LEVEL
#define  DNSc_TRACE_LEVEL                       TRACE_LEVEL_OFF
#endif

#ifndef  DNSc_TRACE
#define  DNSc_TRACE                             printf
#endif

#define  DNSc_TRACE_INFO(x)                   ((DNSc_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(DNSc_TRACE x) : (void)0)
#define  DNSc_TRACE_DBG(x)                    ((DNSc_TRACE_LEVEL >= TRACE_LEVEL_DBG)  ? (void)(DNSc_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                /* DNS client IP port.  Default is 53.                  */
#ifndef  DNSc_CFG_IPPORT
#error  "DNSc_CFG_IPPORT                        illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'dns-c_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on RX.                   */
#ifndef  DNSc_CFG_MAX_RX_TIMEOUT_S
#error  "DNSc_CFG_MAX_RX_TIMEOUT_S              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'dns-c_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on TX.                   */
#ifndef  DNSc_CFG_MAX_TX_TIMEOUT_S
#error  "DNSc_CFG_MAX_TX_TIMEOUT_S              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'dns-c_cfg.h'               "
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif  /* __DNS_C_H__ */
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
