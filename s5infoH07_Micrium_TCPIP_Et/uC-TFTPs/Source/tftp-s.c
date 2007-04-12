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
* Filename      : tftp-s.c
* Version       : V1.85
* Programmer(s) : JJL
*                 JDH
*********************************************************************************************************
*
* This file supports a complete TFTP server that interfaces to any file system.
*
* It is a "single-user" server, meaning that while any one transaction is in progress, other transactions
* are held off by returning a TFTP error condition that indicates that the server is busy.
*
* There are three public functions:
*
*   1. void  TFTPs_En(void)         Used to re-enable a previously disabled server.
*
*   2. void  TFTPs_Dis(void)        Used to disable the running server temporarily.
*
*   3. void  TFTPs_Init(void)       Used to initializes the TFTP server.  The TFTP server task is
*                                   created here.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   TFTPs_MODULE
#include  <includes.h>
#include  <app_cfg.h>
#include  <lib_str.h>
#include  <fs_api.h>
#include  <net.h>
#include  <tftp-s.h>


/*
*********************************************************************************************************
*                                            TFTPs SPECIFIC
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TFTPs_TIMEOUT_SEC_DFLT                           20

#if     (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
#define  TFTPs_TRACE_STR_SIZE                             80
#endif

#define  TFTPs_FILE_OPEN_RD                                0
#define  TFTPs_FILE_OPEN_WR                                1


/*
*********************************************************************************************************
*                                           LOCAL DATA TYPES
*********************************************************************************************************
*/

#if (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
    typedef  struct {
        CPU_INT16U  Id;                                         /* Event ID #.                                          */
        CPU_INT32U  TS;                                         /* Time Stamp.                                          */
        CPU_INT08U  State;                                      /* Current TFTPs state.                                 */
        CPU_CHAR    Str[TFTPs_TRACE_STR_SIZE + 1];              /* ASCII string for comment.                            */
        CPU_INT16U  RxBlkNbr;                                   /* Current Rx Block Number.                             */
        CPU_INT16U  TxBlkNbr;                                   /* Current Tx Block Number.                             */
    } TFTPs_TRACE;
#endif


/*
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*/

CPU_INT16U         TFTPs_RxBlkNbr;                              /* Current block number received.                       */
CPU_INT08U         TFTPs_RxMsgBuf[TFTPs_BUF_SIZE];              /* Incoming packet buffer.                              */
CPU_INT32U         TFTPs_RxMsgCtr;                              /* Number of messages received.                         */
CPU_INT32S         TFTPs_RxMsgLen;

CPU_INT16U         TFTPs_TxBlkNbr;                              /* Current block number being sent.                     */
CPU_INT08U         TFTPs_TxMsgBuf[TFTPs_BUF_SIZE];              /* Outgoing packet buffer.                              */
CPU_INT16U         TFTPs_TxMsgCtr;
CPU_INT32S         TFTPs_TxMsgLen;

NET_SOCK_ADDR_IP   TFTPs_SockAddr;
CPU_INT32U         TFTPs_SockAddrLen;
CPU_INT32S         TFTPs_SockID;

CPU_INT08U         TFTPs_State;                                 /* Current state of TFTPs state machine.                */

CPU_INT16U         TFTPs_OpCode;

FS_FILE           *TFTPs_FileHandle;                            /* File handle of currently opened file.                */

CPU_BOOLEAN        TFTPs_ServerEn;

CPU_INT16U         TFTPs_Timeout_s;
CPU_INT32U         TFTPs_TimeoutCtr;
CPU_INT32U         TFTPs_WrSize;

#if (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
    CPU_CHAR       TFTPs_DispTbl[TFTPs_DISP_MAX_Y][TFTPs_DISP_MAX_X];
#endif


/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

#if (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
    static  TFTPs_TRACE  TFTPs_TraceTbl[TFTPs_TRACE_MAX];
    static  CPU_INT16U   TFTPs_TraceIx;
#endif


/*
*********************************************************************************************************
*                                           LOCAL PROTOTYPES
*********************************************************************************************************
*/

static  TFTP_ERR     TFTPs_ServerInit    (void);

static  TFTP_ERR     TFTPs_StateIdle     (void);
static  TFTP_ERR     TFTPs_StateDataRd   (void);
static  TFTP_ERR     TFTPs_StateDataWr   (void);

static  void         TFTPs_GetRxBlkNbr   (void);

static  void         TFTPs_Terminate     (void);

static  TFTP_ERR     TFTPs_FileOpen      (CPU_BOOLEAN   rw);
static  FS_FILE     *TFTPs_FileOpenMode  (CPU_CHAR     *filename,
                                          CPU_CHAR     *mode,
                                          CPU_BOOLEAN   rw);

static  TFTP_ERR     TFTPs_DataRd        (void);
static  TFTP_ERR     TFTPs_DataWr        (void);
static  void         TFTPs_DataWrAck     (CPU_INT32U    blk_nbr);

static  CPU_INT16S   TFTPs_Tx            (CPU_INT16U    opcode,
                                          CPU_INT16U    blk_nbr,
                                          CPU_INT08U   *p_buf,
                                          CPU_INT16U    len);
static  void         TFTPs_TxErr         (CPU_INT16U    err_code,
                                          CPU_CHAR     *err_msg);

#if (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
    static  void     TFTPs_TraceInit     (void);
    static  void     TFTPs_Trace         (CPU_INT16U    id,
                                          CPU_CHAR     *s);
#endif


