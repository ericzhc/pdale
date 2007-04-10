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
* Filename     : sntp-c.c
* Version      : V1.85
* Programmer(s): AL
*                COP
*                JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>
#include  <net.h>
#include  <app_cfg.h>
#include  <clk.h>
#include  <sntp-c.h>


/*
*********************************************************************************************************
*                                      LOCAL FUNCTIONS PROTOTYPES
*********************************************************************************************************
*/

static  CPU_BOOLEAN  SNTP_Tx (int  sntp_socket, struct sockaddr_in  *sntp_sockaddr);
static  CPU_BOOLEAN  SNTP_Rx (int  sntp_socket, SNTP_PKT  *r_pkt);


/*
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         SNTP_ReqRemoteTime()
*
* Description : Request time to a SNTP server.
*
* Arguments   : server_ip   IP address of the SNTP server.
*               pkt         User-allocated SNTP packet.
*
* Returns     : DEF_TRUE    if time request successfully sent.
*               DEF_FALSE   if time request failure.
*
* Note        : errno is defined if function failed.
*********************************************************************************************************
*/

CPU_BOOLEAN  SNTP_ReqRemoteTime (NET_IP_ADDR   server_ip,
                                 SNTP_PKT     *pkt)
{
    struct  sockaddr_in  sntp_sockaddr;                         /* SNTP server sockaddr.                                */
    int                  sntp_socket;                           /* Socket used to query server.                         */
    CPU_BOOLEAN          result;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (pkt == (SNTP_PKT *)0) {
        return (DEF_FAIL);
    }
#endif

    sntp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);     /* Create SNTP socket.                                  */
    if (sntp_socket == -1) {
        return (DEF_FAIL);
    }

    sntp_sockaddr.sin_family      = AF_INET;                    /* Fill the sockaddr structure.                         */
    sntp_sockaddr.sin_port        = NET_UTIL_HOST_TO_NET_16(SNTPc_CFG_IPPORT);
    sntp_sockaddr.sin_addr.s_addr = NET_UTIL_HOST_TO_NET_32(server_ip);

    result = SNTP_Tx(sntp_socket, &sntp_sockaddr);              /* Send SNTP request.                                   */
    if (result == DEF_FAIL) {
        close(sntp_socket);
        return (DEF_FAIL);
    }

    result = SNTP_Rx(sntp_socket, pkt);                         /* Receive response from SNTP server.                   */
    if (result == DEF_FAIL) {
        close(sntp_socket);
        return (DEF_FAIL);
    }

                                                                /* Set NTP timestamp of the current time.               */
    pkt->RefTSSec   = NET_UTIL_HOST_TO_NET_32(SNTP_GetLocalTime_s());
    pkt->RefTSFract = 0;

    close(sntp_socket);                                         /* Close socket.                                        */
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                      SNTP_GetRemoteTime_s()
*
* Description : Get remote time (NTP timestamp).
*
* Arguments   : pkt     received SNTP packet.
*
* Returns     : Remote time offset from local time.
*********************************************************************************************************
*/

CPU_INT32U  SNTP_GetRemoteTime_s (SNTP_PKT  *pkt)
{
    CPU_FP64    ts_originate;                                   /* Local  timestamp when sending request.               */
    CPU_FP64    ts_rx;                                          /* Remote timestamp when receiving request.             */
    CPU_FP64    ts_tx;                                          /* Remote timestamp when sending result.                */
    CPU_FP64    ts_terminate;                                   /* Local  timestamp when receiving result.              */
    CPU_FP64    local_time_offset;
    CPU_INT32U  local_time;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (pkt == (SNTP_PKT *)0) {
        return (DEF_FAIL);
    }
#endif

    ts_originate = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_OriginateSec)   +
                   (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_OriginateFract) /
                   (CPU_FP64)0x100000000;

    ts_rx        = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_RxSec)          +
                   (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_RxFract)        /
                   (CPU_FP64)0x100000000;

    ts_tx        = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_TxSec)          +
                   (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_TxFract)        /
                   (CPU_FP64)0x100000000;

    ts_terminate = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->RefTSSec)          +
                   (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->RefTSFract)        /
                   (CPU_FP64)0x100000000;

    local_time_offset = ((ts_rx - ts_originate) +               /* Calculate offset from local time.                    */
                         (ts_tx - ts_terminate)) / 2;

    local_time = SNTP_GetLocalTime_s();
    return (local_time + (CPU_INT32U)local_time_offset);        /* Keep only integer part of the offset.                */
}


/*
*********************************************************************************************************
*                                       SNTP_GetRoundTripDly_us()
*
* Description : Get NTP packet round trip delay.
*
* Arguments   : pkt     received SNTP packet.
*
* Returns     : NTP packet round trip delay.
*
* Notes       : If the roundtrip delay is faster than the precision of the
*               system clock, then the roundtrip delay is approximated to 0.
*********************************************************************************************************
*/

