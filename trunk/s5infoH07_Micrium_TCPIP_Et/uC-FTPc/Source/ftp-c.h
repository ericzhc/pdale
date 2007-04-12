/*
*********************************************************************************************************
*                                                uC/FTPc
*                                    File Transfer Protocol (client)
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
*                                              FTP CLIENT
*
* Filename      : ftp-c.h
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

#ifndef  __FTP_C_H__
#define  __FTP_C_H__


/*
*********************************************************************************************************
*                                                  FTP
*                                           SPECIFIC DEFINES
*********************************************************************************************************
*/

#define  FTPc_MAX_PATH_LEN                               255    /* Maximum length for path/file name.                   */
#define  FTPc_NET_BUF_SIZE                              1024    /* Network buffer size.                                 */


/*
*********************************************************************************************************
*                                             FTP COMMANDS
*********************************************************************************************************
*/

#define  FTP_CMD_NOOP                                      0
#define  FTP_CMD_QUIT                                      1
#define  FTP_CMD_REIN                                      2
#define  FTP_CMD_SYST                                      3
#define  FTP_CMD_FEAT                                      4
#define  FTP_CMD_HELP                                      5
#define  FTP_CMD_USER                                      6
#define  FTP_CMD_PASS                                      7
#define  FTP_CMD_MODE                                      8
#define  FTP_CMD_TYPE                                      9
#define  FTP_CMD_STRU                                     10
#define  FTP_CMD_PASV                                     11
#define  FTP_CMD_PORT                                     12
#define  FTP_CMD_PWD                                      13
#define  FTP_CMD_CWD                                      14
#define  FTP_CMD_CDUP                                     15
#define  FTP_CMD_MKD                                      16
#define  FTP_CMD_RMD                                      17
#define  FTP_CMD_NLST                                     18
#define  FTP_CMD_LIST                                     19
#define  FTP_CMD_RETR                                     20
#define  FTP_CMD_STOR                                     21
#define  FTP_CMD_APPE                                     22
#define  FTP_CMD_REST                                     23
#define  FTP_CMD_DELE                                     24
#define  FTP_CMD_RNFR                                     25
#define  FTP_CMD_RNTO                                     26
#define  FTP_CMD_SIZE                                     27
#define  FTP_CMD_MDTM                                     28
#define  FTP_CMD_MAX                                      29    /* This line MUST be the LAST!                          */


/*
*********************************************************************************************************
*                                          FTP REPLY MESSAGES
*********************************************************************************************************
*/

#define  FTP_REPLY_CODE_OKAYOPENING                      150
#define  FTP_REPLY_CODE_OKAY                             200
#define  FTP_REPLY_CODE_SYSTEMSTATUS                     211
#define  FTP_REPLY_CODE_FILESTATUS                       213
#define  FTP_REPLY_CODE_HELPMESSAGE                      214
#define  FTP_REPLY_CODE_SYSTEMTYPE                       215
#define  FTP_REPLY_CODE_SERVERREADY                      220
#define  FTP_REPLY_CODE_SERVERCLOSING                    221
#define  FTP_REPLY_CODE_CLOSINGSUCCESS                   226
#define  FTP_REPLY_CODE_ENTERPASVMODE                    227
#define  FTP_REPLY_CODE_LOGGEDIN                         230
#define  FTP_REPLY_CODE_ACTIONCOMPLETE                   250
#define  FTP_REPLY_CODE_PATHNAME                         257
#define  FTP_REPLY_CODE_NEEDPASSWORD                     331
#define  FTP_REPLY_CODE_NEEDMOREINFO                     350
#define  FTP_REPLY_CODE_NOSERVICE                        421
#define  FTP_REPLY_CODE_CANTOPENDATA                     425
#define  FTP_REPLY_CODE_CLOSEDCONNABORT                  426
#define  FTP_REPLY_CODE_PARMSYNTAXERR                    501
#define  FTP_REPLY_CODE_CMDNOSUPPORT                     502
#define  FTP_REPLY_CODE_CMDBADSEQUENCE                   503
#define  FTP_REPLY_CODE_PARMNOSUPPORT                    504
#define  FTP_REPLY_CODE_NOTLOGGEDIN                      530
#define  FTP_REPLY_CODE_NOTFOUND                         550
#define  FTP_REPLY_CODE_ACTIONABORTED                    551
#define  FTP_REPLY_CODE_NOSPACE                          552
#define  FTP_REPLY_CODE_NAMEERR                          553


/*
*********************************************************************************************************
*                                             FTP DATA TYPE
*********************************************************************************************************
*/

                                                                /* Data type "IMAGE" supported only.                    */
#define  FTP_TYPE_ASCII                                  'A'
#define  FTP_TYPE_EBCDIC                                 'E'
#define  FTP_TYPE_IMAGE                                  'I'
#define  FTP_TYPE_LOCAL                                  'L'


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

                                                                /* This structure is used to build a table of command   */
                                                                /* codes and their corresponding string.                */
typedef struct  FTPc_CmdStruct {
    CPU_INT08U   CmdCode;
    CPU_CHAR    *CmdStr;
}  FTPc_CMD_STRUCT;


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPc_Open     (NET_IP_ADDR   server_ip,
                            CPU_INT16U    server_port,
                            CPU_CHAR     *user,
                            CPU_CHAR     *pass,
                            NET_ERR      *p_net_err);

CPU_BOOLEAN  FTPc_Close    (NET_ERR      *p_net_err);