/*
*********************************************************************************************************
*                                              TFTPs_En()
*
* Description: Enables the TFTP server.
*********************************************************************************************************
*/

void  TFTPs_En (void)
{
    TFTPs_ServerEn = DEF_ENABLED;
    TFTPs_State    = TFTPs_STATE_IDLE;
}


/*
*********************************************************************************************************
*                                              TFTPs_Dis()
*
* Description: Disables the TFTP server.
*********************************************************************************************************
*/

void  TFTPs_Dis (void)
{
    TFTPs_ServerEn = DEF_DISABLED;
    TFTPs_Terminate();
}


/*
*********************************************************************************************************
*                                             TFTPs_Init()
*
* Description: This function initializes the TFTP server.
*
* Arguments  : time_ms      is the timeout used when waiting for a response.
*
* Returns    : DEF_FAIL     TFTPs failed to initialized;
*              DEF_OS       TFTPs initialized correctly.
*********************************************************************************************************
*/

CPU_BOOLEAN  TFTPs_Init (void)
{
    CPU_BOOLEAN  err;


    err = TFTPs_OS_Init((void *)0);
    return (err);
}


/*
*********************************************************************************************************
*                                             TFTPs_Task()
*
* Description : This is the main TFTP server code.
*
* Argument(s) : p_arg       argument passed to the task (ignored).
*
* Return(s)   : None.
*
* Caller(s)   : TFTPs_OS_Task().
*********************************************************************************************************
*/

void  TFTPs_Task (void  *p_arg)
{
    CPU_INT16U  *p_opcode;
    NET_ERR      err;
    NET_ERR      err_sock;


    err = TFTPs_ServerInit();                                   /* Initialize the TFTP server.                          */

                                                                /* Set blocking timeout to infinite until we 'connect'. */
    NetOS_Sock_RxQ_TimeoutSet(TFTPs_SockID, NET_TMR_TIME_INFINITE, &err_sock);

                                                                /* Wait for incoming packets.                           */
    while (DEF_YES) {
        TFTPs_SockAddrLen = sizeof(TFTPs_SockAddr);

                                                                /* Wait for message from client (blocking).             */
        TFTPs_RxMsgLen    = NetSock_RxDataFrom( TFTPs_SockID,
                                               &TFTPs_RxMsgBuf[0],
                                                sizeof(TFTPs_RxMsgBuf),
                                                NET_SOCK_FLAG_NONE,
                                               (NET_SOCK_ADDR     *)&TFTPs_SockAddr,
                                               (NET_SOCK_ADDR_LEN *)&TFTPs_SockAddrLen,
                                                0,
                                                0,
                                                0,
                                               &err);

        if (TFTPs_RxMsgLen < 0) {                               /* A negative value indicates a timeout.                */
            TFTPs_Terminate();                                  /* Terminate the current file transfer process.         */
            continue;
        }

        TFTPs_RxMsgCtr++;                                       /* Keep track of the number of messages received.       */

        if (TFTPs_ServerEn != DEF_ENABLED) {
            TFTPs_TxErr(0, (CPU_CHAR *)"Transaction denied, Server DISABLED");
            continue;
        }

        p_opcode     = (CPU_INT16U *)&TFTPs_RxMsgBuf[TFTP_PKT_OFFSET_OPCODE];
        TFTPs_OpCode = NET_UTIL_NET_TO_HOST_16(*p_opcode);
        switch (TFTPs_State) {
                                                                /* Idle state, expecting a new 'connection'.            */
            case TFTPs_STATE_IDLE:
                 err = TFTPs_StateIdle();
                 break;

                                                                /* Processing a read request.                           */
            case TFTPs_STATE_DATA_RD:
                 err = TFTPs_StateDataRd();
                 break;

                                                                /* Processing a write request.                          */
            case TFTPs_STATE_DATA_WR:
                 err = TFTPs_StateDataWr();
                 break;
        }

                                                                /* Terminate file transfer in progress if we had an     */
                                                                /* error.                                               */
        if (err != TFTP_ERR_NONE) {
            TFTPs_TRACE(1, (CPU_CHAR *)"Task, Error, session terminated         ");
            TFTPs_Terminate();
        }
    }
}


/*
*********************************************************************************************************
*                                          TFTPs_ServerInit()
*
* Description: This function initializes the TFTP server.
*
* Arguments  : None
*
* Returns    : TFTP_ERR_NONE            if the call was succesful;
*              TFTP_ERR_NO_SOCK         if we could not open a socket;
*              TFTP_ERR_CANT_BIND       if we could not bind to the TFTPs port.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_ServerInit (void)
{
    NET_ERR  bind_status;
    NET_ERR  err;


#if (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
    TFTPs_TraceInit();
#endif

    TFTPs_ServerEn = DEF_ENABLED;

                                                                /* Open a socket to listen for incoming connections.    */
    TFTPs_SockID   = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                                   NET_SOCK_TYPE_DATAGRAM,
                                   NET_SOCK_PROTOCOL_UDP,
                                  &err);

    if (TFTPs_SockID < 0) {                                     /* Could not open a socket.                             */
        return (TFTP_ERR_NO_SOCK);
    }

    Mem_Set(&TFTPs_SockAddr, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);  /* Bind a local address so the client can send to us.   */
    TFTPs_SockAddr.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
    TFTPs_SockAddr.Port   = NET_UTIL_HOST_TO_NET_16(TFTPs_CFG_IPPORT);
    TFTPs_SockAddr.Addr   = NET_UTIL_HOST_TO_NET_32(INADDR_ANY);

    bind_status           = NetSock_Bind( TFTPs_SockID,
                                         (NET_SOCK_ADDR *)&TFTPs_SockAddr,
                                          NET_SOCK_ADDR_SIZE,
                                         &err);

    if (bind_status != NET_SOCK_BSD_ERR_NONE) {                 /* Could not bind to the TFTPs port.                    */
        NetSock_Close(TFTPs_SockID, &err);
        return (TFTP_ERR_CANT_BIND);
    }

    TFTPs_State      = TFTPs_STATE_IDLE;

    TFTPs_FileHandle = (FS_FILE *)0;

    TFTPs_RxBlkNbr   = 0;
    TFTPs_RxMsgCtr   = 0;

    TFTPs_TxBlkNbr   = 0;
    TFTPs_TxMsgCtr   = 0;

    return (TFTP_ERR_NONE);
}


