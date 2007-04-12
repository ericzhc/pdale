/*
*********************************************************************************************************
*                                                uC/FTPs
*                                    File Transfer Protocol (server)
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
*                                              FTP SERVER
*
* Filename      : ftp-s.h
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

#ifndef  __FTP_S_H__
#define  __FTP_S_H__


/*
*********************************************************************************************************
*                                                  FTP
*                                           SPECIFIC DEFINES
*********************************************************************************************************
*/

                                                                /* The maximum number of control tasks supported.       */
#define  FTPs_CTRL_TASKS_MAX                               1    /* Actually 1 (serving 1 client at a time).             */
                                                                /* The maximum number of data transfer tasks supported. */

#define  FTPs_CTRL_CONN_Q_SIZE                             3    /* Control connection queue size.                       */
#define  FTPs_DTP_CONN_Q_SIZE                              1    /* Data transfer protocol connection queue size.        */

#define  FTPs_MAX_PATH_LEN                               255    /* Maximum length for path/file name.                   */
#define  FTPs_NET_BUF_LEN                               1024    /* Network buffer length.                               */

#define  FTPs_PATH_SEP_CHAR                     '/'             /* Define the path separator character used FTP.        */
#define  FTPs_FS_PATH_SEP_CHAR                  '\\'            /* Define the path separator character used by the      */
                                                                /* underlying filesystem.                               */
#define  FTPs_ROOT_PATH                         "/"             /* Define the root path name.                           */
#define  FTPs_CURRENT_PATH                      "."             /* Define the current path name.                        */
#define  FTPs_PARENT_PATH                       ".."            /* Define the parent path name.                         */


/*
*********************************************************************************************************
*                                             SERVER STATES
*********************************************************************************************************
*/

#define  FTPs_STATE_LOGOUT                                 0
#define  FTPs_STATE_LOGIN                                  1
#define  FTPs_STATE_GOTUSER                                2
#define  FTPs_STATE_GOTRNFR                                3
#define  FTPs_STATE_GOTREST                                4
#define  FTPs_STATE_MAX                                    5    /* This line MUST be the LAST!                          */


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

#define  FTP_REPLY_OKAYOPENING                             0
#define  FTP_REPLY_OKAY                                    1
#define  FTP_REPLY_SYSTEMSTATUS                            2
#define  FTP_REPLY_FILESTATUS                              3
#define  FTP_REPLY_HELPMESSAGE                             4
#define  FTP_REPLY_SYSTEMTYPE                              5
#define  FTP_REPLY_SERVERREADY                             6
#define  FTP_REPLY_SERVERCLOSING                           7
#define  FTP_REPLY_CLOSINGSUCCESS                          8
#define  FTP_REPLY_ENTERPASVMODE                           9
#define  FTP_REPLY_LOGGEDIN                               10
#define  FTP_REPLY_ACTIONCOMPLETE                         11
#define  FTP_REPLY_PATHNAME                               12
#define  FTP_REPLY_NEEDPASSWORD                           13
#define  FTP_REPLY_NEEDMOREINFO                           14
#define  FTP_REPLY_NOSERVICE                              15
#define  FTP_REPLY_CANTOPENDATA                           16
#define  FTP_REPLY_CLOSEDCONNABORT                        17
#define  FTP_REPLY_PARMSYNTAXERR                          18
#define  FTP_REPLY_CMDNOSUPPORT                           19
#define  FTP_REPLY_CMDBADSEQUENCE                         20
#define  FTP_REPLY_PARMNOSUPPORT                          21
#define  FTP_REPLY_NOTLOGGEDIN                            22
#define  FTP_REPLY_NOTFOUND                               23
#define  FTP_REPLY_ACTIONABORTED                          24
#define  FTP_REPLY_NOSPACE                                25
#define  FTP_REPLY_NAMEERR                                26
#define  FTP_REPLY_MAX                                    27    /* This line MUST be the LAST!                          */

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
*                                           FTP TRANSFER MODE
*********************************************************************************************************
*/

                                                                /* Transfer mode "STREAM" supported only.               */
