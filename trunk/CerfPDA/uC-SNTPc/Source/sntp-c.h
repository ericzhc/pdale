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
* Filename     : sntp-c.h
* Version      : V1.85
* Programmer(s): AL
*                COP
*                JDH
*********************************************************************************************************
*/

#ifndef  __SNTP_C_H__
#define  __SNTP_C_H__


/*
*********************************************************************************************************
*                                           GLOBAL STRUCTURES
*********************************************************************************************************
*/

typedef struct                                                  /* NTP Packet                                           */
{
    CPU_INT32U  CW;                                             /* Control bits for the NTP packet.                     */
    CPU_INT32U  RootDly;                                        /* Rountrip delay.                                      */
    CPU_INT32U  RootDispersion;                                 /* Nominal error returned by server.                    */
    CPU_INT32U  RefID;                                          /* Server reference identifier.                         */
    CPU_INT32U  RefTSSec;                                       /* Timestamp of the last sync.                          */
    CPU_INT32U  RefTSFract;                                     /*   Timestamp fractionnal part.                        */
    CPU_INT32U  TS_OriginateSec;                                /* Local timestamp when sending request.                */
    CPU_INT32U  TS_OriginateFract;                              /*   Timestamp fractionnal part.                        */
    CPU_INT32U  TS_RxSec;                                       /* Remote timestamp when receiving request.             */
    CPU_INT32U  TS_RxFract;                                     /*   Timestamp fractionnal part.                        */
    CPU_INT32U  TS_TxSec;                                       /* Remote timestamp when sending result.                */
    CPU_INT32U  TS_TxFract;                                     /*   Timestamp fractionnal part.                        */
} SNTP_PKT;


/*
*********************************************************************************************************
*                                              PROTOTYPES
*********************************************************************************************************
*/

CPU_BOOLEAN  SNTP_ReqRemoteTime      (NET_IP_ADDR   server_ip,  /* Request remote time.                                 */
                                      SNTP_PKT     *pkt);
CPU_INT32U   SNTP_GetRemoteTime_s    (SNTP_PKT     *pkt);       /* Get remote time (NTP timestamp).                     */
CPU_INT32U   SNTP_GetRoundTripDly_us (SNTP_PKT     *pkt);       /* Get packet round trip delay.                         */


/*
*********************************************************************************************************
*                             PROTOTYPES (to declare in user's application)
*********************************************************************************************************
*/

CPU_INT32U   SNTP_GetLocalTime_s     (void);


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

#ifndef  SNTPc_TRACE_LEVEL
#define  SNTPc_TRACE_LEVEL                      TRACE_LEVEL_OFF
#endif

#ifndef  SNTPc_TRACE
#define  SNTPc_TRACE                            printf
#endif

#define  SNTPc_TRACE_INFO(x)                  ((SNTPc_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(SNTPc_TRACE x) : (void)0)
#define  SNTPc_TRACE_DBG(x)                   ((SNTPc_TRACE_LEVEL >= TRACE_LEVEL_DBG)  ? (void)(SNTPc_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                /* Define the SNTP client IP port.  Default is 123.     */
#ifndef  SNTPc_CFG_IPPORT
#error  "SNTPc_CFG_IPPORT                       illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'sntp-c_cfg.h'              "
#endif

                                                                /* Maximum inactivity time (s) on RX.                   */
#ifndef  SNTPc_CFG_MAX_RX_TIMEOUT_S
#error  "SNTPc_CFG_MAX_RX_TIMEOUT_S             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'sntp-c_cfg.h'              "
#endif

                                                                /* Maximum inactivity time (s) on TX.                   */
#ifndef  SNTPc_CFG_MAX_TX_TIMEOUT_S
#error  "SNTPc_CFG_MAX_TX_TIMEOUT_S             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'sntp-c_cfg.h'              "
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif  /* __SNTP_C_H__ */
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