/*
*********************************************************************************************************
*                                           TFTPs_StateIdle()
*
* Description: This function is called when the TFTP server is in the IDLE state.  Valid commands received
*              in this state are:
*                 RRQ   Read  request;
*                 WRQ   Write request.
*              DATA, ACK and ERR packets are rejected.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_StateIdle (void)
{
    TFTP_ERR  err;
    NET_ERR   err_sock;


    TFTPs_TRACE(10, (CPU_CHAR *)"Idle State                              ");
    switch (TFTPs_OpCode) {
        case TFTP_OPCODE_RD_REQ:
                                                                /* Open the desired file for reading.                   */
             err                = TFTPs_FileOpen(TFTPs_FILE_OPEN_RD);
             if (err == TFTP_ERR_NONE) {
                 TFTPs_TRACE(11, (CPU_CHAR *)"Rd Request, File Opened             ");
                 TFTPs_TxBlkNbr = 0;
                 TFTPs_State    = TFTPs_STATE_DATA_RD;
                 err            = TFTPs_DataRd();               /* Read the first block of data from the file and send  */
                                                                /* to client.                                           */
             }
             break;

        case TFTP_OPCODE_ACK:                                   /* NOT supposed to get ACKs in the Idle state.          */
             TFTPs_TRACE(12, (CPU_CHAR *)"ACK received, not supposed to!          ");
             err                = TFTP_ERR_ACK;
             break;

        case TFTP_OPCODE_WR_REQ:
             TFTPs_TxBlkNbr     = 0;
                                                                /* Open the desired file for writing.                   */
             err                = TFTPs_FileOpen(TFTPs_FILE_OPEN_WR);
             if (err == TFTP_ERR_NONE) {
                 TFTPs_TRACE(13, (CPU_CHAR *)"Wr Request, File Opened                 ");
                 TFTPs_State    = TFTPs_STATE_DATA_WR;
                 TFTPs_DataWrAck(TFTPs_TxBlkNbr);               /* Acknowledge the client.                              */
                 err            = TFTP_ERR_NONE;
             }
             break;

        case TFTP_OPCODE_DATA:                                  /* NOT supposed to get DATA packets in the Idle state.  */
             err                = TFTP_ERR_DATA;
             break;

        case TFTP_OPCODE_ERR:                                   /* NOT supposed to get ERR packets in the Idle state.   */
             err                = TFTP_ERR_ERR;
             break;
    }

    if (err == TFTP_ERR_NONE) {
        TFTPs_TRACE(14, (CPU_CHAR *)"No error, Timeout set                   ");
        NetOS_Sock_RxQ_TimeoutSet( TFTPs_SockID,
                                   TFTPs_CFG_MAX_RX_TIMEOUT_S,
                                  &err_sock);
    }

    return (err);
}


/*
*********************************************************************************************************
*                                          TFTPs_StateDataRd()
*
* Description: This function is called when the TFTP server is expecting data blocks to be read by the
*              client.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_StateDataRd (void)
{
    TFTP_ERR  err;


    err = TFTP_ERR_NONE;
    switch (TFTPs_OpCode) {
        case TFTP_OPCODE_RD_REQ:                                /* NOT supposed to get RRQ packets in the DATA.         */
                                                                /* Read state.                                          */
             FS_FClose(TFTPs_FileHandle);                       /* Close the file and then, re-open it!                 */
                                                                /* Open the desired file for reading.                   */
             err                = TFTPs_FileOpen(TFTPs_FILE_OPEN_RD);
             if (err == TFTP_ERR_NONE) {
                 TFTPs_TRACE(20, (CPU_CHAR *)"Data Rd, Rx RD_REQ.                     ");
                 TFTPs_TxBlkNbr = 0;
                 TFTPs_State    = TFTPs_STATE_DATA_RD;
                 err            = TFTPs_DataRd();               /* Read the first block of data from the file and send  */
                                                                /* to client.                                           */
            }
            break;

        case TFTP_OPCODE_ACK:
             TFTPs_GetRxBlkNbr();
             if (TFTPs_RxBlkNbr == TFTPs_TxBlkNbr) {            /* Confirm that we got the acknowledge for the data     */
                                                                /* sent.                                                */
                 TFTPs_TRACE(21, (CPU_CHAR *)"Data Rd, ACK Rx'd                       ");
                 err            = TFTPs_DataRd();               /* Read the first block of data from the file and send  */
                                                                /* to client.                                           */
             } else {
                 TFTPs_TRACE(22, (CPU_CHAR *)"Data Rd, Rx doesn't match Tx block#     ");
                 err            = TFTP_ERR_ACK;
             }
             break;

        case TFTP_OPCODE_WR_REQ:                                /* NOT supposed to get WRQ packets in the DATA Read     */
                                                                /* state.                                               */
             TFTPs_TRACE(23, (CPU_CHAR *)"Data Rd, Rx'd WR_REQ                    ");
             TFTPs_TxErr(0,  (CPU_CHAR *)"RRQ server busy, WRQ  opcode?");
             err                = TFTP_ERR_WR_REQ;
             break;

        case TFTP_OPCODE_DATA:                                  /* NOT supposed to get DATA packets in the DATA Read    */
                                                                /* state.                                               */
             TFTPs_TRACE(24, (CPU_CHAR *)"Data Rd, Rx'd DATA                      ");
             TFTPs_TxErr(0,  (CPU_CHAR *)"RRQ server busy, DATA opcode?");
             err                = TFTP_ERR_DATA;
             break;

        case TFTP_OPCODE_ERR:
             TFTPs_TRACE(25, (CPU_CHAR *)"Data Rd, Rx'd ERR                       ");
             TFTPs_TxErr(0,  (CPU_CHAR *)"RRQ server busy, ERR  opcode?");
             err                = TFTP_ERR_ERR;
             break;
    }

    return (err);
}