#define  FTP_MODE_STREAM                        'S'
#define  FTP_MODE_BLOCK                         'B'
#define  FTP_MODE_COMPRESSED                    'C'


/*
*********************************************************************************************************
*                                             FTP DATA TYPE
*********************************************************************************************************
*/

                                                                /* Data type "ASCII" and "IMAGE" supported only.        */
#define  FTP_TYPE_ASCII                         'A'
#define  FTP_TYPE_EBCDIC                        'E'
#define  FTP_TYPE_IMAGE                         'I'
#define  FTP_TYPE_LOCAL                         'L'


/*
*********************************************************************************************************
*                                            FTP ASCII FORM
*********************************************************************************************************
*/

                                                                /* FTP ASCII form "NON_PRINT" supported only.           */
#define  FTP_FORM_NONPRINT                      'N'
#define  FTP_FORM_TELNET                        'T'
#define  FTP_FORM_CARGCTRL                      'C'


/*
*********************************************************************************************************
*                                          FTP DATA STRUCTURE
*********************************************************************************************************
*/

                                                                /* FTP data structure "FILE" supported only.            */
#define  FTP_STRU_FILE                          'F'
#define  FTP_STRU_RECORD                        'R'
#define  FTP_STRU_PAGE                          'P'


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

                                                                /* This structure is used to build a table of command   */
                                                                /* codes and their corresponding string.  The context   */
                                                                /* is the state(s) in which the command is allowed.     */
typedef struct  FTPs_CmdStruct {
    CPU_INT08U         CmdCode;
    CPU_CHAR          *CmdStr;
    CPU_BOOLEAN        CmdCntxt[FTPs_STATE_MAX];
}  FTPs_CMD_STRUCT;

                                                                /* This structure is used to build a table of reply     */
                                                                /* codes and their corresponding messages.              */
typedef struct  FTPs_ReplyStruct {
    CPU_INT16U         ReplyCode;
    CPU_CHAR          *ReplyStr;
}  FTPs_REPLY_STRUCT;

                                                                /* A structure of this type is created for each         */
                                                                /* established connection with the FTP server.          */
                                                                /* A pointer to it is passed around for use by the      */
                                                                /* various functions within the server.                 */
typedef  struct  FTPs_SessionStruct {
    CPU_INT32S         CtrlSockID;
    CPU_INT08U         CtrlState;
    CPU_INT08U         CtrlCmd;
    CPU_CHAR          *CtrlCmdArgs;

    NET_SOCK_ADDR_IP   DtpSockAddr;
    CPU_INT32S         DtpSockID;
    CPU_INT32S         DtpPasvSockID;
    CPU_BOOLEAN        DtpPasv;
    CPU_INT08U         DtpMode;
    CPU_INT08U         DtpType;
    CPU_INT08U         DtpForm;
    CPU_INT08U         DtpStru;
    CPU_INT08U         DtpCmd;
    CPU_INT32U         DtpOffset;

    CPU_CHAR           User    [FTPs_CFG_MAX_USER_LEN];
    CPU_CHAR           Pass    [FTPs_CFG_MAX_PASS_LEN];

    CPU_CHAR           BasePath[FTPs_MAX_PATH_LEN];
    CPU_CHAR           RelPath [FTPs_MAX_PATH_LEN];
    CPU_CHAR           CurEntry[FTPs_MAX_PATH_LEN];
}  FTPs_SESSION_STRUCT;


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPs_Init (NET_IP_ADDR   public_addr,              /* FTPs module initialization.                          */
                        NET_PORT_NBR  public_port);

void         FTPs_Srv_Task  (void  *p_arg);                     /* Server task: waits for clients to connect.           */

