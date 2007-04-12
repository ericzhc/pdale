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
*                                           TFTP SERVER
*
* Filename      : tftps.c
* Programmer(s) : JJL
* Version       : V1.10
*********************************************************************************************************
*
* This file supports a complete TFTP server that interfaces to any file system.
*
* It is a "single-user" server, meaning that while any one transaction is in progress, other transactions 
* are held off by returning a TFTP error condition that indicates that the server is busy.
*
* There are three public functions:
*
*   1. void TFTPs_En(void)           Used to re-enable a previously disabled server.
*       
*   2. void TFTPs_Dis(void)          Used to disable the running server temporarily.
*       
*   3. void TFTPs_Task(void)         This function needs to be called by your RTOS.  This function will 
*                                    NOT return because it loops forever waiting for packets from a TFTP 
*                                    client.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define   TFTPs_MODULE

#define   TFTPs_GLOBALS
#include  <includes.h>


/*$PAGE*/
/*
*********************************************************************************************************
*                                           TFTPs  SPECIFIC
*                                              CONSTANTS
*********************************************************************************************************
*/

#define  TFTPs_TIMEOUT_SEC_DFLT      20

#if      TFTPs_TRACE_EN > 0
#define  TFTPs_TRACE_STR_SIZE        81
#endif

#define  TFTPs_FILE_OPEN_RD           0
#define  TFTPs_FILE_OPEN_WR           1

/*
*********************************************************************************************************
*                                           TFTPs  SPECIFIC
*                                              MACROS
*********************************************************************************************************
*/

#if  TFTPs_TRACE_EN > 0
#define  TFTPs_TRACE(a, b)    TFTPs_Trace(a, b)
#else
#define  TFTPs_TRACE(a, b)
#endif

/*
*********************************************************************************************************
*                                         LOCAL DATA TYPES
*********************************************************************************************************
*/

#if  TFTPs_TRACE_EN > 0
typedef  struct  {
    CPU_INT16U  Id;                                   /* Event ID #                                    */
    CPU_INT32U  TS;                                   /* Time Stamp                                    */
    CPU_INT08U  State;                                /* Current TFTPs state                           */
    CPU_CHAR    Str[TFTPs_TRACE_STR_SIZE];            /* ASCII string for comment                      */
    CPU_INT16U  RxBlkNbr;                             /* Current Rx Block Number                       */
    CPU_INT16U  TxBlkNbr;                             /* Current Tx Block Number                       */
} TFTPs_TRACE;
#endif

/*
*********************************************************************************************************
*                                         LOCAL VARIABLES
*********************************************************************************************************
*/

#if  TFTPs_TRACE_EN > 0
static  TFTPs_TRACE  TFTPs_TraceTbl[TFTPs_TRACE_MAX];
static  CPU_INT16U   TFTPs_TraceIx;
#endif

/*
*********************************************************************************************************
*                                         LOCAL PROTOTYPES
*********************************************************************************************************
*/

static  TFTP_ERR      TFTPs_ServerInit(void);

static  TFTP_ERR      TFTPs_StateIdle(void);
static  TFTP_ERR      TFTPs_StateDataRd(void);
static  TFTP_ERR      TFTPs_StateDataWr(void);

static  void          TFTPs_GetRxBlkNbr(void);

static  void          TFTPs_Terminate(void);

static  TFTP_ERR      TFTPs_FileOpen(CPU_BOOLEAN rw);
static  FS_FILE      *TFTPs_FileOpenMode(CPU_CHAR *filename, CPU_CHAR *mode, CPU_BOOLEAN rw);
static  TFTP_ERR      TFTPs_DataRd(void);
static  TFTP_ERR      TFTPs_DataWr(void);
static  void          TFTPs_DataWrAck(INT32U blk_nbr);

static  CPU_INT16S    TFTPs_Tx(CPU_INT16U opcode, CPU_INT16U blk_nbr, CPU_INT08U *p_buf, CPU_INT16U len);
static  void          TFTPs_TxErr(CPU_INT16U err_code, CPU_CHAR *err_msg);

#if  TFTPs_TRACE_EN > 0
static  void          TFTPs_TraceInit(void);
static  void          TFTPs_Trace(CPU_INT16U id, CPU_CHAR *s);
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                            TFTPs_En()
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
*                                            TFTPs_Dis()
*
* Description: Disables the TFTP server.
*********************************************************************************************************
*/