/*
*********************************************************************************************************
*                                          TFTPs_StateDataWr()
*
* Description: This function is called when the client is sending data to be written in a server's file.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_StateDataWr (void)
{
    TFTP_ERR  err;


    err = TFTP_ERR_NONE;
    switch (TFTPs_OpCode) {
        case TFTP_OPCODE_RD_REQ:
             TFTPs_TRACE(30, (CPU_CHAR *)"Data Wr, WRQ server busy, RRQ  opcode?  ");
             TFTPs_TxErr(0,  (CPU_CHAR *)"WRQ server busy, RRQ  opcode?");
             err             = TFTP_ERR_RD_REQ;
             break;

        case TFTP_OPCODE_ACK:
             TFTPs_TRACE(31, (CPU_CHAR *)"Data Wr, WRQ server busy, ACK  opcode?  ");
             TFTPs_TxErr(0,  (CPU_CHAR *)"WRQ server busy, ACK  opcode?");
             err             = TFTP_ERR_ACK;
             break;

        case TFTP_OPCODE_WR_REQ:
             FS_FClose(TFTPs_FileHandle);
             TFTPs_TxBlkNbr  = 0;
                                                                /* Open the desired file for writing.                   */
             err             = TFTPs_FileOpen(TFTPs_FILE_OPEN_WR);
             if (err == TFTP_ERR_NONE) {
                 TFTPs_TRACE(32, (CPU_CHAR *)"Data Wr, Rx'd WR_REQ again              ");
                 TFTPs_State = TFTPs_STATE_DATA_WR;
                 TFTPs_DataWrAck(TFTPs_TxBlkNbr);               /* Acknowledge the client.                              */
                 err         = TFTP_ERR_NONE;
             }
             break;

        case TFTP_OPCODE_DATA:
             TFTPs_TRACE(33, (CPU_CHAR *)"Data Wr, Rx'd DATA --- OK               ");
             err             = TFTPs_DataWr();                  /* Write data to file.                                  */
             break;

        case TFTP_OPCODE_ERR:
             TFTPs_TRACE(34, (CPU_CHAR *)"Data Wr, WRQ server busy, ERR  opcode?  ");
             TFTPs_TxErr(0,  (CPU_CHAR *)"WRQ server busy, ERR  opcode?");
             err             = TFTP_ERR_ERR;
             break;
    }

    return (err);
}


/*
*********************************************************************************************************
*                                          TFTPs_GetRxBlkNbr()
*
* Description: This function extracts the block number from the received TFTP command packet.
*********************************************************************************************************
*/

static  void  TFTPs_GetRxBlkNbr (void)
{
    CPU_INT16U  *p_blk_nbr;


    p_blk_nbr      = (CPU_INT16U *)&TFTPs_RxMsgBuf[TFTP_PKT_OFFSET_BLK_NBR];
    TFTPs_RxBlkNbr = NET_UTIL_NET_TO_HOST_16(*p_blk_nbr);
}


/*
*********************************************************************************************************
*                                           TFTPs_Terminate()
*
* Description: This function terminates the current file transfer process.
*********************************************************************************************************
*/

static  void  TFTPs_Terminate (void)
{
    TFTP_ERR  err_sock;


    TFTPs_State = TFTPs_STATE_IDLE;                             /* Abort current file transfer.                         */
    if (TFTPs_FileHandle != (FS_FILE *)0) {
        FS_FClose(TFTPs_FileHandle);                            /* Close the current opened file.                       */
        TFTPs_FileHandle  = (FS_FILE *)0;
    }

                                                                /* Reset blocking timeout to infinite.                  */
    NetOS_Sock_RxQ_TimeoutSet( TFTPs_SockID,
                               NET_TMR_TIME_INFINITE,
                              &err_sock);
}