void         FTPs_Ctrl_Task (void  *p_arg);                     /* Control task: control session with the client.       */


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                               DEFINED IN USER'S APPLICATION (CALLBACKS)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            FTPs_AuthUser()
*
* Description : Authenticate User.
*
* Argument(s) : ftp_session     structure that contains FTP session states and control data.
*
* Return(s)   : DEF_OK:         authentication succedded.
*               DEF_FAIL:       authentication failed.
*
* Caller(s)   : FTPs_Ctrl_Task()
*
* Note        : The application SHOULD use the User and Pass fields of the ftp_session structure to
*               authorize or not the FTP client to connect.
*
*               If the FTP client is authorized to connect, the application MUST set two fields of
*               the ftp_session structure: BasePath and RelPath.
*               1. BasePath is defined for each user and represent the higher level of path that the
*                  user can access.  Ex.: if BasePath = "/FTPROOT", when user does "CWD /", the
*                  system will actually points to "/FTPROOT" in the filesystem, but the user see "/".
*               2. BasePath can be set to "/".  With this setting, user will see all the filesystem.
*                  This configuration is, however, insecure if you want to hide some files to user.
*               3. RelPath is the current path that the user see.  Is is concatened to the BasePath
*                  for filesystem translation.  Ex.: if user types "CWD /test" and base_path is
*                  "/TFTPROOT", rel_path will be "/test", but the directory accessed on filesystem
*                  will be "/TFTPROOT/test".
*
*               BasePath and RelPath MUST be formatted by the following way:
*               1. The path separator is the UNIX '/'.
*               2. The path MUST start by a '/'.
*               3. The path MUST not end by a '/', except for the root path "/".
*
*               BasePath and RelPath MAY be case sensitive, depending on the underlying filesystem.
*               BasePath and RelPath MAY use name lengths according to the underlying filesystem.
*               Paths separators WILL be converted to the underlying filesystem separator by FTPs.
*
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPs_AuthUser  (FTPs_SESSION_STRUCT  *ftp_session);


/*
*********************************************************************************************************
*                                       RTOS INTERFACE FUNCTIONS
*                                           (see ftp-s_os.c)
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPs_Srv_OS_TaskCreate  (void  *p_arg);            /* Server task creation.                                */

CPU_BOOLEAN  FTPs_Ctrl_OS_TaskCreate (void  *p_arg);            /* Control task creation.                               */

void         FTPs_OS_TaskSuspend     (void);                    /* Current task suspend.                                */

void         FTPs_OS_TaskDelete      (void);                    /* Current task terminate.                              */

void         FTPs_OS_TxDly           (void);


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

#ifndef  FTPs_TRACE_LEVEL
#define  FTPs_TRACE_LEVEL                       TRACE_LEVEL_OFF
#endif

#ifndef  FTPs_TRACE
#define  FTPs_TRACE                             printf
#endif