void  TFTPs_Dis (void)
{
    TFTPs_ServerEn = DEF_DISABLED;
    TFTPs_Terminate();
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                            TFTPs_Init()
*
* Description: This function initializes the TFTP server.
*
* Arguments  : time                 is the timeout used when waiting for a response.
*
* Returns    : DEF_FAIL             TFTPs failed to initialized
*              DEF_OS               TFTPs initialized correctly
*********************************************************************************************************
*/

CPU_BOOLEAN  TFTPs_Init (NET_TMR_TICK  time)
{
    CPU_BOOLEAN  err;


    TFTPs_BlockingTimeout = time;                                               /* Save the blocking timeout value                        */
    err                   = TFTPs_OS_Init();
    return (err);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           TFTPs_Task()
*
* Description: This is the main TFTP server code.
*
* Arguments  : time                 is the timeout used when waiting for a response.
*
* Returns    : TFTP_ERR_NONE        if the call was succesful
*              TFTP_ERR_NO_SOCK     if we could not open a socket
*              TFTP_ERR_CANT_BIND   if we could not bind to the TFTPs port
*********************************************************************************************************
*/

TFTP_ERR  TFTPs_Task (void)
{
    CPU_INT16U   *p_opcode;
    NET_ERR       err;
    NET_ERR       err_sock;


    err = TFTPs_ServerInit();                                                 /* Initialize the TFTP server                               */
    if (err != TFTP_ERR_NONE) {
        return (err);
    }

    NetOS_Sock_RxQ_TimeoutSet(TFTPs_SockID, NET_TMR_TIME_INFINITE, &err_sock);/* Set blocking timeout to infinite until we 'connect'      */

    while (TRUE) {                                                            /* Wait for incoming packets.                               */
        TFTPs_SockAddrLen = sizeof(TFTPs_SockAddr);
        TFTPs_RxMsgLen    = NetSock_RxDataFrom(TFTPs_SockID,                  /* Wait for message from client (blocking)                  */
                                               (void *)&TFTPs_RxMsgBuf[0],
                                               (CPU_INT16S)sizeof(TFTPs_RxMsgBuf),
                                               0,
                                               (NET_SOCK_ADDR     *)&TFTPs_SockAddr,
                                               (NET_SOCK_ADDR_LEN *)&TFTPs_SockAddrLen,
                                               0,
                                               0,
                                               0,
                                               &err);

        if (TFTPs_RxMsgLen < 0) {                                             /* A negative value indicates a timeout                     */
            TFTPs_Terminate();                                                /* Terminate the current file transfer process              */
            continue;
        }

        TFTPs_RxMsgCtr++;                                                     /* Keep track of the number of messages received            */

        if (TFTPs_ServerEn != DEF_ENABLED) {
            TFTPs_TxErr(0, "Transaction denied, Server DISABLED");
            continue;
        }

        p_opcode     = (CPU_INT16U *)&TFTPs_RxMsgBuf[TFTP_PKT_OFFSET_OPCODE];
        TFTPs_OpCode = NET_UTIL_NET_TO_HOST_16(*p_opcode);
        switch (TFTPs_State) {
            case TFTPs_STATE_IDLE:                                            /* Idle state, expecting a new 'connection'                 */
                 err = TFTPs_StateIdle();
                 break;

            case TFTPs_STATE_DATA_RD:                                         /* Processing a read request                                */
                 err = TFTPs_StateDataRd();
                 break;

            case TFTPs_STATE_DATA_WR:                                         /* Processing a write request                               */
                 err = TFTPs_StateDataWr();
                 break;
        }
        if (err != TFTP_ERR_NONE) {                                           /* Terminate file transfer in progress if we had an error   */
            TFTPs_TRACE(1, "Task, Error, session terminated         ");
            TFTPs_Terminate();
        }
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        TFTPs_ServerInit()
*
* Description: This function initializes the TFTP server.
*
* Arguments  : None
*
* Returns    : TFTP_ERR_NONE        if the call was succesful
*              TFTP_ERR_NO_SOCK     if we could not open a socket
*              TFTP_ERR_CANT_BIND   if we could not bind to the TFTPs port
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_ServerInit (void)
{
    int      bind_status;
    NET_ERR  err;


#if TFTPs_TRACE_EN > 0
    TFTPs_TraceInit();
#endif

    TFTPs_ServerEn = DEF_ENABLED;

                                                                                /* Open a socket to listen for incoming connections.      */
    TFTPs_SockID   = (int)NetSock_Open((CPU_INT16S)NET_SOCK_ADDR_FAMILY_IP_V4,  
                                       (CPU_INT16S)NET_SOCK_TYPE_DATAGRAM,
                                       (CPU_INT16S)NET_SOCK_PROTOCOL_UDP,
                                       (NET_ERR  *)&err);

    if (TFTPs_SockID < 0) {
        return (TFTP_ERR_NO_SOCK);                                              /* Could not open a socket                                */
    }
    
    Mem_Set((char *)&TFTPs_SockAddr, 0, NET_SOCK_ADDR_SIZE);                    /* Bind a local address so the client can send to us.     */
    TFTPs_SockAddr.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
    TFTPs_SockAddr.Port   = NET_UTIL_HOST_TO_NET_16(TFTPs_PORT_NBR_SERVER);
    TFTPs_SockAddr.Addr   = NET_UTIL_HOST_TO_NET_32(INADDR_ANY);

    bind_status           = NetSock_Bind(TFTPs_SockID,
                                        (NET_SOCK_ADDR *)&TFTPs_SockAddr,
                                         NET_SOCK_ADDR_SIZE, 
                                         &err);

    if (bind_status != NET_SOCK_BSD_ERR_NONE) {
        NetSock_Close(TFTPs_SockID, &err);                                      /* Could not bind to the TFTPs port                       */
        return (TFTP_ERR_CANT_BIND);
    }

    TFTPs_State      = TFTPs_STATE_IDLE;

    TFTPs_FileHandle = (FS_FILE *)0;

    TFTPs_RxBlkNbr   =  0;
    TFTPs_RxMsgCtr   =  0;

    TFTPs_TxBlkNbr   =  0;
    TFTPs_TxMsgCtr   =  0;
    return (TFTP_ERR_NONE);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         TFTPs_StateIdle()
*
* Description: This function is called when the TFTP server is in the IDLE state.  Valid commands received
*              in this state are: 
*
*                 RRQ    Read  request
*                 WRQ    Write request
*
*              DATA, ACK and ERR packets are rejected.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_StateIdle (void)
{
    TFTP_ERR  err;
    NET_ERR   err_sock;


    TFTPs_TRACE(10, "Idle State                              ");
    switch (TFTPs_OpCode) {
        case TFTP_OPCODE_RD_REQ:
             err = TFTPs_FileOpen(TFTPs_FILE_OPEN_RD);             /* Open the desired file for reading                              */
             if (err == TFTP_ERR_NONE) { 
                 TFTPs_TRACE(11, "Rd Request, File Opened             ");
                 TFTPs_TxBlkNbr = 0;
                 TFTPs_State    = TFTPs_STATE_DATA_RD;
                 err            = TFTPs_DataRd();                  /* Read the first block of data from the file and send to client  */
             }
             break;

        case TFTP_OPCODE_ACK:                                      /* NOT supposed to get ACKs in the Idle state                     */
             TFTPs_TRACE(12, "ACK received, not supposed to!          ");
             err = TFTP_ERR_ACK;
             break;

        case TFTP_OPCODE_WR_REQ:
             TFTPs_TxBlkNbr = 0;
             err            = TFTPs_FileOpen(TFTPs_FILE_OPEN_WR);  /* Open the desired file for writing                              */
             if (err == TFTP_ERR_NONE) { 
                 TFTPs_TRACE(13, "Wr Request, File Opened                 ");
                 TFTPs_State  = TFTPs_STATE_DATA_WR;
                 TFTPs_DataWrAck(TFTPs_TxBlkNbr);                  /* Acknowledge the client                                         */
                 err          = TFTP_ERR_NONE;
             }
             break;

        case TFTP_OPCODE_DATA:                                     /* NOT supposed to get DATA packets in the Idle state             */
             err = TFTP_ERR_DATA;
             break;

        case TFTP_OPCODE_ERR:                                      /* NOT supposed to get ERR packets in the Idle state              */
             err = TFTP_ERR_ERR;
             break;
    }
    if (err == TFTP_ERR_NONE) {
        TFTPs_TRACE(14, "No error, Timeout set                   ");
        NetOS_Sock_RxQ_TimeoutSet(TFTPs_SockID, TFTPs_BlockingTimeout, &err_sock);
    } 
    return (err);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        TFTPs_StateDataRd()
*
* Description: This function is called when the TFTP server is expecting data blocks to be read by the 
*              client.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_StateDataRd (void)
{
    TFTP_ERR     err;


    err = TFTP_ERR_NONE;
    switch (TFTPs_OpCode) {
        case TFTP_OPCODE_RD_REQ:                                   /* NOT supposed to get RRQ packets in the DATA Read state        */
             FS_FClose(TFTPs_FileHandle);                          /* Close the file and then, re-open it!                          */
             err = TFTPs_FileOpen(TFTPs_FILE_OPEN_RD);             /* Open the desired file for reading                             */
             if (err == TFTP_ERR_NONE) { 
                 TFTPs_TRACE(20, "Data Rd, Rx RD_REQ.                     ");
                 TFTPs_TxBlkNbr = 0;
                 TFTPs_State    = TFTPs_STATE_DATA_RD;
                 err            = TFTPs_DataRd();                  /* Read the first block of data from the file and send to client */
             }
             break;

        case TFTP_OPCODE_ACK:
             TFTPs_GetRxBlkNbr();
             if (TFTPs_RxBlkNbr == TFTPs_TxBlkNbr) {               /* Confirm that we got the acknowledge for the data sent         */
                 TFTPs_TRACE(21, "Data Rd, ACK Rx'd                       ");
                 err = TFTPs_DataRd();                             /* Read the first block of data from the file and send to client */
             } else {
                 TFTPs_TRACE(22, "Data Rd, Rx doesn't match Tx block#     ");
                 err = TFTP_ERR_ACK;
             }
             break;

        case TFTP_OPCODE_WR_REQ:                                   /* NOT supposed to get WRQ packets in the DATA Read state        */
             TFTPs_TRACE(23, "Data Rd, Rx'd WR_REQ                    ");
             TFTPs_TxErr(0, "RRQ server busy, WRQ  opcode?"); 
             err = TFTP_ERR_WR_REQ;
             break;

        case TFTP_OPCODE_DATA:                                     /* NOT supposed to get DATA packets in the DATA Read state       */
             TFTPs_TRACE(24, "Data Rd, Rx'd DATA                      ");
             TFTPs_TxErr(0, "RRQ server busy, DATA opcode?"); 
             err = TFTP_ERR_DATA;
             break;

        case TFTP_OPCODE_ERR:
             TFTPs_TRACE(25, "Data Rd, Rx'd ERR                       ");
             TFTPs_TxErr(0, "RRQ server busy, ERR  opcode?"); 
             err = TFTP_ERR_ERR;
             break;
    }
    return (err);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        TFTPs_StateDataWr()
*
* Description: This function is called when the client is sending data to be written in a server's file.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_StateDataWr (void)
{
    TFTP_ERR     err;



    err = TFTP_ERR_NONE;
    switch (TFTPs_OpCode) {
        case TFTP_OPCODE_RD_REQ:
             TFTPs_TRACE(30, "Data Wr, WRQ server busy, RRQ  opcode?  ");
             TFTPs_TxErr(0, "WRQ server busy, RRQ  opcode?"); 
             err = TFTP_ERR_RD_REQ;
             break;

        case TFTP_OPCODE_ACK:
             TFTPs_TRACE(31, "Data Wr, WRQ server busy, ACK  opcode?  ");
             TFTPs_TxErr(0, "WRQ server busy, ACK  opcode?"); 
             err = TFTP_ERR_ACK;
             break;

        case TFTP_OPCODE_WR_REQ:
             FS_FClose(TFTPs_FileHandle);
             TFTPs_TxBlkNbr = 0;
             err            = TFTPs_FileOpen(TFTPs_FILE_OPEN_WR);  /* Open the desired file for writing                              */
             if (err == TFTP_ERR_NONE) { 
                 TFTPs_TRACE(32, "Data Wr, Rx'd WR_REQ again              ");
                 TFTPs_State  = TFTPs_STATE_DATA_WR;
                 TFTPs_DataWrAck(TFTPs_TxBlkNbr);                  /* Acknowledge the client                                         */
                 err          = TFTP_ERR_NONE;
             }
             break;

        case TFTP_OPCODE_DATA:
             TFTPs_TRACE(33, "Data Wr, Rx'd DATA --- OK               ");
             err = TFTPs_DataWr();                                 /* Write data to file                                             */
             break;

        case TFTP_OPCODE_ERR:
             TFTPs_TRACE(34, "Data Wr, WRQ server busy, ERR  opcode?  ");
             TFTPs_TxErr(0, "WRQ server busy, ERR  opcode?"); 
             err = TFTP_ERR_ERR;
             break;
    }
    return (err);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        TFTPs_GetRxBlkNbr()
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
*                                        TFTPs_Terminate()
*
* Description: This function terminates the current file transfer process.
*********************************************************************************************************
*/

static  void  TFTPs_Terminate (void)
{
    TFTP_ERR  err_sock;


    TFTPs_State = TFTPs_STATE_IDLE;                                           /* Abort current file transfer                            */
    if (TFTPs_FileHandle != (FS_FILE *)0) {
        FS_FClose(TFTPs_FileHandle);                                          /* Close the current opened file                          */
        TFTPs_FileHandle  = (FS_FILE *)0;
    } 
    NetOS_Sock_RxQ_TimeoutSet(TFTPs_SockID, NET_TMR_TIME_INFINITE, &err_sock); /* Reset blocking timeout to infinite                     */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        TFTPs_FileOpen()
*
* Description: Reads the filename and file mode from the TFTP packet and attemps to open the file in the
*              desired file.
*
* Note(s)    : 1) This function also extracts options as specified in RFC 2349:
*
*                 "timeout"   specifies the timeout in seconds to wait in case we don't receive data 
*                             request after we initiated a read request.
*
*                 "tsize"     specifies the size of the file in bytes that the client is writing.
*
*                 Note that both these options may not be supported by the client and thus, we assume
*                 default values if they are not specified.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_FileOpen (CPU_BOOLEAN rw)
{
    CPU_CHAR  *p_filename;
    CPU_CHAR  *p_mode;
    CPU_CHAR  *p_name;
    CPU_CHAR  *p_value;

    
                                                                      /* ------------------------- GET FILENAME ------------------------ */
    p_filename = (CPU_CHAR *)&TFTPs_RxMsgBuf[TFTP_PKT_OFFSET_FILENAME];
                                                                      /* ------------------------- GET FILE MODE ----------------------- */
    p_mode     = p_filename;                                          /* Point to the 'Mode' string                                      */
    while (*p_mode > 0) {
        p_mode++;
    }
    p_mode++;
                                                                      /* --------- GET RFC2349 "timeout" OPTION (IF AVAILABLE) --------- */
    p_name = p_mode;                                                  /* Skip over the 'Mode' string                                     */
    while (*p_name > 0) {
        p_name++;
    }
    p_name++;
    if (Str_Cmp(p_name, "timeout") == 0) {                            /* See if the client specified a "timeout" string   (RFC2349)      */
        p_value = p_name;                                             /* Yes, skip over the "timeout" string                             */
        while (*p_value > 0) {
            p_value++;
        }
        p_value++;
        TFTPs_TimeoutSec = atoi(p_value);                             /*      get the timeout (in seconds)                               */
    } else {
        TFTPs_TimeoutSec = TFTPs_TIMEOUT_SEC_DFLT;                    /* No,  assume the default timeout (in seconds)                    */

                                                                      /* ---------- GET RFC2349 "tsize" OPTION (IF AVAILABLE) ---------- */
        if (Str_Cmp(p_name, "tsize") == 0) {                          /*      See if the client specified a "tsize" string   (RFC2349)   */
            p_value = p_name;
            while (*p_value > 0) {                                    /*      Yes, skip over the "tsize" string                          */
                p_value++;
            }
            p_value++;
            TFTPs_WrSize = atol(p_value);                             /*           get the size of the file to write                     */
        } else {
            TFTPs_WrSize = 0;                                         /*           assume a default value of 0                           */
        }
    }

                                                                      /* ------------------------ OPEN THE FILE ------------------------ */
    TFTPs_FileHandle = TFTPs_FileOpenMode(p_filename, p_mode, rw);
    if (TFTPs_FileHandle == (FS_FILE *)0) {
        TFTPs_TxErr(0, "file not found");
        return (TFTP_ERR_FILE_NOT_FOUND);
    }
    return (TFTP_ERR_NONE);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        TFTPs_FileOpenMode()
*
* Description: Opens the file for Read or Write
*********************************************************************************************************
*/

static  FS_FILE *TFTPs_FileOpenMode (CPU_CHAR *filename, CPU_CHAR *mode, CPU_BOOLEAN rw)
{
    FS_FILE     *handle;
    CPU_INT16S   result;


    switch (rw) {
        case TFTPs_FILE_OPEN_RD:
             result = Str_Cmp(mode, "octet");
             if (result == 0) {
                 handle = FS_FOpen(filename, "rb");
                 return (handle);
             }
             result = Str_Cmp(mode, "netascii");
             if (result == 0) {
                 handle = FS_FOpen(filename, "r");
                 return (handle);
             }
             return ((FS_FILE *)0);

        case TFTPs_FILE_OPEN_WR:
             result = Str_Cmp(mode, "octet");
             if (result == 0) {
                 handle = FS_FOpen(filename, "wb");
                 return (handle);
             }
             result = Str_Cmp(mode, "netascii");
             if (result == 0) {
                 handle = FS_FOpen(filename, "w");
                 return (handle);
             }
             return ((FS_FILE *)0);

        default:     
             return ((FS_FILE *)0);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         TFTPs_DataRd()
*
* Description: This function reads data from the file system and sends it to the client.
*********************************************************************************************************
*/

static  TFTP_ERR  TFTPs_DataRd (void)
{
    CPU_INT16S  tx_size;


                                                                      /* Read data from the file                   */
    TFTPs_TxMsgLen = (int)FS_FRead((void *)&TFTPs_TxMsgBuf[TFTP_PKT_OFFSET_DATA], 
                                   (FS_SIZE_T)1, 
                                   (FS_SIZE_T)512, 
                                   TFTPs_FileHandle);

    if (TFTPs_TxMsgLen < 512) {                                       /* Close the file when we are done           */
        FS_FClose(TFTPs_FileHandle);
        TFTPs_State = TFTPs_STATE_IDLE;
    }

    if (TFTPs_TxMsgLen < 0) {                                         /* See if we had an error reading the file   */
        TFTPs_TxErr(0, "RRQ file read error");                        /* Yes, send error packet                    */
        return (TFTP_ERR_FILE_RD);
    }

    TFTPs_TxMsgCtr++;
    TFTPs_TxBlkNbr++;

    TFTPs_TxMsgLen += TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR;

    tx_size         = TFTPs_Tx(TFTP_OPCODE_DATA, TFTPs_TxBlkNbr, &TFTPs_TxMsgBuf[0], TFTPs_TxMsgLen);

    if (tx_size < 0) {
        TFTPs_TxErr(0, "RRQ file read error");                        /* Yes, send error packet                    */
        return (TFTP_ERR_TX);
    }

    return (TFTP_ERR_NONE);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      TFTPs_DataWr()
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

    if (data_bytes > 0) {                                             /* Write data to the file                         */
        FS_FWrite((void *)&TFTPs_RxMsgBuf[TFTP_PKT_OFFSET_DATA], 
                  (FS_SIZE_T)1, 
                  (FS_SIZE_T)data_bytes, 
                  TFTPs_FileHandle);
    }
    if (data_bytes < 512) {                                           /* See if we are done writing the file            */ 
        FS_FClose(TFTPs_FileHandle);                                  /* Close the file                                 */
        TFTPs_State = TFTPs_STATE_IDLE;
    }

    p_blk_nbr      = (CPU_INT16U *)&TFTPs_RxMsgBuf[TFTP_PKT_OFFSET_BLK_NBR];
    blk_nbr        = NET_UTIL_NET_TO_HOST_16(*p_blk_nbr);
    TFTPs_TxBlkNbr = blk_nbr;
    TFTPs_DataWrAck(blk_nbr);

    return (TFTP_ERR_NONE);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         TFTPs_DataWrAck()
*
* Description: This function sends an acknowledge to the client.
*********************************************************************************************************
*/

static  void  TFTPs_DataWrAck (CPU_INT32U blk_nbr)
{
    CPU_INT16S  tx_len;


    tx_len = TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR;
    TFTPs_TxMsgCtr++;

    TFTPs_Tx(TFTP_OPCODE_ACK, blk_nbr, &TFTPs_TxMsgBuf[0], tx_len);
}

/*
*********************************************************************************************************
*                                           TFTPs_Tx()
*
* Description: This function is called to send TFTP reply packets.
*********************************************************************************************************
*/

static  CPU_INT16S  TFTPs_Tx (CPU_INT16U opcode, CPU_INT16U blk_nbr, CPU_INT08U *p_buf, CPU_INT16U tx_len)
{
    CPU_INT16U  *pbuf16;
    CPU_INT16S   bytes_sent;
    NET_ERR      err;



    pbuf16     = (CPU_INT16U *)&TFTPs_TxMsgBuf[TFTP_PKT_OFFSET_OPCODE];
    *pbuf16    = NET_UTIL_NET_TO_HOST_16(opcode);
    pbuf16     = (CPU_INT16U *)&TFTPs_TxMsgBuf[TFTP_PKT_OFFSET_BLK_NBR];
    *pbuf16    = NET_UTIL_NET_TO_HOST_16(blk_nbr);
    bytes_sent = NetSock_TxDataTo(TFTPs_SockID,
                                  p_buf,
                                  tx_len,
                                  0,
                                  (NET_SOCK_ADDR *)&TFTPs_SockAddr,
                                  NET_SOCK_ADDR_SIZE,
                                  &err);

    return (bytes_sent);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         TFTPs_TxErr()
*
* Description: Used as a convenient mechanism to send an error message to the client, when the server 
*              detects something wrong.
*********************************************************************************************************
*/

static  void  TFTPs_TxErr (CPU_INT16U err_code, CPU_CHAR *err_msg)
{
    CPU_INT16S  tx_len;


    tx_len = Str_Len(err_msg) + TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR + 1;

    Str_Copy((CPU_CHAR *)&TFTPs_TxMsgBuf[TFTP_PKT_OFFSET_DATA], err_msg);

    TFTPs_Tx(TFTP_OPCODE_ERR, err_code, &TFTPs_TxMsgBuf[0], tx_len);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        TFTPs_TraceInit()
*
* Description: This function is called to initialize the trace debug feature of this module.
*********************************************************************************************************
*/

#if  TFTPs_TRACE_EN > 0
static  void  TFTPs_TraceInit (void)
{
    CPU_INT16U  i;


    TFTPs_TraceIx = 0;
    for (i = 0; i < TFTPs_TRACE_MAX; i++) {
        TFTPs_TraceTbl[i].Id = 0;
        TFTPs_TraceTbl[i].TS = (INT32U)0;
    }
}
#endif

/*
*********************************************************************************************************
*                                          TFTPs_Trace()
*
* Description: This function is used to record traces of execution of this module.
*********************************************************************************************************
*/

#if  TFTPs_TRACE_EN > 0
static  void  TFTPs_Trace (CPU_INT16U id, CPU_CHAR *s)
{
    TFTPs_TraceTbl[TFTPs_TraceIx].Id    = id;
    TFTPs_TraceTbl[TFTPs_TraceIx].TS    = OSTimeGet();
    TFTPs_TraceTbl[TFTPs_TraceIx].State = TFTPs_State;

    Str_Copy(TFTPs_TraceTbl[TFTPs_TraceIx].Str, s);

    TFTPs_TraceTbl[TFTPs_TraceIx].RxBlkNbr = TFTPs_RxBlkNbr;
    TFTPs_TraceTbl[TFTPs_TraceIx].TxBlkNbr = TFTPs_TxBlkNbr;

    TFTPs_TraceIx++;
    if (TFTPs_TraceIx >= TFTPs_TRACE_MAX) {
        TFTPs_TraceIx = 0;
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          TFTPs_Disp()
*
* Description: This function populates a two dimensional array of characters with information about
*              the TFTPs module for purpose of displaying on an ASCII display.
*********************************************************************************************************
*/

#if TFTPs_DISP_EN > 0
void  TFTPs_Disp (void)
{
    CPU_CHAR  s[TFTPs_DISP_MAX_X];
    NET_ERR   err;


                                  /*           1111111111222222222233333333334444444444555555555566666666667777777777 */
                                  /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    Str_Copy(&TFTPs_DispTbl[ 0][0], "------------------------------------ TFTPs ------------------------------------");
    Str_Copy(&TFTPs_DispTbl[ 1][0], "State : xxxxxxxxxx                                                             ");
    Str_Copy(&TFTPs_DispTbl[ 2][0], "OpCode:                                                                        ");
    Str_Copy(&TFTPs_DispTbl[ 3][0], "                                                                               ");
    Str_Copy(&TFTPs_DispTbl[ 4][0], "Rx Msg Ctr: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[ 5][0], "Rx Block #: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[ 6][0], "Rx Msg Len: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[ 7][0], "Rx Msg    : xx xx xx xx xx xx xx xx xx xx xx                                   ");
    Str_Copy(&TFTPs_DispTbl[ 8][0], "                                                                               ");
    Str_Copy(&TFTPs_DispTbl[ 9][0], "Tx Msg Ctr: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[10][0], "Tx Block #: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[11][0], "Tx Msg Len: xxxxx                                                              ");
    Str_Copy(&TFTPs_DispTbl[12][0], "Tx Msg    : xx xx xx xx xx xx xx xx xx xx xx                                   ");
    Str_Copy(&TFTPs_DispTbl[13][0], "                                                                               ");
    Str_Copy(&TFTPs_DispTbl[14][0], "                                                                               ");
    Str_Copy(&TFTPs_DispTbl[15][0], "Source IP :                                                                    ");
    Str_Copy(&TFTPs_DispTbl[16][0], "Dest   IP :                                                                    ");
                                  /*           1111111111222222222233333333334444444444555555555566666666667777777777 */
                                  /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */

    switch (TFTPs_State) {                                  /* Display state of TFTPs state machine           */
        case TFTPs_STATE_IDLE:
             Str_Copy(&TFTPs_DispTbl[1][8], "IDLE      ");
             break;

        case TFTPs_STATE_DATA_RD:
             Str_Copy(&TFTPs_DispTbl[1][8], "DATA READ ");
             break;

        case TFTPs_STATE_DATA_WR:
             Str_Copy(&TFTPs_DispTbl[1][8], "DATA WRITE");
             break;
    };

    switch (TFTPs_OpCode) {                                 /* Display Op-Code received                       */
        case 0:
             Str_Copy(&TFTPs_DispTbl[2][8], "-NONE-");
             break;

        case TFTP_OPCODE_RD_REQ:
             Str_Copy(&TFTPs_DispTbl[2][8], "RD REQ");
             break;

        case TFTP_OPCODE_WR_REQ:
             Str_Copy(&TFTPs_DispTbl[2][8], "WR REQ");
             break;

        case TFTP_OPCODE_DATA:
             Str_Copy(&TFTPs_DispTbl[2][8], "DATA  ");
             break;

        case TFTP_OPCODE_ACK:
             Str_Copy(&TFTPs_DispTbl[2][8], "ACK   ");
             break;

        case TFTP_OPCODE_ERR:
             Str_Copy(&TFTPs_DispTbl[2][8], "ERR   ");
             break;
    };

    sprintf(s, "%5u", (int)TFTPs_RxMsgCtr);                 /* Display #messages received                     */
    Str_Copy(&TFTPs_DispTbl[4][12], s);

    sprintf(s, "%5u", (int)TFTPs_RxBlkNbr);                 /* Display current block number                   */
    Str_Copy(&TFTPs_DispTbl[5][12], s);

    if (TFTPs_RxMsgLen < 10000) {
        sprintf(s, "%5u", (int)TFTPs_RxMsgLen);             /* Display received message length                */
        Str_Copy(&TFTPs_DispTbl[6][12], s);
    }

    sprintf(s, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", 
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


    sprintf(s, "%5u", (int)TFTPs_TxMsgCtr);                 /* Display #messages sent                         */
    Str_Copy(&TFTPs_DispTbl[ 9][12], s);

    sprintf(s, "%5u", (int)TFTPs_TxBlkNbr);                 /* Display current block number                   */
    Str_Copy(&TFTPs_DispTbl[10][12], s);

    if (TFTPs_TxMsgLen < 10000) {
        sprintf(s, "%5u", (int)TFTPs_TxMsgLen);             /* Display sent message length                    */
        Str_Copy(&TFTPs_DispTbl[11][12], s);
    }

    sprintf(s, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", 
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

/*$PAGE*/
/*
*********************************************************************************************************
*                                          TFTPs_DispTrace()
*
* Description: This function populates a two dimensional array of characters with information about
*              the TFTPs TRACE for purpose of displaying on an ASCII display.
*********************************************************************************************************
*/

#if (TFTPs_DISP_EN > 0) && (TFTPs_TRACE_EN > 0)
void  TFTPs_DispTrace (void)
{
    CPU_INT16U  i;
    CPU_CHAR    s[TFTPs_DISP_MAX_X];
    CPU_CHAR   *ps;


                                  /*           1111111111222222222233333333334444444444555555555566666666667777777777 */
                                  /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    Str_Copy(&TFTPs_DispTbl[ 0][0], "--------------------------------- TFTPs TRACE ----------------------------------");
    Str_Copy(&TFTPs_DispTbl[ 1][0], " TS     ID     State  Rx#   Tx#                                                 ");
    Str_Copy(&TFTPs_DispTbl[ 2][0], "                      xxxxx xxxxx  x                                            ");
    Str_Copy(&TFTPs_DispTbl[ 3][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 4][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 5][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 6][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 7][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 8][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[ 9][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[10][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[11][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[12][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[13][0], "                                                                                ");
    Str_Copy(&TFTPs_DispTbl[14][0], "                                                                                ");
                                  /*           1111111111222222222233333333334444444444555555555566666666667777777777 */
                                  /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */

    for (i = 0; i < TFTPs_TRACE_MAX; i++) {
        ps = &TFTPs_DispTbl[i + 2][0];

        if (i == TFTPs_TraceIx) {                                             /* Indicate current position of trace                       */
            Str_Copy(ps, ">");
        } else {
            Str_Copy(ps, " ");
        }
                  
        sprintf(s, "%5u  %5u", 
                (int)(TFTPs_TraceTbl[i].TS & 0xFFFF),
                (int)(TFTPs_TraceTbl[i].Id)); 
        Str_Cat(ps, s);

        switch (TFTPs_TraceTbl[i].State) {
            case TFTPs_STATE_IDLE:                                            /* Idle state, expecting a new 'connection'                 */
                 Str_Cat(ps, "  IDLE ");
                 break;

            case TFTPs_STATE_DATA_RD:                                         /* Processing a read request                                */
                 Str_Cat(ps, "  RD   ");
                 break;

            case TFTPs_STATE_DATA_WR:                                         /* Processing a write request                               */
                 Str_Cat(ps, "  WR   ");
                 break;

            default:
                 Str_Cat(ps, "  ERROR");
                 break;
        }

        sprintf(s, "  %5u  %5u  ", 
                (int)TFTPs_TraceTbl[i].RxBlkNbr,
                (int)TFTPs_TraceTbl[i].TxBlkNbr); 
        Str_Cat(ps, s);

        Str_Cat(ps, TFTPs_TraceTbl[i].Str);
    }
}
#endif
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	   		 		  	   		   	 			       	   	  		      		      		  	 	 		  				 	      		   	  		  				 	  	   		      		    		 	       	  	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		      	   	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