/*
*********************************************************************************************************
*                                           TFTPs_FileOpen()
*
* Description: Reads the filename and file mode from the TFTP packet and attemps to open the file in the
*              desired file.
*
* Note(s)    : 1) This function also extracts options as specified in RFC 2349:
*
*                 "timeout"     specifies the timeout in seconds to wait in case we don't receive data
*                               request after we initiated a read request.
*
*                 "tsize"       specifies the size of the file in bytes that the client is writing.
*
*                 Note that both these options may not be supported by the client and thus, we assume
*                 default values if they are not specified.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_FileOpen (CPU_BOOLEAN  rw)
{
    CPU_CHAR  *p_filename;
    CPU_CHAR  *p_mode;
    CPU_CHAR  *p_name;
    CPU_CHAR  *p_value;


                                                                /* ---- GET FILENAME ---------------------------------- */
    p_filename = &TFTPs_RxMsgBuf[TFTP_PKT_OFFSET_FILENAME];
                                                                /* ---- GET FILE MODE --------------------------------- */
    p_mode     = p_filename;                                    /* Point to the 'Mode' string.                          */
    while (*p_mode > (CPU_CHAR)0) {
        p_mode++;
    }
    p_mode++;
                                                                /* ---- GET RFC2349 "timeout" OPTION (IF AVAILABLE) --- */
    p_name = p_mode;                                            /* Skip over the 'Mode' string.                         */
    while (*p_name > (CPU_CHAR)0) {
        p_name++;
    }
    p_name++;

                                                                /* See if the client specified a "timeout" string       */
    if (Str_Cmp(p_name, (CPU_CHAR *)"timeout") == 0) {                      /* (RFC2349).                                           */

        p_value = p_name;                                       /* Yes, skip over the "timeout" string.                 */
        while (*p_value > (CPU_CHAR)0) {
            p_value++;
        }
        p_value++;
                                                                /* Get the timeout (in seconds).                        */
        TFTPs_Timeout_s = Str_ToLong(p_value, (CPU_CHAR  **)0, 0);
    } else {
        TFTPs_Timeout_s = TFTPs_TIMEOUT_SEC_DFLT;               /* No,  assume the default timeout (in seconds).        */

                                                                /* ---- GET RFC2349 "tsize" OPTION (IF AVAILABLE) ----- */
                                                                /* See if the client specified a "tsize" string         */
                                                                /* (RFC2349).                                           */
        if (Str_Cmp(p_name, (CPU_CHAR *)"tsize") == 0) {
            p_value = p_name;
            while (*p_value > (CPU_CHAR)0) {                    /* Yes, skip over the "tsize" string.                   */
                p_value++;
            }
            p_value++;
                                                                /* Get the size of the file to write.                   */
            TFTPs_WrSize = Str_ToLong(p_value, (CPU_CHAR  **)0, 0);
        } else {
            TFTPs_WrSize = 0;                                   /* Assume a default value of 0.                         */
        }
    }

                                                                /* ---- OPEN THE FILE --------------------------------- */
    TFTPs_FileHandle = TFTPs_FileOpenMode(p_filename, p_mode, rw);

    if (TFTPs_FileHandle == (FS_FILE *)0) {
        TFTPs_TxErr(0, (CPU_CHAR *)"file not found");
        return (TFTP_ERR_FILE_NOT_FOUND);
    }

    return (TFTP_ERR_NONE);
}


/*
*********************************************************************************************************
*                                         TFTPs_FileOpenMode()
*
* Description: Opens the file for Read or Write.
*********************************************************************************************************
*/

static  FS_FILE  *TFTPs_FileOpenMode (CPU_CHAR     *filename,
                                      CPU_CHAR     *mode,
                                      CPU_BOOLEAN   rw)
{
    FS_FILE     *handle;
    CPU_INT16S   result;


    switch (rw) {
        case TFTPs_FILE_OPEN_RD:
             result = Str_Cmp(mode, (CPU_CHAR *)"octet");
             if (result == 0) {
                 handle = FS_FOpen((char *)filename, (const char *)"rb");
                 return (handle);
             }
             result = Str_Cmp(mode, (CPU_CHAR *)"netascii");
             if (result == 0) {
                 handle = FS_FOpen((char *)filename, (const char *)"r");
                 return (handle);
             }
             return ((FS_FILE *)0);

        case TFTPs_FILE_OPEN_WR:
             result = Str_Cmp(mode, (CPU_CHAR *)"octet");
             if (result == 0) {
                 handle = FS_FOpen((char *)filename, (const char *)"wb");
                 return (handle);
             }
             result = Str_Cmp(mode, (CPU_CHAR *)"netascii");
             if (result == 0) {
                 handle = FS_FOpen((char *)filename, (const char *)"w");
                 return (handle);
             }
             return ((FS_FILE *)0);

        default:
             return ((FS_FILE *)0);
    }
}


/*
*********************************************************************************************************
*                                            TFTPs_DataRd()
*
* Description: This function reads data from the file system and sends it to the client.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_DataRd (void)
{
    CPU_INT16S  tx_size;


                                                                /* Read data from the file.                             */
    TFTPs_TxMsgLen = FS_FRead(&TFTPs_TxMsgBuf[TFTP_PKT_OFFSET_DATA],
                               1,
                               TFTPs_BLOCK_SIZE,
                               TFTPs_FileHandle);

    if (TFTPs_TxMsgLen < TFTPs_BLOCK_SIZE) {                    /* Close the file when we are done.                     */
        FS_FClose(TFTPs_FileHandle);
        TFTPs_State = TFTPs_STATE_IDLE;
    }

    if (TFTPs_TxMsgLen < 0) {                                   /* See if we had an error reading the file.             */
        TFTPs_TxErr(0, (CPU_CHAR *)"RRQ file read error");      /* Yes, send error packet.                              */
        return (TFTP_ERR_FILE_RD);
    }

    TFTPs_TxMsgCtr++;
    TFTPs_TxBlkNbr++;

    TFTPs_TxMsgLen += TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR;

    tx_size         = TFTPs_Tx( TFTP_OPCODE_DATA,
                                TFTPs_TxBlkNbr,
                               &TFTPs_TxMsgBuf[0],
                                TFTPs_TxMsgLen);

    if (tx_size < 0) {
        TFTPs_TxErr(0, (CPU_CHAR *)"RRQ file read error");      /* Yes, send error packet.                              */
        return (TFTP_ERR_TX);
    }

    return (TFTP_ERR_NONE);
}