CPU_BOOLEAN  FTPc_RecvBuf  (CPU_CHAR     *remote_file_name,
                            CPU_INT08U   *local_file,
                            CPU_INT32U    local_file_size,
                            NET_ERR      *p_net_err);

CPU_BOOLEAN  FTPc_SendBuf  (CPU_CHAR     *remote_file_name,
                            CPU_INT08U   *local_file,
                            CPU_INT32U    local_file_size,
                            CPU_BOOLEAN   append,
                            NET_ERR      *p_net_err);


CPU_BOOLEAN  FTPc_RecvFile (CPU_CHAR     *remote_file_name,
                            CPU_CHAR     *local_file_name,
                            NET_ERR      *p_net_err);

CPU_BOOLEAN  FTPc_SendFile (CPU_CHAR     *remote_file_name,
                            CPU_CHAR     *local_file_name,
                            CPU_BOOLEAN   append,
                            NET_ERR      *p_net_err);


/*
*********************************************************************************************************
*                                       RTOS INTERFACE FUNCTIONS
*                                           (see ftp-c_os.c)
*********************************************************************************************************
*/

void         FTPc_OS_TxDly (void);


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

#ifndef  FTPc_TRACE_LEVEL
#define  FTPc_TRACE_LEVEL                       TRACE_LEVEL_OFF
#endif

#ifndef  FTPc_TRACE
#define  FTPc_TRACE                             printf
#endif

#define  FTPc_TRACE_INFO(x)                   ((FTPc_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(FTPc_TRACE x) : (void)0)
#define  FTPc_TRACE_DBG(x)                    ((FTPc_TRACE_LEVEL >= TRACE_LEVEL_DBG)  ? (void)(FTPc_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                /* FTP Control IP port.  Default is 21.                 */
#ifndef  FTPc_CFG_CTRL_IPPORT
#error  "FTPc_CFG_CTRL_IPPORT                   illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* FTP Data IP port.  Default is 20.                    */
#ifndef  FTPc_CFG_DTP_IPPORT
#error  "FTPc_CFG_DTP_IPPORT                    illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on CONNECT.              */
#ifndef  FTPc_CFG_CTRL_MAX_CONN_TIMEOUT_S
#error  "FTPc_CFG_CTRL_MAX_CONN_TIMEOUT_S       illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on RX.                   */
#ifndef  FTPc_CFG_CTRL_MAX_RX_TIMEOUT_S
#error  "FTPc_CFG_CTRL_MAX_RX_TIMEOUT_S         illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on TX.                   */
#ifndef  FTPc_CFG_CTRL_MAX_TX_TIMEOUT_S
#error  "FTPc_CFG_CTRL_MAX_TX_TIMEOUT_S         illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on ACCEPT.               */
#ifndef  FTPc_CFG_DTP_MAX_ACCEPT_TIMEOUT_S
#error  "FTPc_CFG_DTP_MAX_ACCEPT_TIMEOUT_S      illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on CONNECT.              */
#ifndef  FTPc_CFG_DTP_MAX_CONN_TIMEOUT_S
#error  "FTPc_CFG_DTP_MAX_CONN_TIMEOUT_S        illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on RX.                   */
#ifndef  FTPc_CFG_DTP_MAX_RX_TIMEOUT_S
#error  "FTPc_CFG_DTP_MAX_RX_TIMEOUT_S          illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on TX.                   */
#ifndef  FTPc_CFG_DTP_MAX_TX_TIMEOUT_S
#error  "FTPc_CFG_DTP_MAX_TX_TIMEOUT_S          illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum number of retries on CONNECT.                */
#ifndef  FTPc_CFG_CTRL_MAX_CONN_RETRY
#error  "FTPc_CFG_CTRL_MAX_CONN_RETRY           illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum number of retries on RX.                     */
#ifndef  FTPc_CFG_CTRL_MAX_RX_RETRY
#error  "FTPc_CFG_CTRL_MAX_RX_RETRY             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum number of retries on TX.                     */
#ifndef  FTPc_CFG_CTRL_MAX_TX_RETRY
#error  "FTPc_CFG_CTRL_MAX_TX_RETRY             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum number of retries on ACCEPT.                 */
#ifndef  FTPc_CFG_DTP_MAX_ACCEPT_RETRY
#error  "FTPc_CFG_DTP_MAX_ACCEPT_RETRY          illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum number of retries on CONNECT.                */
#ifndef  FTPc_CFG_DTP_MAX_CONN_RETRY
#error  "FTPc_CFG_DTP_MAX_CONN_RETRY            illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum number of retries on RX.                     */
#ifndef  FTPc_CFG_DTP_MAX_RX_RETRY
#error  "FTPc_CFG_DTP_MAX_RX_RETRY              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum number of retries on TX.                     */
#ifndef  FTPc_CFG_DTP_MAX_TX_RETRY
#error  "FTPc_CFG_DTP_MAX_TX_RETRY              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum length for user name.                        */
#ifndef  FTPc_CFG_MAX_USER_LEN
#error  "FTPc_CFG_MAX_USER_LEN                  illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* Maximum length for password.                         */
#ifndef  FTPc_CFG_MAX_PASS_LEN
#error  "FTPc_CFG_MAX_PASS_LEN                  illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif

                                                                /* If 1, interface functions using FS are enabled.      */
                                                                /* If 0, only functions using RAM buffers are enabled.  */
#ifndef  FTPc_CFG_USE_FS
#error  "FTPc_CFG_USE_FS                        illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-c_cfg.h'               "
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif  /* __FTP_C_H__ */
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
