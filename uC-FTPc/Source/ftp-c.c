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
* Filename      : ftp-c.c
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   FTSc_MODULE
#include  <includes.h>
#include  <net.h>
#include  <ftp-c.h>

#if (FTPc_CFG_USE_FS > 0)
#include  <fs_api.h>
#endif


/*
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*/

static  CPU_INT32S  FTPc_CTRL_SOCK_ID;
static  CPU_INT32S  FTPc_DTP_SOCK_ID;


/*
*********************************************************************************************************
*                                           INITIALIZED DATA
*********************************************************************************************************
*/

                                                                /* This structure is used to build a table of command   */
                                                                /* codes and their corresponding string.  The context   */
                                                                /* is the state(s) in which the command is allowed.     */
static  const  FTPc_CMD_STRUCT  FTPc_Cmd[] = {
    {FTP_CMD_NOOP, "NOOP"},
    {FTP_CMD_QUIT, "QUIT"},
    {FTP_CMD_REIN, "REIN"},
    {FTP_CMD_SYST, "SYST"},
    {FTP_CMD_FEAT, "FEAT"},
    {FTP_CMD_HELP, "HELP"},
    {FTP_CMD_USER, "USER"},
    {FTP_CMD_PASS, "PASS"},
    {FTP_CMD_MODE, "MODE"},
    {FTP_CMD_TYPE, "TYPE"},
    {FTP_CMD_STRU, "STRU"},
    {FTP_CMD_PASV, "PASV"},
    {FTP_CMD_PORT, "PORT"},
    {FTP_CMD_PWD,  "PWD" },
    {FTP_CMD_CWD,  "CWD" },
    {FTP_CMD_CDUP, "CDUP"},
    {FTP_CMD_MKD,  "MKD" },
    {FTP_CMD_RMD,  "RMD" },
    {FTP_CMD_NLST, "NLST"},
    {FTP_CMD_LIST, "LIST"},
    {FTP_CMD_RETR, "RETR"},
    {FTP_CMD_STOR, "STOR"},
    {FTP_CMD_APPE, "APPE"},
    {FTP_CMD_REST, "REST"},
    {FTP_CMD_DELE, "DELE"},
    {FTP_CMD_RNFR, "RNFR"},
    {FTP_CMD_RNTO, "RNTO"},
    {FTP_CMD_SIZE, "SIZE"},
    {FTP_CMD_MDTM, "MDTM"},
    {FTP_CMD_MAX,  "MAX" }                                      /* This line MUST be the LAST!                          */
};


/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  CPU_INT32U   FTPc_WaitForStatus (CPU_INT32S   sock_id,
                                         CPU_CHAR    *net_buf,
                                         CPU_INT16U   net_buf_size,
                                         NET_ERR     *p_net_err);

static  CPU_BOOLEAN  FTPc_RxLine        (CPU_INT32S   sock_id,
                                         CPU_CHAR    *net_buf,
                                         CPU_INT16U   net_buf_size,
                                         NET_ERR     *p_net_err);

static  CPU_BOOLEAN  FTPc_Tx            (CPU_INT32S   sock_id,
                                         CPU_CHAR    *net_buf,
                                         CPU_INT16U   net_buf_size,
                                         NET_ERR     *p_net_err);