/*
*********************************************************************************************************
*                                            TFTPs_DataWr()
*
* Description: This function writes data to the file system.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_DataWr (void)
{
    CPU_INT16U   blk_nbr;
    CPU_INT16S   data_bytes;
    CPU_INT16U  *p_blk_nbr;


    data_bytes = TFTPs_RxMsgLen - TFTP_PKT_SIZE_OPCODE - TFTP_PKT_SIZE_BLK_NBR;

    if (data_bytes > 0) {                                       /* Write data to the file.                              */
        FS_FWrite(&TFTPs_RxMsgBuf[TFTP_PKT_OFFSET_DATA],
                   1,
                   data_bytes,
                   TFTPs_FileHandle);
    }
    if (data_bytes < TFTPs_BLOCK_SIZE) {                        /* See if we are done writing the file.                 */
        FS_FClose(TFTPs_FileHandle);                            /* Close the file.                                      */
        TFTPs_State = TFTPs_STATE_IDLE;
    }

    p_blk_nbr      = (CPU_INT16U *)&TFTPs_RxMsgBuf[TFTP_PKT_OFFSET_BLK_NBR];
    blk_nbr        = NET_UTIL_NET_TO_HOST_16(*p_blk_nbr);
    TFTPs_TxBlkNbr = blk_nbr;
    TFTPs_DataWrAck(blk_nbr);

    return (TFTP_ERR_NONE);
}


/*
*********************************************************************************************************
*                                           TFTPs_DataWrAck()
*
* Description: This function sends an acknowledge to the client.
*********************************************************************************************************
*/

static  void  TFTPs_DataWrAck (CPU_INT32U  blk_nbr)
{
    CPU_INT16S  tx_len;


    tx_len = TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR;
    TFTPs_TxMsgCtr++;

    TFTPs_Tx( TFTP_OPCODE_ACK, blk_nbr,
             &TFTPs_TxMsgBuf[0],
              tx_len);
}


/*
*********************************************************************************************************
*                                              TFTPs_Tx()
*
* Description: This function is called to send TFTP reply packets.
*********************************************************************************************************
*/

static  CPU_INT16S  TFTPs_Tx (CPU_INT16U   opcode,
                              CPU_INT16U   blk_nbr,
                              CPU_INT08U  *p_buf,
                              CPU_INT16U   tx_len)
{
    CPU_INT16U  *pbuf16;
    CPU_INT16S   bytes_sent;
    NET_ERR      err;


     pbuf16    = (CPU_INT16U *)&TFTPs_TxMsgBuf[TFTP_PKT_OFFSET_OPCODE];
    *pbuf16    = NET_UTIL_NET_TO_HOST_16(opcode);

     pbuf16    = (CPU_INT16U *)&TFTPs_TxMsgBuf[TFTP_PKT_OFFSET_BLK_NBR];
    *pbuf16    = NET_UTIL_NET_TO_HOST_16(blk_nbr);

    bytes_sent = NetSock_TxDataTo( TFTPs_SockID,
                                   p_buf,
                                   tx_len,
                                   NET_SOCK_FLAG_NONE,
                                  (NET_SOCK_ADDR *)&TFTPs_SockAddr,
                                   NET_SOCK_ADDR_SIZE,
                                  &err);

    return (bytes_sent);
}


/*
*********************************************************************************************************
*                                             TFTPs_TxErr()
*
* Description: Used as a convenient mechanism to send an error message to the client, when the server
*              detects something wrong.
*********************************************************************************************************
*/

static  void  TFTPs_TxErr (CPU_INT16U   err_code,
                           CPU_CHAR    *err_msg)
{
    CPU_INT16S  tx_len;


    tx_len = Str_Len(err_msg) + TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR + 1;

    Str_Copy(&TFTPs_TxMsgBuf[TFTP_PKT_OFFSET_DATA], err_msg);

    TFTPs_Tx( TFTP_OPCODE_ERR,
              err_code,
             &TFTPs_TxMsgBuf[0],
              tx_len);
}


/*
*********************************************************************************************************
*                                           TFTPs_TraceInit()
*
* Description: This function is called to initialize the trace debug feature of this module.
*********************************************************************************************************
*/

#if (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
static  void  TFTPs_TraceInit (void)
{
    CPU_INT16U  i;


    TFTPs_TraceIx = 0;
    for (i = 0; i < TFTPs_TRACE_MAX; i++) {
        TFTPs_TraceTbl[i].Id = 0;
        TFTPs_TraceTbl[i].TS = 0;
    }
}
#endif


/*
*********************************************************************************************************
*                                             TFTPs_Trace()
*
* Description: This function is used to record traces of execution of this module.
*********************************************************************************************************
*/

