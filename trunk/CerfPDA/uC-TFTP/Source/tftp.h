/*
*********************************************************************************************************
*                                               uC/TFTPs
*                                 Trivial File Transfer Protocol Server
*
*                             (c) Copyright 2003-2004, Micrium, Weston, FL
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
* Filename      : tftp.h
* Programmer(s) : JJL
* Version       : V1.10
*********************************************************************************************************
*/

#ifdef   TFTPs_GLOBALS
#define  TFTPs_EXT
#else
#define  TFTPs_EXT  extern
#endif

/*
*********************************************************************************************************
*                                   TFTP (client or server) SPECIFIC
*                                              CONSTANTS
*********************************************************************************************************
*/

#define  TFTP_PKT_OFFSET_OPCODE         0
#define  TFTP_PKT_OFFSET_FILENAME       2
#define  TFTP_PKT_OFFSET_BLK_NBR        2
#define  TFTP_PKT_OFFSET_ERR_CODE       2
#define  TFTP_PKT_OFFSET_ERR_MSG        4
#define  TFTP_PKT_OFFSET_DATA           4

#define  TFTP_PKT_SIZE_OPCODE           2
#define  TFTP_PKT_SIZE_BLK_NBR          2
#define  TFTP_PKT_SIZE_ERR_CODE         2
#define  TFTP_PKT_SIZE_FILENAME_NUL     1
#define  TFTP_PKT_SIZE_MODE_NUL         1


                                                  /* ---------- TFTP opcodes (see Stevens pg 466) ---------- */
#define  TFTP_OPCODE_RD_REQ             1         /* Read                                                    */
#define  TFTP_OPCODE_WR_REQ             2         /* Write                                                   */
#define  TFTP_OPCODE_DATA               3         /* Data                                                    */
#define  TFTP_OPCODE_ACK                4         /* Acknowledge                                             */
#define  TFTP_OPCODE_ERR                5         /* Error                                                   */


                                                  /* ------------------ TFTP error codes ------------------- */
#define  TFTP_ERR_NONE                  0
#define  TFTP_ERR_RD_REQ                1
#define  TFTP_ERR_WR_REQ                2
#define  TFTP_ERR_DATA                  3
#define  TFTP_ERR_ACK                   4
#define  TFTP_ERR_ERR                   5
#define  TFTP_ERR_FILE_NOT_FOUND        6
#define  TFTP_ERR_TX                    7
#define  TFTP_ERR_FILE_RD               8
#define  TFTP_ERR_TIMED_OUT             9
#define  TFTP_ERR_NO_SOCK              10         /* No socket available                                     */
#define  TFTP_ERR_CANT_BIND            11         /* Could not bind to the TFTPs port                        */


/*
*********************************************************************************************************
*                                           TFTPs SPECIFIC
*                                              CONSTANTS
*********************************************************************************************************
*/

#define  TFTPs_PORT_NBR_SERVER         69



                                                  /* ---- TFTP Server error codes: (see Stevens pg 467) ---- */
#define  TFTPs_ERR_ERR_STR              0         /* Not defined                                             */
#define  TFTPs_ERR_FILE_NOT_FOUND       1         /* File not found                                          */
#define  TFTPs_ERR_ACCESS_VIOLATION     2         /* Access violation                                        */
#define  TFTPs_ERR_DISK_FULL            3         /* Disk full                                               */
#define  TFTPs_ERR_ILLEGAL_OP           4         /* Illegal TFTP operation                                  */
#define  TFTPs_ERR_BAD_PORT_NBR         5         /* Unknown port number                                     */
#define  TFTPs_ERR_FILE_EXISTS          6         /* File already exists                                     */
#define  TFTPs_ERR_NO_SUCH_USER         7         /* No such user                                            */

                                                  /* ------------------ TFTP Server modes ------------------ */
#define  TFTPs_MODE_OCTET               1 
#define  TFTPs_MODE_NETASCII            2 

                                                  /* ------------------ TFTP Server states ----------------- */
#define  TFTPs_STATE_IDLE               0 
#define  TFTPs_STATE_DATA_RD            1 
#define  TFTPs_STATE_DATA_WR            2 


#define  TFTPs_BUF_SIZE              (512 + TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR)

/*
*********************************************************************************************************
*                                            TFTP SPECIFIC
*                                             DATA TYPES
*********************************************************************************************************
*/

typedef  CPU_INT16U  TFTP_ERR;

/*
*********************************************************************************************************
*                                        TFTPS GLOBAL VARIABLES
*********************************************************************************************************
*/

TFTPs_EXT  NET_TMR_TICK         TFTPs_BlockingTimeout;

TFTPs_EXT  CPU_INT16U           TFTPs_RxBlkNbr;                       /* Current block number received              */
TFTPs_EXT  CPU_INT08U           TFTPs_RxMsgBuf[TFTPs_BUF_SIZE];       /* Incoming packet buffer                     */
TFTPs_EXT  int                  TFTPs_RxMsgCtr;                       /* Number of messages received                */
TFTPs_EXT  int                  TFTPs_RxMsgLen;

TFTPs_EXT  CPU_INT16U           TFTPs_TxBlkNbr;                       /* Current block number being sent            */
TFTPs_EXT  CPU_INT08U           TFTPs_TxMsgBuf[TFTPs_BUF_SIZE];       /* Outgoing packet buffer                     */
TFTPs_EXT  CPU_INT16U           TFTPs_TxMsgCtr;
TFTPs_EXT  int                  TFTPs_TxMsgLen;

TFTPs_EXT  NET_SOCK_ADDR_IP     TFTPs_SockAddr;
TFTPs_EXT  int                  TFTPs_SockAddrLen;
TFTPs_EXT  int                  TFTPs_SockID;

TFTPs_EXT  CPU_INT08U           TFTPs_State;                          /* Current state of TFTPs state machine       */

TFTPs_EXT  CPU_INT16U           TFTPs_OpCode;

TFTPs_EXT  FS_FILE             *TFTPs_FileHandle;                     /* File handle of currently opened file       */

TFTPs_EXT  CPU_BOOLEAN          TFTPs_ServerEn;                   

TFTPs_EXT  CPU_INT16U           TFTPs_TimeoutSec;
TFTPs_EXT  CPU_INT32U           TFTPs_TimeoutCtr;
TFTPs_EXT  CPU_INT32U           TFTPs_WrSize;


#if TFTPs_DISP_EN > 0
TFTPs_EXT  CPU_CHAR             TFTPs_DispTbl[TFTPs_DISP_MAX_Y][TFTPs_DISP_MAX_X];
#endif

/*
*********************************************************************************************************
*                                           TFTPs SPECIFIC
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         TFTPs_En(void);
void         TFTPs_Dis(void);
CPU_BOOLEAN  TFTPs_Init(NET_TMR_TICK time);

TFTP_ERR     TFTPs_Task(void);

#if TFTPs_DISP_EN > 0
void         TFTPs_Disp(void);
#endif

#if (TFTPs_DISP_EN > 0) && (TFTPs_TRACE_EN > 0)
void         TFTPs_DispTrace(void);
#endif

/*
*********************************************************************************************************
*                                           TFTPs SPECIFIC
*                                 RTOS INTERFACE FUNCTION PROTOTYPES
*********************************************************************************************************
*/

CPU_BOOLEAN  TFTPs_OS_Init(void);
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	   		 		  	   		   	 			       	   	  		      		      		  	 	 		  				 	      		   	  		  				 	  	   		      		    		 	       	  	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		      	   	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