CPU_INT32U  SNTP_GetRoundTripDly_us (SNTP_PKT  *pkt)
{
    CPU_FP64  ts_originate;                                     /* Local  timestamp when sending request.               */
    CPU_FP64  ts_rx;                                            /* Remote timestamp when receiving request.             */
    CPU_FP64  ts_tx;                                            /* Remote timestamp when sending result.                */
    CPU_FP64  ts_terminate;                                     /* Local  timestamp when receiving result.              */
    CPU_FP64  round_trip_dly;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (pkt == (SNTP_PKT *)0) {
        return (DEF_FAIL);
    }
#endif

    ts_originate = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_OriginateSec)   +
                   (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_OriginateFract) /
                   (CPU_FP64)0x100000000;

    ts_rx        = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_RxSec)          +
                   (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_RxFract)        /
                   (CPU_FP64)0x100000000;

    ts_tx        = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_TxSec)          +
                   (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->TS_TxFract)        /
                   (CPU_FP64)0x100000000;

    ts_terminate = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->RefTSSec)          +
                   (CPU_FP64)NET_UTIL_NET_TO_HOST_32(pkt->RefTSFract)        /
                   (CPU_FP64)0x100000000;

    round_trip_dly = ((ts_terminate - ts_originate) -           /* Calculate round trip delay.                          */
                      (ts_tx - ts_rx)) * 1000000;

    return ((CPU_INT32U)round_trip_dly);                        /* Keep only integer part of the delay.                 */
}


/*
*********************************************************************************************************
*                                            LOCAL FUNCTIONS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               SNTP_Tx()
*
* Description : Send NTP packet to server.
*
* Arguments   : sntp_socket     socket to send NTP packet.
*               pkt             sent NTP packet.
*
* Returns     : DEF_TRUE    if packet     successfully sent.
*               DEF_FALSE   if packet NOT successfully sent.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  SNTP_Tx (int  sntp_socket, struct sockaddr_in  *sntp_sockaddr)
{
    SNTP_PKT    pkt;                                            /* The NTP packet to send.                              */
    CPU_INT16S  res;


                                                                /*                                                      */
                                                                /* Initialize SNTP_PKT (see RFC2030)                    */
                                                                /*                                                      */
                                                                /* Ccntrol Word (Lower to higher bits)                  */
                                                                /* ------------                                         */
                                                                /*    -> Precision  (8 bits) = 0                        */
                                                                /*    -> Poll Inter (8 bits) = 0                        */
                                                                /*    -> Stratum    (8 bits) = 0                        */
                                                                /*    -> Mode       (3 bits) = 3 (for client mode)      */
                                                                /*    -> Version    (3 bits) = 4 (for version 4)        */
                                                                /*    -> Leap indic (2 bits) = 0 (for no warning)       */
    pkt.CW                = 0x00000023;
    pkt.RootDly           = 0;
    pkt.RootDispersion    = 0;
    pkt.RefID             = 0;
    pkt.RefTSSec          = 0;
    pkt.RefTSFract        = 0;
    pkt.TS_OriginateSec   = 0;
    pkt.TS_OriginateFract = 0;
    pkt.TS_RxSec          = 0;
    pkt.TS_RxFract        = 0;
    pkt.TS_TxSec          = NET_UTIL_HOST_TO_NET_32(SNTP_GetLocalTime_s());
    pkt.TS_TxFract        = 0;

    res = sendto( sntp_socket,                                  /* Send SNTP packet.                                    */
                 (CPU_INT08S *)&pkt,
                  sizeof(SNTP_PKT),
                  0,
                 (struct sockaddr *)sntp_sockaddr,
                  sizeof(struct sockaddr_in));
    if (res <= 0) {
        return (DEF_FAIL);
    }
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                               SNTP_Rx()
*
* Description : Receive NTP packet from server.
*
* Arguments   : sntp_socket     socket to receive NTP packet.
*               pkt             received NTP packet.
*
* Returns     : DEF_TRUE    if packet     successfully received.
*               DEF_FALSE   if packet NOT successfully received.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  SNTP_Rx (int  sntp_socket, SNTP_PKT  *pkt)
{
    struct  sockaddr_in  remote_addr;
    CPU_INT16S           res;
    CPU_INT16S           size;


    size = sizeof(struct sockaddr_in);
    res  = recvfrom( sntp_socket,
                    (CPU_INT08U *)pkt,
                     sizeof(SNTP_PKT),
                     0,
                    (struct sockaddr *)&remote_addr,
                    (int *)&size);

    if (res <= 0) {
        return (DEF_FAIL);
    }
    return (DEF_OK);
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