#if (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
static  void  TFTPs_Trace (CPU_INT16U   id,
                           CPU_CHAR    *s)
{
    TFTPs_TraceTbl[TFTPs_TraceIx].Id       = id;
    TFTPs_TraceTbl[TFTPs_TraceIx].TS       = OSTimeGet();
    TFTPs_TraceTbl[TFTPs_TraceIx].State    = TFTPs_State;

    Str_Copy(TFTPs_TraceTbl[TFTPs_TraceIx].Str, s);

    TFTPs_TraceTbl[TFTPs_TraceIx].RxBlkNbr = TFTPs_RxBlkNbr;
    TFTPs_TraceTbl[TFTPs_TraceIx].TxBlkNbr = TFTPs_TxBlkNbr;

    TFTPs_TraceIx++;
    if (TFTPs_TraceIx >= TFTPs_TRACE_MAX) {
        TFTPs_TraceIx  = 0;
    }
}
#endif


/*
*********************************************************************************************************
*                                             TFTPs_Disp()
*
* Description: This function populates a two dimensional array of characters with information about
*              the TFTPs module for purpose of displaying on an ASCII display.
*********************************************************************************************************
*/

#if (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
void  TFTPs_Disp (void)
{
    CPU_CHAR  s[TFTPs_DISP_MAX_X];
    NET_ERR   err;


                                             /*           1111111111222222222233333333334444444444555555555566666666667777777777 */
                                             /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    Str_Copy(&TFTPs_DispTbl[ 0][0], (CPU_CHAR *)"------------------------------------ TFTPs ------------------------------------");
    Str_Copy(&TFTPs_DispTbl[ 1][0], (CPU_CHAR *)"State : xxxxxxxxxx                                                             ");
    Str_Copy(&TFTPs_DispTbl[ 2][0], (CPU_CHAR *)"OpCode:                                                                        ");
    Str_Copy(&TFTPs_DispTbl[ 3][0], (CPU_CHAR *)"                                                                               ");
    Str_Copy(&TFTPs_DispTbl[ 4][0], (CPU_CHAR *)"Rx Msg Ctr: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[ 5][0], (CPU_CHAR *)"Rx Block #: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[ 6][0], (CPU_CHAR *)"Rx Msg Len: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[ 7][0], (CPU_CHAR *)"Rx Msg    : xx xx xx xx xx xx xx xx xx xx xx                                   ");
    Str_Copy(&TFTPs_DispTbl[ 8][0], (CPU_CHAR *)"                                                                               ");
    Str_Copy(&TFTPs_DispTbl[ 9][0], (CPU_CHAR *)"Tx Msg Ctr: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[10][0], (CPU_CHAR *)"Tx Block #: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[11][0], (CPU_CHAR *)"Tx Msg Len: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[12][0], (CPU_CHAR *)"Tx Msg    : xx xx xx xx xx xx xx xx xx xx xx                                   ");
    Str_Copy(&TFTPs_DispTbl[13][0], (CPU_CHAR *)"                                                                               ");
    Str_Copy(&TFTPs_DispTbl[14][0], (CPU_CHAR *)"                                                                               ");
    Str_Copy(&TFTPs_DispTbl[15][0], (CPU_CHAR *)"Source IP :                                                                    ");
    Str_Copy(&TFTPs_DispTbl[16][0], (CPU_CHAR *)"Dest   IP :                                                                    ");
                                                /*           1111111111222222222233333333334444444444555555555566666666667777777777 */
                                                /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */

    switch (TFTPs_State) {                                      /* Display state of TFTPs state machine.                */
        case TFTPs_STATE_IDLE:
             Str_Copy(&TFTPs_DispTbl[1][8], (CPU_CHAR *)"IDLE      ");
             break;

        case TFTPs_STATE_DATA_RD:
             Str_Copy(&TFTPs_DispTbl[1][8], (CPU_CHAR *)"DATA READ ");
             break;

        case TFTPs_STATE_DATA_WR:
             Str_Copy(&TFTPs_DispTbl[1][8], (CPU_CHAR *)"DATA WRITE");
             break;
    };

    switch (TFTPs_OpCode) {                                     /* Display Op-Code received.                            */
        case 0:
             Str_Copy(&TFTPs_DispTbl[2][8], (CPU_CHAR *)"-NONE-");
             break;

        case TFTP_OPCODE_RD_REQ:
             Str_Copy(&TFTPs_DispTbl[2][8], (CPU_CHAR *)"RD REQ");
             break;

        case TFTP_OPCODE_WR_REQ:
             Str_Copy(&TFTPs_DispTbl[2][8], (CPU_CHAR *)"WR REQ");
             break;

        case TFTP_OPCODE_DATA:
             Str_Copy(&TFTPs_DispTbl[2][8], (CPU_CHAR *)"DATA  ");
             break;

        case TFTP_OPCODE_ACK:
             Str_Copy(&TFTPs_DispTbl[2][8], (CPU_CHAR *)"ACK   ");
             break;

        case TFTP_OPCODE_ERR:
             Str_Copy(&TFTPs_DispTbl[2][8], (CPU_CHAR *)"ERR   ");
             break;
    };

                                                                /* Display number of messages received.                 */
    Str_Format_Print(s, TFTPs_DISP_MAX_X, "%5u", TFTPs_RxMsgCtr);
    Str_Copy(&TFTPs_DispTbl[4][12], s);

                                                                /* Display current block number.                        */
    Str_Format_Print(s, TFTPs_DISP_MAX_X, "%5u", TFTPs_RxBlkNbr);
    Str_Copy(&TFTPs_DispTbl[5][12], s);

                                                                /* Display received message length.                     */
    Str_Format_Print(s, TFTPs_DISP_MAX_X, "%5u", TFTPs_RxMsgLen);
    Str_Copy(&TFTPs_DispTbl[6][12], s);

    Str_Format_Print(s, TFTPs_DISP_MAX_X, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                     TFTPs_RxMsgBuf[0],
                     TFTPs_RxMsgBuf[1],
                     TFTPs_RxMsgBuf[2],
                     TFTPs_RxMsgBuf[3],
                     TFTPs_RxMsgBuf[4],
                     TFTPs_RxMsgBuf[5],
                     TFTPs_RxMsgBuf[6],
                     TFTPs_RxMsgBuf[7],
                     TFTPs_RxMsgBuf[8],
                     TFTPs_RxMsgBuf[9]);
    Str_Copy(&TFTPs_DispTbl[7][12], s);

                                                                /* Display number of messages sent.                     */
    Str_Format_Print(s, TFTPs_DISP_MAX_X, "%5u", TFTPs_TxMsgCtr);
    Str_Copy(&TFTPs_DispTbl[ 9][12], s);

                                                                /* Display current block number.                        */
    Str_Format_Print(s, TFTPs_DISP_MAX_X, "%5u", TFTPs_TxBlkNbr);
    Str_Copy(&TFTPs_DispTbl[10][12], s);

                                                                /* Display sent message length.                         */
    Str_Format_Print(s, TFTPs_DISP_MAX_X, "%5u", TFTPs_TxMsgLen);
    Str_Copy(&TFTPs_DispTbl[11][12], s);

    Str_Format_Print(s, TFTPs_DISP_MAX_X, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                     TFTPs_TxMsgBuf[0],
                     TFTPs_TxMsgBuf[1],
                     TFTPs_TxMsgBuf[2],
                     TFTPs_TxMsgBuf[3],
                     TFTPs_TxMsgBuf[4],
                     TFTPs_TxMsgBuf[5],
                     TFTPs_TxMsgBuf[6],
                     TFTPs_TxMsgBuf[7],
                     TFTPs_TxMsgBuf[8],
                     TFTPs_TxMsgBuf[9]);
    Str_Copy(&TFTPs_DispTbl[12][12], s);

    NetASCII_IP_to_Str(TFTPs_SockAddr.Addr, s, DEF_NO, &err);
    Str_Copy(&TFTPs_DispTbl[15][12], s);
}
#endif


/*
*********************************************************************************************************
*                                           TFTPs_DispTrace()
*
* Description: This function populates a two dimensional array of characters with information about the
*              TFTPs TRACE for purpose of displaying on an ASCII display.
*********************************************************************************************************
*/

#if (TFTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO)
void  TFTPs_DispTrace (void)
{
    CPU_INT16U  i;
    CPU_CHAR   s[TFTPs_DISP_MAX_X];
    CPU_CHAR  *ps;


                                              /*           1111111111222222222233333333334444444444555555555566666666667777777777 */
                                              /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    Str_Copy(&TFTPs_DispTbl[ 0][0], (CPU_CHAR *)"--------------------------------- TFTPs TRACE ----------------------------------");
    Str_Copy(&TFTPs_DispTbl[ 1][0], (CPU_CHAR *)" TS     ID     State  Rx#   Tx#                                                 ");
    Str_Copy(&TFTPs_DispTbl[ 2][0], (CPU_CHAR *)"                      xxxxx xxxxx  x                                            ");
    Str_Copy(&TFTPs_DispTbl[ 3][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 4][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 5][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 6][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 7][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 8][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 9][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[10][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[11][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[12][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[13][0], (CPU_CHAR *)"                                                                                ");
    Str_Copy(&TFTPs_DispTbl[14][0], (CPU_CHAR *)"                                                                                ");
                                              /*           1111111111222222222233333333334444444444555555555566666666667777777777 */
                                              /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */

    for (i = 0; i < TFTPs_TRACE_MAX; i++) {
        ps = &TFTPs_DispTbl[i + 2][0];

        if (i == TFTPs_TraceIx) {                               /* Indicate current position of trace.                  */
            Str_Copy(ps, ">");
        } else {
            Str_Copy(ps, " ");
        }

        Str_Format_Print(s, TFTPs_DISP_MAX_X, "%5u  %5u",
                         TFTPs_TraceTbl[i].TS & 0xFFFF,
                         TFTPs_TraceTbl[i].Id);
        Str_Cat(ps, s);

        switch (TFTPs_TraceTbl[i].State) {
            case TFTPs_STATE_IDLE:                              /* Idle state, expecting a new 'connection'.            */
                 Str_Cat(ps, (CPU_CHAR *)"  IDLE ");
                 break;

            case TFTPs_STATE_DATA_RD:                           /* Processing a read request.                           */
                 Str_Cat(ps, (CPU_CHAR *)"  RD   ");
                 break;

            case TFTPs_STATE_DATA_WR:                           /* Processing a write request.                          */
                 Str_Cat(ps, (CPU_CHAR *)"  WR   ");
                 break;

            default:
                 Str_Cat(ps, (CPU_CHAR *)"  ERROR");
                 break;
        }

        Str_Format_Print(s, TFTPs_DISP_MAX_X, "  %5u  %5u  ",
                         TFTPs_TraceTbl[i].RxBlkNbr,
                         TFTPs_TraceTbl[i].TxBlkNbr);
        Str_Cat(ps, s);

        Str_Cat(ps, TFTPs_TraceTbl[i].Str);
    }
}
#endif
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