#define  FTPs_TRACE_INFO(x)                   ((FTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(FTPs_TRACE x) : (void)0)
#define  FTPs_TRACE_DBG(x)                    ((FTPs_TRACE_LEVEL >= TRACE_LEVEL_DBG)  ? (void)(FTPs_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                /* Server task name.                                    */
#ifndef  FTPs_SRV_OS_CFG_TASK_NAME
#error  "FTPs_SRV_OS_CFG_TASK_NAME              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Control task name.                                   */
#ifndef  FTPs_CTRL_OS_CFG_TASK_NAME
#error  "FTPs_CTRL_OS_CFG_TASK_NAME             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Server task priority.                                */
#ifndef  FTPs_SRV_OS_CFG_TASK_PRIO
#error  "FTPs_SRV_OS_CFG_TASK_PRIO              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Control task priority.                               */
#ifndef  FTPs_CTRL_OS_CFG_TASK_PRIO
#error  "FTPs_CTRL_OS_CFG_TASK_PRIO             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Server task stack size.                              */
#ifndef  FTPs_SRV_OS_CFG_TASK_STK_SIZE
#error  "FTPs_SRV_OS_CFG_TASK_STK_SIZE          illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Control task stack size.                             */
#ifndef  FTPs_CTRL_OS_CFG_TASK_STK_SIZE
#error  "FTPs_CTRL_OS_CFG_TASK_STK_SIZE         illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* FTP Control IP port.  Default is 21.                 */
#ifndef  FTPs_CFG_CTRL_IPPORT
#error  "FTPs_CFG_CTRL_IPPORT                   illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* FTP Data IP port.  Default is 20.                    */
#ifndef  FTPs_CFG_DTP_IPPORT
#error  "FTPs_CFG_DTP_IPPORT                    illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on ACCEPT.               */
#ifndef  FTPs_CFG_CTRL_MAX_ACCEPT_TIMEOUT_S
#error  "FTPs_CFG_CTRL_MAX_ACCEPT_TIMEOUT_S     illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on RX.                   */
#ifndef  FTPs_CFG_CTRL_MAX_RX_TIMEOUT_S
#error  "FTPs_CFG_CTRL_MAX_RX_TIMEOUT_S         illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on TX.                   */
#ifndef  FTPs_CFG_CTRL_MAX_TX_TIMEOUT_S
#error  "FTPs_CFG_CTRL_MAX_TX_TIMEOUT_S         illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on ACCEPT.               */
#ifndef  FTPs_CFG_DTP_MAX_ACCEPT_TIMEOUT_S
#error  "FTPs_CFG_DTP_MAX_ACCEPT_TIMEOUT_S      illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on CONNECT.              */
#ifndef  FTPs_CFG_DTP_MAX_CONN_TIMEOUT_S
#error  "FTPs_CFG_DTP_MAX_CONN_TIMEOUT_S        illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on RX.                   */
#ifndef  FTPs_CFG_DTP_MAX_RX_TIMEOUT_S
#error  "FTPs_CFG_DTP_MAX_RX_TIMEOUT_S          illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum inactivity time (s) on TX.                   */
#ifndef  FTPs_CFG_DTP_MAX_TX_TIMEOUT_S
#error  "FTPs_CFG_DTP_MAX_TX_TIMEOUT_S          illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum number of retries on ACCEPT.                 */
#ifndef  FTPs_CFG_CTRL_MAX_ACCEPT_RETRY
#error  "FTPs_CFG_CTRL_MAX_ACCEPT_RETRY         illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum number of retries on RX.                     */
#ifndef  FTPs_CFG_CTRL_MAX_RX_RETRY
#error  "FTPs_CFG_CTRL_MAX_RX_RETRY             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum number of retries on TX.                     */
#ifndef  FTPs_CFG_CTRL_MAX_TX_RETRY
#error  "FTPs_CFG_CTRL_MAX_TX_RETRY             illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum number of retries on ACCEPT.                 */
#ifndef  FTPs_CFG_DTP_MAX_ACCEPT_RETRY
#error  "FTPs_CFG_DTP_MAX_ACCEPT_RETRY          illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum number of retries on CONNECT.                */
#ifndef  FTPs_CFG_DTP_MAX_CONN_RETRY
#error  "FTPs_CFG_DTP_MAX_CONN_RETRY            illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum number of retries on RX.                     */
#ifndef  FTPs_CFG_DTP_MAX_RX_RETRY
#error  "FTPs_CFG_DTP_MAX_RX_RETRY              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum number of retries on TX.                     */
#ifndef  FTPs_CFG_DTP_MAX_TX_RETRY
#error  "FTPs_CFG_DTP_MAX_TX_RETRY              illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum length for user name.                        */
#ifndef  FTPs_CFG_MAX_USER_LEN
#error  "FTPs_CFG_MAX_USER_LEN                  illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif

                                                                /* Maximum length for password.                         */
#ifndef  FTPs_CFG_MAX_PASS_LEN
#error  "FTPs_CFG_MAX_PASS_LEN                  illegally #define'd in 'app_cfg.h'"
#error  "                                       see template file in package      "
#error  "                                       named 'ftp-s_cfg.h'               "
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif  /* __FTP_S_H__ */
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