/*
*********************************************************************************************************
*                                              FTPc_Open()
*
* Description : Open connection to an FTP server.
*
* Argument(s) : server_ip       IP address of the server.
*               server_port     IP port of the server.
*               user            account username on the server.
*               pass            account password on the server..
*
* Return(s)   : DEF_FAIL        connection failed.
*               DEF_OK          connection successful.
*
* Caller(s)   : Application.
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPc_Open  (NET_IP_ADDR   server_ip,
                         CPU_INT16U    server_port,
                         CPU_CHAR     *user,
                         CPU_CHAR     *pass,
                         NET_ERR      *p_net_err)
{
    NET_SOCK_ADDR      server_addr;
    NET_SOCK_ADDR_IP  *p_server_addr;
    CPU_CHAR           buf[FTPc_NET_BUF_SIZE];
    CPU_INT32U         buf_size;

    CPU_INT32U         reply_code;
    CPU_BOOLEAN        rtn_code;


                                                                /* Open control socket.                                 */
    FTPc_TRACE_INFO(("FTPc OPEN CTRL socket.\n"));
    FTPc_CTRL_SOCK_ID = NetSock_Open(NET_SOCK_ADDR_FAMILY_IP_V4,
                                     NET_SOCK_TYPE_STREAM,
                                     NET_SOCK_PROTOCOL_TCP,
                                     p_net_err);

    if (*p_net_err == NET_SOCK_ERR_NONE) {
         Mem_Set(&server_addr, (CPU_CHAR)0, sizeof(server_addr));
         p_server_addr         = (NET_SOCK_ADDR_IP *)&server_addr;
         p_server_addr->Family =  NET_SOCK_ADDR_FAMILY_IP_V4;
         p_server_addr->Addr   =  NET_UTIL_HOST_TO_NET_32(server_ip);
         p_server_addr->Port   =  NET_UTIL_HOST_TO_NET_16(server_port);

                                                                /* Set connect timemout.                                */
        NetOS_Sock_ConnReqTimeoutSet(FTPc_CTRL_SOCK_ID, FTPc_CFG_CTRL_MAX_CONN_TIMEOUT_S * DEF_TIME_NBR_mS_PER_SEC, p_net_err);

                                                                /* Connect control socket.                              */
        FTPc_TRACE_INFO(("FTPc CONN CTRL socket.\n"));
        NetSock_Conn( FTPc_CTRL_SOCK_ID,
                     (NET_SOCK_ADDR *)&server_addr,
                      sizeof(NET_SOCK_ADDR),
                      p_net_err);
        if ((*p_net_err != NET_SOCK_ERR_NONE) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_EMPTY) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_CLOSED)) {
              FTPc_TRACE_DBG(("FTPc NetSock_Conn() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
              return (DEF_FAIL);
        }
        if ((*p_net_err == NET_SOCK_ERR_RX_Q_EMPTY) ||
            (*p_net_err == NET_SOCK_ERR_RX_Q_CLOSED)) {
             FTPc_TRACE_DBG(("FTPc NetSock_Conn() timeout, line #%d.\n", __LINE__));
             return (DEF_FAIL);
        }
    } else {
        FTPc_TRACE_DBG(("FTPc NetSock_Open() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
        return (DEF_FAIL);
    }

                                                                /* Receive status lines until "server ready status".    */
    while (DEF_OK) {
        reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
        FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
        if (reply_code != FTP_REPLY_CODE_SERVERREADY) {
            continue;
        }
        break;
    }

                                                                /* ******** USERNAME ************************************/
                                                                /* Send USER command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %s\r\n", FTPc_Cmd[FTP_CMD_USER].CmdStr, user);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_NEEDPASSWORD) {
        return (DEF_FAIL);
    }

                                                                /* ******** PASSWORD ************************************/
                                                                /* Send PASS command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %s\r\n", FTPc_Cmd[FTP_CMD_PASS].CmdStr, pass);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_LOGGEDIN) {
        return (DEF_FAIL);
    }

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                             FTPc_Close()
*
* Description : Close FTP connection.
*
* Argument(s) : None.
*
* Return(s)   : DEF_FAIL        FTP connection close failed.
*               DEF_OK          FTP connection close successful.
*
* Caller(s)   : Application.
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPc_Close  (NET_ERR  *p_net_err)
{
    CPU_CHAR      buf[FTPc_NET_BUF_SIZE];
    CPU_INT32U    buf_size;

    CPU_INT32U    reply_code;
    CPU_BOOLEAN   rtn_code;


                                                                /* Send QUIT command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s\r\n", FTPc_Cmd[FTP_CMD_QUIT].CmdStr);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        NetSock_Close(FTPc_CTRL_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE CTRL socket.\n"));
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_SERVERCLOSING) {
        NetSock_Close(FTPc_CTRL_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE CTRL socket.\n"));
        return (DEF_FAIL);
    }

                                                                /* Close socket.                                        */
    NetSock_Close(FTPc_CTRL_SOCK_ID, p_net_err);
    FTPc_TRACE_INFO(("FTPc CLOSE CTRL socket.\n"));
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                            FTPc_RecvBuf()
*
* Description : Receive a file from an FTP server into a memory buffer.
*
* Argument(s) : remote_file_name        name of the file in FTP server.
*               file                    memory buffer to hold received file.
*               file_size               size of the memory buffer.
*
* Return(s)   : DEF_FAIL        transmission failed.
*               DEF_OK          transmission successful.
*
* Caller(s)   : Application.
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPc_RecvBuf  (CPU_CHAR    *remote_file_name,
                            CPU_INT08U  *file,
                            CPU_INT32U   file_size,
                            NET_ERR     *p_net_err)
{
    NET_IP_ADDR        server_ip;
    CPU_INT16U         server_port;
    NET_SOCK_ADDR      server_addr;
    NET_SOCK_ADDR_IP  *p_server_addr;
    CPU_CHAR           buf[FTPc_NET_BUF_SIZE];
    CPU_INT32U         buf_size;

    CPU_INT32U         reply_code;
    CPU_BOOLEAN        rtn_code;
    CPU_INT32U         rx_pkt_cnt;

    CPU_CHAR          *tmp_buf;
    CPU_INT32S         tmp_val;
    CPU_INT32U         rx_len;
    CPU_INT32U         bytes_recv;
    CPU_INT32U         got_file_size;

                                                                /* Send TYPE command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %c\r\n", FTPc_Cmd[FTP_CMD_TYPE].CmdStr, FTP_TYPE_IMAGE);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_OKAY) {
        return (DEF_FAIL);
    }

                                                                /* Send SIZE command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %s\r\n", FTPc_Cmd[FTP_CMD_SIZE].CmdStr, remote_file_name);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_FILESTATUS) {
        return (DEF_FAIL);
    }

    tmp_buf       = buf;
    tmp_val       = Str_ToLong(tmp_buf, &tmp_buf, 10);         /* Skip result code.                                    */
    tmp_buf++;

    tmp_val       = Str_ToLong(tmp_buf, &tmp_buf, 10);         /* Get file size.                                       */
    tmp_buf++;
    got_file_size = tmp_val;
    if (got_file_size > file_size) {
        return (DEF_FAIL);
    }

                                                                /* Send PASV command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s\r\n", FTPc_Cmd[FTP_CMD_PASV].CmdStr);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_ENTERPASVMODE) {
        return (DEF_FAIL);
    }

    server_ip    = 0;
    server_port  = 0;
    tmp_buf      = Str_Char(buf, '(');
    tmp_buf++;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Address MSB.                                  */
    tmp_buf++;
    server_ip   += tmp_val << 24;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);
    tmp_buf++;
    server_ip   += tmp_val << 16;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);
    tmp_buf++;
    server_ip   += tmp_val << 8;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Address LSB.                                  */
    tmp_buf++;
    server_ip   += tmp_val << 0;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Port MSB.                                     */
    tmp_buf++;
    server_port += tmp_val << 8;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Port LSB.                                     */
    tmp_buf++;
    server_port += tmp_val << 0;

                                                                /* Open data socket.                                    */
    FTPc_TRACE_INFO(("FTPc OPEN DTP socket.\n"));
    FTPc_DTP_SOCK_ID = NetSock_Open(NET_SOCK_ADDR_FAMILY_IP_V4,
                                    NET_SOCK_TYPE_STREAM,
                                    NET_SOCK_PROTOCOL_TCP,
                                    p_net_err);

    if (*p_net_err == NET_SOCK_ERR_NONE) {
         Mem_Set(&server_addr, (CPU_CHAR)0, sizeof(server_addr));
         p_server_addr         = (NET_SOCK_ADDR_IP *)&server_addr;
         p_server_addr->Family =  NET_SOCK_ADDR_FAMILY_IP_V4;
         p_server_addr->Addr   =  NET_UTIL_HOST_TO_NET_32(server_ip);
         p_server_addr->Port   =  NET_UTIL_HOST_TO_NET_16(server_port);

                                                                /* Set connect timemout.                                */
        NetOS_Sock_ConnReqTimeoutSet(FTPc_DTP_SOCK_ID, FTPc_CFG_DTP_MAX_CONN_TIMEOUT_S * DEF_TIME_NBR_mS_PER_SEC, p_net_err);

                                                                /* Connect data socket.                                 */
        FTPc_TRACE_INFO(("FTPc CONN DTP socket.\n"));
        NetSock_Conn( FTPc_DTP_SOCK_ID,
                     (NET_SOCK_ADDR *)&server_addr,
                      sizeof(NET_SOCK_ADDR),
                      p_net_err);
        if ((*p_net_err != NET_SOCK_ERR_NONE) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_EMPTY) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_CLOSED)) {
              FTPc_TRACE_DBG(("FTPc NetSock_Conn() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
              NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
              FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
              return (DEF_FAIL);
        }
        if ((*p_net_err == NET_SOCK_ERR_RX_Q_EMPTY) ||
            (*p_net_err == NET_SOCK_ERR_RX_Q_CLOSED)) {
             FTPc_TRACE_DBG(("FTPc NetSock_Conn() timeout, line #%d.\n", __LINE__));
             NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
             FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
             return (DEF_FAIL);
        }
    } else {
        FTPc_TRACE_DBG(("FTPc NetSock_Open() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

                                                                /* Send RETR command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %s\r\n", FTPc_Cmd[FTP_CMD_RETR].CmdStr, remote_file_name);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_OKAYOPENING) {
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

    NetOS_TCP_RxQ_TimeoutSet(FTPc_DTP_SOCK_ID, FTPc_CFG_DTP_MAX_CONN_TIMEOUT_S * DEF_TIME_NBR_mS_PER_SEC, p_net_err);

    bytes_recv = 0;
	rx_pkt_cnt = 0;
    while (bytes_recv < got_file_size) {

        tmp_buf = file + bytes_recv;
        rx_len  = got_file_size - bytes_recv;
        if (rx_len > DEF_INT_16S_MAX_VAL) {
            rx_len = DEF_INT_16S_MAX_VAL;
        }

		tmp_val = NetSock_RxData(FTPc_DTP_SOCK_ID, tmp_buf, rx_len, NET_SOCK_FLAG_NONE, p_net_err);
        if (tmp_val > 0) {
            bytes_recv += tmp_val;
        }
        if ((*p_net_err != NET_SOCK_ERR_NONE) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_EMPTY) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_CLOSED)) {
              FTPc_TRACE_DBG(("FTPc NetSock_RxData() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
              break;
        }

        FTPc_TRACE_DBG(("FTPc RX DATA #%03d.\n", rx_pkt_cnt));

        /* In this case, a timeout represent an end-of-file     */
        /* condition.                                           */
        if ((*p_net_err == NET_SOCK_ERR_RX_Q_EMPTY) ||
            (*p_net_err == NET_SOCK_ERR_RX_Q_CLOSED)) {
             break;
        }
        rx_pkt_cnt++;
    }

                                                                /* Close socket.                                        */
    NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
    FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_CLOSINGSUCCESS) {
        return (DEF_FAIL);
    }

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                            FTPc_SendBuf()
*
* Description : Send a memory buffer to an FTP server.
*
* Argument(s) : remote_file_name        name of the file in FTP server.
*               file                    memory buffer to send.
*               file_size               size of the memory buffer.
*               append                  if DEF_TRUE, existing file on FTP server will be appended with
*                                           memory buffer.  If file doesn't exist on FTP server, it will be
*                                           created.
*                                       if DEF_FALSE, existing file on FTP server will be overwritten.
*                                           If file doesn't exist on FTP server, it will be created.
*
* Return(s)   : DEF_FAIL        transmission failed.
*               DEF_OK          transmission successful.
*
* Caller(s)   : Application.
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPc_SendBuf  (CPU_CHAR     *remote_file_name,
                            CPU_INT08U   *file,
                            CPU_INT32U    file_size,
                            CPU_BOOLEAN   append,
                            NET_ERR      *p_net_err)
{
    NET_IP_ADDR        server_ip;
    CPU_INT16U         server_port;
    NET_SOCK_ADDR      server_addr;
    NET_SOCK_ADDR_IP  *p_server_addr;
    CPU_CHAR           buf[FTPc_NET_BUF_SIZE];
    CPU_INT32U         buf_size;

    CPU_INT32U         reply_code;
    CPU_BOOLEAN        rtn_code;

    CPU_CHAR          *tmp_buf;
    CPU_INT08U         tmp_val;
    CPU_INT32U         bytes_sent;
    CPU_INT32U         tx_pkt_cnt;


                                                                /* Send TYPE command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %c\r\n", FTPc_Cmd[FTP_CMD_TYPE].CmdStr, FTP_TYPE_IMAGE);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_OKAY) {
        return (DEF_FAIL);
    }

                                                                /* Send PASV command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s\r\n", FTPc_Cmd[FTP_CMD_PASV].CmdStr);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_ENTERPASVMODE) {
        return (DEF_FAIL);
    }

    server_ip    = 0;
    server_port  = 0;
    tmp_buf      = Str_Char(buf, '(');
    tmp_buf++;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Address MSB.                                  */
    tmp_buf++;
    server_ip   += tmp_val << 24;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);
    tmp_buf++;
    server_ip   += tmp_val << 16;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);
    tmp_buf++;
    server_ip   += tmp_val << 8;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Address LSB.                                  */
    tmp_buf++;
    server_ip   += tmp_val << 0;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Port MSB.                                     */
    tmp_buf++;
    server_port += tmp_val << 8;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Port LSB.                                     */
    tmp_buf++;
    server_port += tmp_val << 0;

                                                                /* Open data socket.                                    */
    FTPc_TRACE_INFO(("FTPc OPEN DTP socket.\n"));
    FTPc_DTP_SOCK_ID = NetSock_Open(NET_SOCK_ADDR_FAMILY_IP_V4,
                                    NET_SOCK_TYPE_STREAM,
                                    NET_SOCK_PROTOCOL_TCP,
                                    p_net_err);

    if (*p_net_err == NET_SOCK_ERR_NONE) {
         Mem_Set(&server_addr, (CPU_CHAR)0, sizeof(server_addr));
         p_server_addr         = (NET_SOCK_ADDR_IP *)&server_addr;
         p_server_addr->Family =  NET_SOCK_ADDR_FAMILY_IP_V4;
         p_server_addr->Addr   =  NET_UTIL_HOST_TO_NET_32(server_ip);
         p_server_addr->Port   =  NET_UTIL_HOST_TO_NET_16(server_port);

                                                                /* Set connect timemout.                                */
        NetOS_Sock_ConnReqTimeoutSet(FTPc_DTP_SOCK_ID, FTPc_CFG_DTP_MAX_CONN_TIMEOUT_S * DEF_TIME_NBR_mS_PER_SEC, p_net_err);

                                                                /* Connect data socket.                                 */
        FTPc_TRACE_INFO(("FTPc CONN DTP socket.\n"));
        NetSock_Conn( FTPc_DTP_SOCK_ID,
                     (NET_SOCK_ADDR *)&server_addr,
                      sizeof(NET_SOCK_ADDR),
                      p_net_err);
        if ((*p_net_err != NET_SOCK_ERR_NONE) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_EMPTY) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_CLOSED)) {
              FTPc_TRACE_DBG(("FTPc NetSock_Conn() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
              NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
              FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
              return (DEF_FAIL);
        }
        if ((*p_net_err == NET_SOCK_ERR_RX_Q_EMPTY) ||
            (*p_net_err == NET_SOCK_ERR_RX_Q_CLOSED)) {
             FTPc_TRACE_DBG(("FTPc NetSock_Conn() timeout, line #%d.\n", __LINE__));
             NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
             FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
             return (DEF_FAIL);
        }
    } else {
        FTPc_TRACE_DBG(("FTPc NetSock_Open() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

    if (append == DEF_YES) {
                                                                /* Send APPE command.                                   */
        buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %s\r\n", FTPc_Cmd[FTP_CMD_APPE].CmdStr, remote_file_name);
        rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
        FTPc_TRACE_INFO(("FTPc TX: %s", buf));
        if (rtn_code == DEF_FAIL) {
            NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
            FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
            return (DEF_FAIL);
        }
    } else {
                                                                /* Send STOR command.                                   */
        buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %s\r\n", FTPc_Cmd[FTP_CMD_STOR].CmdStr, remote_file_name);
        rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
        FTPc_TRACE_INFO(("FTPc TX: %s", buf));
        if (rtn_code == DEF_FAIL) {
            NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
            FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
            return (DEF_FAIL);
        }
    }
                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_OKAYOPENING) {
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

    bytes_sent = 0;
    tx_pkt_cnt = 0;
    while (bytes_sent < file_size) {
        buf_size = file_size - bytes_sent;
        if (buf_size > FTPc_NET_BUF_SIZE) {
            buf_size = FTPc_NET_BUF_SIZE;
        }

        FTPc_TRACE_DBG(("FTPc TX DATA #%03d... ", tx_pkt_cnt));
        rtn_code = FTPc_Tx(FTPc_DTP_SOCK_ID, file, buf_size, p_net_err);
        if (rtn_code == DEF_FAIL) {
            FTPc_TRACE_DBG(("FTPc FTPc_Tx() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
            break;
        }

        FTPc_TRACE_DBG(("\n"));
        bytes_sent += buf_size;
        file       += buf_size;
        tx_pkt_cnt++;
    }

                                                                /* Close socket.                                        */
    NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
    FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    if (reply_code != FTP_REPLY_CODE_CLOSINGSUCCESS) {
        return (DEF_FAIL);
    }
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                            FTPc_RecvFile()
*
* Description : Receive a file from an FTP server to the file system.
*
* Argument(s) : remote_file_name        name of the file in FTP server.
*               local_file_name         name of the file in filesystem.
*
* Return(s)   : DEF_FAIL        transmission failed.
*               DEF_OK          transmission successful.
*
* Caller(s)   : Application.
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPc_RecvFile (CPU_CHAR  *remote_file_name,
                            CPU_CHAR  *local_file_name,
                            NET_ERR   *p_net_err)
{
#if (FTPc_CFG_USE_FS > 0)
    NET_IP_ADDR        server_ip;
    CPU_INT16U         server_port;
    NET_SOCK_ADDR      server_addr;
    NET_SOCK_ADDR_IP  *p_server_addr;
    CPU_CHAR           buf[FTPc_NET_BUF_SIZE];
    CPU_INT32U         buf_size;

    CPU_INT32U         reply_code;
    CPU_BOOLEAN        rtn_code;

    CPU_CHAR          *tmp_buf;
    CPU_INT32S         tmp_val;
    CPU_INT32U         rx_pkt_cnt;

    FS_FILE           *p_file;
    CPU_INT32S         fs_len;
    CPU_INT32S         fs_err;


                                                                /* Send TYPE command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %c\r\n", FTPc_Cmd[FTP_CMD_TYPE].CmdStr, FTP_TYPE_IMAGE);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_OKAY) {
        return (DEF_FAIL);
    }


                                                                /* Send PASV command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s\r\n", FTPc_Cmd[FTP_CMD_PASV].CmdStr);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_ENTERPASVMODE) {
        return (DEF_FAIL);
    }

    server_ip    = 0;
    server_port  = 0;
    tmp_buf      = Str_Char(buf, '(');
    tmp_buf++;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Address MSB.                                  */
    tmp_buf++;
    server_ip   += tmp_val << 24;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);
    tmp_buf++;
    server_ip   += tmp_val << 16;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);
    tmp_buf++;
    server_ip   += tmp_val << 8;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Address LSB.                                  */
    tmp_buf++;
    server_ip   += tmp_val << 0;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Port MSB.                                     */
    tmp_buf++;
    server_port += tmp_val << 8;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Port LSB.                                     */
    tmp_buf++;
    server_port += tmp_val << 0;

                                                                /* Open data socket.                                    */
    FTPc_TRACE_INFO(("FTPc OPEN DTP socket.\n"));
    FTPc_DTP_SOCK_ID = NetSock_Open(NET_SOCK_ADDR_FAMILY_IP_V4,
                                    NET_SOCK_TYPE_STREAM,
                                    NET_SOCK_PROTOCOL_TCP,
                                    p_net_err);

    if (*p_net_err == NET_SOCK_ERR_NONE) {
         Mem_Set(&server_addr, (CPU_CHAR)0, sizeof(server_addr));
         p_server_addr         = (NET_SOCK_ADDR_IP *)&server_addr;
         p_server_addr->Family =  NET_SOCK_ADDR_FAMILY_IP_V4;
         p_server_addr->Addr   =  NET_UTIL_HOST_TO_NET_32(server_ip);
         p_server_addr->Port   =  NET_UTIL_HOST_TO_NET_16(server_port);

                                                                /* Set connect timemout.                                */
        NetOS_Sock_ConnReqTimeoutSet(FTPc_DTP_SOCK_ID, FTPc_CFG_DTP_MAX_CONN_TIMEOUT_S * DEF_TIME_NBR_mS_PER_SEC, p_net_err);

                                                                /* Connect data socket.                                 */
        FTPc_TRACE_INFO(("FTPc CONN DTP socket.\n"));
        NetSock_Conn(FTPc_DTP_SOCK_ID,
                     (NET_SOCK_ADDR *)&server_addr,
                     sizeof(NET_SOCK_ADDR),
                     p_net_err);
        if ((*p_net_err != NET_SOCK_ERR_NONE) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_EMPTY) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_CLOSED)) {
              FTPc_TRACE_DBG(("FTPc NetSock_Conn() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
              NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
              FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
              return (DEF_FAIL);
        }
        if ((*p_net_err == NET_SOCK_ERR_RX_Q_EMPTY) ||
            (*p_net_err == NET_SOCK_ERR_RX_Q_CLOSED)) {
             FTPc_TRACE_DBG(("FTPc NetSock_Conn() timeout, line #%d.\n", __LINE__));
             NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
             FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
             return (DEF_FAIL);
        }
    } else {
        FTPc_TRACE_DBG(("FTPc NetSock_Open() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

                                                                /* Send RETR command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %s\r\n", FTPc_Cmd[FTP_CMD_RETR].CmdStr, remote_file_name);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_OKAYOPENING) {
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

    p_file = FS_FOpen((char *)local_file_name, "wb");
    if (p_file == (FS_FILE *)0) {
        FTPc_TRACE_DBG(("FTPc FS_FOpen failed, line #%d.\n", __LINE__));
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

    NetOS_TCP_RxQ_TimeoutSet(FTPc_DTP_SOCK_ID, FTPc_CFG_DTP_MAX_CONN_TIMEOUT_S * DEF_TIME_NBR_mS_PER_SEC, p_net_err);

	rx_pkt_cnt = 0;
    while (DEF_TRUE) {
        buf_size = NetSock_RxData(FTPc_DTP_SOCK_ID, buf, sizeof(buf), NET_SOCK_FLAG_NONE, p_net_err);
        if ((*p_net_err != NET_SOCK_ERR_NONE) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_EMPTY) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_CLOSED)) {
              FTPc_TRACE_DBG(("FTPc NetSock_RxData() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
              break;
        }

        FTPc_TRACE_DBG(("FTPc RX DATA #%03d.\n", rx_pkt_cnt));

        /* In this case, a timeout represent an end-of-file     */
        /* condition.                                           */
        if ((*p_net_err == NET_SOCK_ERR_RX_Q_EMPTY) ||
            (*p_net_err == NET_SOCK_ERR_RX_Q_CLOSED)) {
             break;
        }

        fs_len = FS_FWrite(buf, 1, buf_size, p_file);
        fs_err = FS_FError(p_file);
        if (fs_len != buf_size) {
            if (fs_err == FS_ERR_DISKFULL) {
                FTPc_TRACE_DBG(("FTPc Disk Full, line #%d.\n", __LINE__));
            } else {
                FTPc_TRACE_DBG(("FTPc FS_FWrite() failed: error #%d, line #%d.\n", fs_err, __LINE__));
            }
            break;
        }
        rx_pkt_cnt++;
    }

    FS_FClose(p_file);

                                                                /* Close socket.                                        */
    NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
    FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_CLOSINGSUCCESS) {
        return (DEF_FAIL);
    }

    return (DEF_OK);
#else
    return (DEF_FAIL);
#endif
}


/*
*********************************************************************************************************
*                                            FTPc_SendFile()
*
* Description : Send a file located in the file system to an FTP server.
*
* Argument(s) : remote_file_name        name of the file in FTP server.
*               local_file_name         name of the file in filesystem.
*               append                  if DEF_TRUE, existing file on FTP server will be appended with
*                                           local file.  If file doesn't exist on FTP server, it will be
*                                           created.
*                                       if DEF_FALSE, existing file on FTP server will be overwritten.
*                                           If file doesn't exist on FTP server, it will be created.
*
* Return(s)   : DEF_FAIL        transmission failed.
*               DEF_OK          transmission successful.
*
* Caller(s)   : Application.
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPc_SendFile (CPU_CHAR     *remote_file_name,
                            CPU_CHAR     *local_file_name,
                            CPU_BOOLEAN   append,
                            NET_ERR      *p_net_err)
{
#if (FTPc_CFG_USE_FS > 0)
    NET_IP_ADDR        server_ip;
    CPU_INT16U         server_port;
    NET_SOCK_ADDR      server_addr;
    NET_SOCK_ADDR_IP  *p_server_addr;
    CPU_CHAR           buf[FTPc_NET_BUF_SIZE];
    CPU_INT32U         buf_size;

    CPU_INT32U         reply_code;
    CPU_BOOLEAN        rtn_code;

    CPU_CHAR          *tmp_buf;
    CPU_INT08U         tmp_val;
    CPU_INT32U         tx_pkt_cnt;

    FS_FILE           *p_file;
    CPU_INT32S         fs_len;
    CPU_INT32S         fs_err;


                                                                /* Send TYPE command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %c\r\n", FTPc_Cmd[FTP_CMD_TYPE].CmdStr, FTP_TYPE_IMAGE);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_OKAY) {
        return (DEF_FAIL);
    }

                                                                /* Send PASV command.                                   */
    buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s\r\n", FTPc_Cmd[FTP_CMD_PASV].CmdStr);
    rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
    FTPc_TRACE_INFO(("FTPc TX: %s", buf));
    if (rtn_code == DEF_FAIL) {
        return (DEF_FAIL);
    }

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_ENTERPASVMODE) {
        return (DEF_FAIL);
    }

    server_ip    = 0;
    server_port  = 0;
    tmp_buf      = Str_Char(buf, '(');
    tmp_buf++;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Address MSB.                                  */
    tmp_buf++;
    server_ip   += tmp_val << 24;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);
    tmp_buf++;
    server_ip   += tmp_val << 16;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);
    tmp_buf++;
    server_ip   += tmp_val << 8;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Address LSB.                                  */
    tmp_buf++;
    server_ip   += tmp_val << 0;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Port MSB.                                     */
    tmp_buf++;
    server_port += tmp_val << 8;

    tmp_val      = Str_ToLong(tmp_buf, &tmp_buf, 10);          /* Get IP Port LSB.                                     */
    tmp_buf++;
    server_port += tmp_val << 0;

                                                                /* Open data socket.                                    */
    FTPc_TRACE_INFO(("FTPc OPEN DTP socket.\n"));
    FTPc_DTP_SOCK_ID = NetSock_Open(NET_SOCK_ADDR_FAMILY_IP_V4,
                                    NET_SOCK_TYPE_STREAM,
                                    NET_SOCK_PROTOCOL_TCP,
                                    p_net_err);

    if (*p_net_err == NET_SOCK_ERR_NONE) {
         Mem_Set(&server_addr, (CPU_CHAR)0, sizeof(server_addr));
         p_server_addr         = (NET_SOCK_ADDR_IP *)&server_addr;
         p_server_addr->Family =  NET_SOCK_ADDR_FAMILY_IP_V4;
         p_server_addr->Addr   =  NET_UTIL_HOST_TO_NET_32(server_ip);
         p_server_addr->Port   =  NET_UTIL_HOST_TO_NET_16(server_port);

                                                                /* Set connect timemout.                                */
        NetOS_Sock_ConnReqTimeoutSet(FTPc_DTP_SOCK_ID, FTPc_CFG_DTP_MAX_CONN_TIMEOUT_S * DEF_TIME_NBR_mS_PER_SEC, p_net_err);

                                                                /* Connect data socket.                                 */
        FTPc_TRACE_INFO(("FTPc CONN DTP socket.\n"));
        NetSock_Conn( FTPc_DTP_SOCK_ID,
                     (NET_SOCK_ADDR *)&server_addr,
                      sizeof(NET_SOCK_ADDR),
                      p_net_err);
        if ((*p_net_err != NET_SOCK_ERR_NONE) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_EMPTY) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_CLOSED)) {
              FTPc_TRACE_DBG(("FTPc NetSock_Conn() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
              NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
              FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
              return (DEF_FAIL);
        }
        if ((*p_net_err == NET_SOCK_ERR_RX_Q_EMPTY) ||
            (*p_net_err == NET_SOCK_ERR_RX_Q_CLOSED)) {
             FTPc_TRACE_DBG(("FTPc NetSock_Conn() timeout, line #%d.\n", __LINE__));
             NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
             FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
             return (DEF_FAIL);
        }
    } else {
        FTPc_TRACE_DBG(("FTPc NetSock_Open() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

    if (append == DEF_YES) {
                                                                /* Send APPE command.                                   */
        buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %s\r\n", FTPc_Cmd[FTP_CMD_APPE].CmdStr, remote_file_name);
        rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
        FTPc_TRACE_INFO(("FTPc TX: %s", buf));
        if (rtn_code == DEF_FAIL) {
            NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
            FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
            return (DEF_FAIL);
        }
    } else {
                                                                /* Send STOR command.                                   */
        buf_size = Str_FmtPrint((char *)buf, FTPc_NET_BUF_SIZE, "%s %s\r\n", FTPc_Cmd[FTP_CMD_STOR].CmdStr, remote_file_name);
        rtn_code = FTPc_Tx(FTPc_CTRL_SOCK_ID, buf, buf_size, p_net_err);
        FTPc_TRACE_INFO(("FTPc TX: %s", buf));
        if (rtn_code == DEF_FAIL) {
            NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
            FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
            return (DEF_FAIL);
        }
    }
                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));
    if (reply_code != FTP_REPLY_CODE_OKAYOPENING) {
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

    p_file = FS_FOpen((char *)local_file_name, "rb");
    if (p_file == (FS_FILE *)0) {
        NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
        FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));
        return (DEF_FAIL);
    }

    tx_pkt_cnt = 0;
    while (DEF_TRUE) {
        fs_len = FS_FRead(buf, 1, sizeof(buf), p_file);
        fs_err = FS_FError(p_file);
        if (fs_len == 0) {
            if (fs_err != FS_ERR_EOF) {
                FTPc_TRACE_DBG(("FTPc FS_FRead() failed: error #%d, line #%d.\n", fs_err, __LINE__));
            }
            break;
        }

        FTPc_TRACE_DBG(("FTPc TX DATA #%03d... ", tx_pkt_cnt));
        rtn_code = FTPc_Tx(FTPc_DTP_SOCK_ID, buf, fs_len, p_net_err);
        if (rtn_code == DEF_FAIL) {
            FTPc_TRACE_DBG(("FTPc FTPc_Tx() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
            break;
        }

        FTPc_TRACE_DBG(("\n"));
        if (fs_len != sizeof(buf)) {
            break;
        }
        tx_pkt_cnt++;
    }
    FS_FClose(p_file);

                                                                /* Close socket.                                        */
    NetSock_Close(FTPc_DTP_SOCK_ID, p_net_err);
    FTPc_TRACE_INFO(("FTPc CLOSE DTP socket.\n"));

                                                                /* Receive status line.                                 */
    reply_code = FTPc_WaitForStatus(FTPc_CTRL_SOCK_ID, buf, FTPc_NET_BUF_SIZE, p_net_err);
    if (reply_code != FTP_REPLY_CODE_CLOSINGSUCCESS) {
        return (DEF_FAIL);
    }
    FTPc_TRACE_INFO(("FTPc RX: %s\n", buf));

    return (DEF_OK);
#else
    return (DEF_FAIL);
#endif
}


/*
*********************************************************************************************************
*                                         FTPc_WaitForStatus()
*
* Description : Receive FTP server reply after a request and analyse it to find the server status reply.
*
* Argument(s) : sock_id             TCP socket ID.
*               net_buf             buffer to receive reply.
*               net_buf_size        size of buffer to receive reply.
*               p_net_err           contains error message returned.
*
* Return(s)   : FTP server status code.
*
* Caller(s)   : Various.
*********************************************************************************************************
*/

static  CPU_INT32U  FTPc_WaitForStatus (CPU_INT32S   sock_id,
                                        CPU_CHAR    *net_buf,
                                        CPU_INT16U   net_buf_size,
                                        NET_ERR     *p_net_err)
{
    CPU_INT32U   rx_retry;
    CPU_INT32U   reply_code;


                                                                /* Receive status line.                                 */
    rx_retry = 0;
    do {
        FTPc_RxLine(sock_id, net_buf, net_buf_size, p_net_err);
        rx_retry++;
    } while ((*p_net_err == DEF_FAIL) &&
             ( rx_retry   < FTPc_CFG_CTRL_MAX_RX_RETRY));
    if (rx_retry == FTPc_CFG_CTRL_MAX_RX_RETRY) {
        return (DEF_FAIL);
    }

                                                                /* Check for expected status.                           */
    reply_code = Str_ToLong(net_buf, 0, 10);
    return (reply_code);
}


/*
*********************************************************************************************************
*                                             FTPc_RxLine()
*
* Description : Receive data until EOL character received ('\n') or buffer filled.
*
* Argument(s) : sock_id             TCP socket ID.
*               net_buf             buffer to hold received data.
*               net_buf_size        size of buffer to hold received data.
*               p_net_err           contains error message returned.
*
* Return(s)   : DEF_FAIL        reception failed.
*               DEF_OK          reception successful.
*
* Caller(s)   : Various.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  FTPc_RxLine (CPU_INT32S   sock_id,
                                  CPU_CHAR    *net_buf,
                                  CPU_INT16U   net_buf_size,
                                  NET_ERR     *p_net_err)
{
    CPU_CHAR    *net_buf2;
    CPU_INT32S   pkt_size;


    NetOS_TCP_RxQ_TimeoutSet(sock_id, FTPc_CFG_CTRL_MAX_RX_TIMEOUT_S * DEF_TIME_NBR_mS_PER_SEC, p_net_err);

    while (DEF_TRUE) {
        pkt_size = NetSock_RxData(sock_id,
                                  net_buf,
                                  net_buf_size,
                                  NET_SOCK_FLAG_NONE,
                                  p_net_err);
        if ((*p_net_err != NET_SOCK_ERR_NONE) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_EMPTY) &&
            (*p_net_err != NET_SOCK_ERR_RX_Q_CLOSED)) {
              FTPc_TRACE_DBG(("FTPc NetSock_RxData() failed: error #%d, line #%d.\n", *p_net_err, __LINE__));
              break;
        }
        if ((*p_net_err == NET_SOCK_ERR_RX_Q_EMPTY) ||
            (*p_net_err == NET_SOCK_ERR_RX_Q_CLOSED)) {
             FTPc_TRACE_DBG(("FTPc NetSock_RxData() timeout, line #%d.\n", __LINE__));
             break;
        }

        net_buf2 = (CPU_CHAR *)Str_Char_N(net_buf, pkt_size, '\n');
        if ( net_buf2     != (CPU_CHAR *)0) {
            *net_buf2      = (CPU_CHAR  )0;
             net_buf_size  = net_buf2 - net_buf;
             break;
        }

        net_buf_size -= pkt_size;
        net_buf      += pkt_size;
    }

    if (*p_net_err != NET_SOCK_ERR_NONE) {
         return (DEF_FAIL);
    }
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                               FTPc_Tx()
*
* Description : Transmit data to TCP socket, handling transient errors and incomplete buffer transmit.
*
* Argument(s) : sock_id             TCP socket ID.
*               net_buf             buffer to send.
*               net_buf_size        size of buffer to send.
*               p_net_err           contains error message returned.
*
* Return(s)   : DEF_FAIL        transmission failed.
*               DEF_OK          transmission successful.
*
* Caller(s)   : Various.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  FTPc_Tx (CPU_INT32S   sock_id,
                              CPU_CHAR    *net_buf,
                              CPU_INT16U   net_buf_len,
                              NET_ERR     *net_err)
{
    CPU_CHAR     *tx_buf;
    CPU_INT16S    tx_buf_len;
    CPU_INT16S    tx_len;
    CPU_INT16S    tx_len_tot;
    CPU_INT32U    tx_retry_cnt;
    CPU_BOOLEAN   tx_done;
    CPU_BOOLEAN   tx_dly;


    tx_len_tot   = 0;
    tx_retry_cnt = 0;
    tx_done      = DEF_NO;
    tx_dly       = DEF_NO;

    while ((tx_len_tot   <  net_buf_len)               &&       /* While tx tot len < buf len ...   */
           (tx_retry_cnt <  FTPc_CFG_DTP_MAX_TX_RETRY) &&       /* ... & tx retry   < MAX     ...   */
           (tx_done      == DEF_NO)) {                          /* ... & tx NOT done;         ...   */

        if (tx_dly == DEF_YES) {                                /* Dly tx, if req'd.                */
            FTPc_OS_TxDly();
        }

        tx_buf     = net_buf     + tx_len_tot;
        tx_buf_len = net_buf_len - tx_len_tot;
        tx_len     = NetSock_TxData(sock_id,                    /* ... tx data.                     */
                                    tx_buf,
                                    tx_buf_len,
                                    NET_SOCK_FLAG_NONE,
                                    net_err);
        switch (*net_err) {
            case NET_SOCK_ERR_NONE:
                 if (tx_len > 0) {                              /* If          tx len > 0, ...      */
                     tx_len_tot += tx_len;                      /* ... inc tot tx len.              */
                     tx_dly      = DEF_NO;
                 } else {                                       /* Else dly next tx.                */
                     tx_dly      = DEF_YES;
                 }
                 tx_retry_cnt = 0;
                 break;

            case NET_SOCK_ERR_NOT_USED:
            case NET_SOCK_ERR_INVALID_TYPE:
            case NET_SOCK_ERR_INVALID_FAMILY:
            case NET_SOCK_ERR_INVALID_STATE:
                 tx_done = DEF_YES;
                 break;

            case NET_ERR_TX:                                    /* If transitory tx err, ...    */
            default:
                 tx_dly = DEF_YES;                              /* ... dly next tx.             */
                 tx_retry_cnt++;
                 break;
        }
    }

    if (*net_err != NET_SOCK_ERR_NONE) {
        return (DEF_FAIL);
    }
    return (DEF_OK);
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
