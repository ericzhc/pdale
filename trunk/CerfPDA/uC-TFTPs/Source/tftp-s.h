/*
*********************************************************************************************************
*                                               uC/TFTPs
*                                Trivial File Transfer Protocol (server)
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
*                                              TFTP SERVER
*
* Filename      : tftp-s.h
* Version       : V1.85
* Programmer(s) : JJL
*                 JDH
*********************************************************************************************************
*/

#ifndef  __TFTP_S_H__
#define  __TFTP_S_H__


/*
*********************************************************************************************************
*                                        TFTP (client or server)
*                                           SPECIFIC DEFINES
*********************************************************************************************************
*/

#define  TFTP_PKT_OFFSET_OPCODE                            0
#define  TFTP_PKT_OFFSET_FILENAME                          2
#define  TFTP_PKT_OFFSET_BLK_NBR                           2
#define  TFTP_PKT_OFFSET_ERR_CODE                          2
#define  TFTP_PKT_OFFSET_ERR_MSG                           4
#define  TFTP_PKT_OFFSET_DATA                              4

#define  TFTP_PKT_SIZE_OPCODE                              2
#define  TFTP_PKT_SIZE_BLK_NBR                             2
#define  TFTP_PKT_SIZE_ERR_CODE                            2
#define  TFTP_PKT_SIZE_FILENAME_NUL                        1
#define  TFTP_PKT_SIZE_MODE_NUL                            1

                                                                /* ---- TFTP opcodes (see Stevens p. 466) ------------- */
#define  TFTP_OPCODE_RD_REQ                                1    /* Read                                                 */
#define  TFTP_OPCODE_WR_REQ                                2    /* Write                                                */
#define  TFTP_OPCODE_DATA                                  3    /* Data                                                 */
#define  TFTP_OPCODE_ACK                                   4    /* Acknowledge                                          */
#define  TFTP_OPCODE_ERR                                   5    /* Error                                                */

                                                                /* ---- TFTP error codes ------------------------------ */
#define  TFTP_ERR_NONE                                     0
#define  TFTP_ERR_RD_REQ                                   1
#define  TFTP_ERR_WR_REQ                                   2
#define  TFTP_ERR_DATA                                     3
#define  TFTP_ERR_ACK                                      4
#define  TFTP_ERR_ERR                                      5
#define  TFTP_ERR_FILE_NOT_FOUND                           6
#define  TFTP_ERR_TX                                       7
#define  TFTP_ERR_FILE_RD                                  8
#define  TFTP_ERR_TIMED_OUT                                9
#define  TFTP_ERR_NO_SOCK                                 10    /* No socket available.                                 */
#define  TFTP_ERR_CANT_BIND                               11    /* Could not bind to the TFTPs port.                    */


/*
*********************************************************************************************************
*                                             TFTP (server)
*                                           SPECIFIC DEFINES
*********************************************************************************************************
*/

                                                                /* ---- TFTP Server error codes: (see Stevens p. 467) - */
#define  TFTPs_ERR_ERR_STR                                 0    /* Not defined.                                         */
#define  TFTPs_ERR_FILE_NOT_FOUND                          1    /* File not found.                                      */
#define  TFTPs_ERR_ACCESS_VIOLATION                        2    /* Access violation.                                    */
#define  TFTPs_ERR_DISK_FULL                               3    /* Disk full.                                           */
#define  TFTPs_ERR_ILLEGAL_OP                              4    /* Illegal TFTP operation.                              */
#define  TFTPs_ERR_BAD_PORT_NBR                            5    /* Unknown port number.                                 */
#define  TFTPs_ERR_FILE_EXISTS                             6    /* File already exists.                                 */
#define  TFTPs_ERR_NO_SUCH_USER                            7    /* No such user.                                        */

                                                                /* ---- TFTP Server modes ----------------------------- */
#define  TFTPs_MODE_OCTET                                  1
#define  TFTPs_MODE_NETASCII                               2

                                                                /* ---- TFTP Server states ---------------------------- */
#define  TFTPs_STATE_IDLE                                  0
#define  TFTPs_STATE_DATA_RD                               1
#define  TFTPs_STATE_DATA_WR                               2

#define  TFTPs_BLOCK_SIZE                                512
#define  TFTPs_BUF_SIZE                         (TFTPs_BLOCK_SIZE + TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR)


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

typedef  CPU_INT16U  TFTP_ERR;


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         TFTPs_En         (void);
void         TFTPs_Dis        (void);
CPU_BOOLEAN  TFTPs_Init       (void);

void         TFTPs_Task       (void        *p_arg);

#if (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
    void     TFTPs_Disp       (void);
    void     TFTPs_DispTrace  (void);
#endif


/*
*********************************************************************************************************
*                                       RTOS INTERFACE FUNCTIONS
*                                           (see tftp-s_os.c)
*********************************************************************************************************
*/

CPU_BOOLEAN  TFTPs_OS_Init    (void        *p_arg);


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

#ifndef  TFTPs_TRACE_LEVEL
#define  TFTPs_TRACE_LEVEL                      TRACE_LEVEL_OFF
#endif

#ifndef  TFTPs_TRACE
#define  TFTPs_TRACE                            printf
#endif

#define  TFTPs_TRACE_INFO(x)                  ((TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(TFTPs_TRACE x) : (void)0)
#define  TFTPs_TRACE_DBG(x)                   ((TFTPs_TRACE_LEVEL >= TRACE_LEVEL_DBG)  ? (void)(TFTPs_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                /* Task name for debugging purposes.                    */
#ifndef  TFTPs_OS_CFG_TASK_NAME
#error  "TFTPs_OS_CFG_TASK_NAME                 illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'tftp-s_cfg.h'              "
#endif

                                                                /* Task priority.                                       */
#ifndef  TFTPs_OS_CFG_TASK_PRIO
#error  "TFTPs_OS_CFG_TASK_PRIO                 illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'tftp-s_cfg.h'              "
#endif

                                                                /* Task stack size.                                     */
#ifndef  TFTPs_OS_CFG_TASK_STK_SIZE
#error  "TFTPs_OS_CFG_TASK_STK_SIZE             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'tftp-s_cfg.h'              "
#endif

                                                                /* TFTP server IP port.  Default is 69.                 */
#ifndef  TFTPs_CFG_IPPORT
#error  "TFTPs_CFG_IPPORT                       illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'tftp-s_cfg.h'              "
#endif

                                                                /* Maximum inactivity time (s) on RX.                   */
#ifndef  TFTPs_CFG_MAX_RX_TIMEOUT_S
#error  "TFTPs_CFG_MAX_RX_TIMEOUT_S             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'tftp-s_cfg.h'              "
#endif

                                                                /* Maximum inactivity time (s) on TX.                   */
#ifndef  TFTPs_CFG_MAX_TX_TIMEOUT_S
#error  "TFTPs_CFG_MAX_TX_TIMEOUT_S             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'tftp-s_cfg.h'              "
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif  /* __TFTP_S_H__ */
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
