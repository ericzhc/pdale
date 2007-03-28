/*
*********************************************************************************************************
*                                               uC/POP3c
*                                    Post Office Protocol 3 (client)
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
*                                              POP3 CLIENT
*
* Filename      : pop3-c.c
* Version       : V1.02
* Programmer(s) : SR
*                 JDH
*********************************************************************************************************
*
* This code implements a subset of the POP3 client protocol (RFC 1939).  More precisely, the following
* commands have been implemented:
*   USER
*   PASS
*   QUIT
*   STAT
*   LIST
*   RETR
*   DELE
*   NOOP
*   RSET
*
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define    POP3c_MODULE
#include  <lib_mem.h>
#include  <lib_str.h>
#include  <net.h>
#include  <pop3-c.h>


/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

static CPU_CHAR  POP3c_Resp_Buf[POP3c_RESP_BUF_LEN];


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static CPU_BOOLEAN   POP3c_ValidateResp     (CPU_CHAR     *resp);

static NET_ERR       POP3c_QueryServer      (NET_SOCK_ID   sock,
                                             CPU_CHAR     *query);

static CPU_CHAR     *POP3c_RespServerSingle (NET_SOCK_ID   sock,
                                             NET_ERR      *perr);

static CPU_INT32U    POP3c_RespServerMulti  (NET_SOCK_ID   sock,
                                             CPU_CHAR     *dest_buf,
                                             CPU_INT32U    buf_size,
                                             CPU_BOOLEAN   is_msg,
                                             NET_ERR      *perr);

static CPU_BOOLEAN   POP3c_GetLine          (CPU_CHAR     *buf,
                                             CPU_INT32U   *start,
                                             CPU_INT16U   *len,
                                             CPU_INT32U    buf_size);

static CPU_CHAR     *POP3c_USER             (NET_SOCK_ID   sock,
                                             CPU_CHAR     *username,
                                             NET_ERR      *perr);

static CPU_CHAR     *POP3c_PASS             (NET_SOCK_ID   sock,
                                             CPU_CHAR     *pswd,
                                             NET_ERR      *perr);

static CPU_CHAR     *POP3c_QUIT             (NET_SOCK_ID   sock,
                                             NET_ERR      *perr);

static CPU_CHAR     *POP3c_STAT             (NET_SOCK_ID   sock,
                                             NET_ERR      *perr);

static CPU_CHAR     *POP3c_LIST             (NET_SOCK_ID   sock,
                                             CPU_INT32U    msg_nbr,
                                             NET_ERR      *perr);

static CPU_INT32U    POP3c_RETR             (NET_SOCK_ID   sock,
                                             CPU_INT32U    msg_nbr,
                                             CPU_CHAR     *dest_buf,
                                             CPU_INT32U    buf_size,
                                             NET_ERR      *perr);

static CPU_CHAR     *POP3c_DELE             (NET_SOCK_ID   sock,
                                             CPU_INT32U    msg_nbr,
                                             NET_ERR      *perr);

#if 0                                                           /* Not yet used functions.                              */
static CPU_CHAR     *POP3c_NOOP             (NET_SOCK_ID   sock,
                                             NET_ERR      *perr);

static CPU_CHAR     *POP3c_RSET             (NET_SOCK_ID   sock,
                                             NET_ERR      *perr);
#endif


/*
*********************************************************************************************************
*                                            POP3c_Connect()
*
* Description : (1) Establish a TCP connection to the POP3 server.
*                   (a) Determine port
*                   (b) Open the socket
*                   (c) Establish a TCP connection
*                   (d) Receive server's response
*
* Argument(s) : ip_server       IP address of the POP3 server to contact.
*               port            TCP port to use.  If "0", POP3c_DFLT_PORT is used.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               POP3c_ERR_NONE                      No error, TCP connection established.
*                               POP3c_ERR_SOCK_OPEN_FAILED          Error opening socket.
*                               POP3c_ERR_SOCK_CONN_FAILED          Error connecting to server.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : Socket descriptor/handle identifier, if NO error.
*               -1,                                  Otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1)  If anything goes wrong while trying to connect to the server, the socket is
*                    closed by calling NetSock_Close.  Hence, all data structures are returned to
*                    their original state in case of a failure.
*********************************************************************************************************
*/

NET_SOCK_ID  POP3c_Connect (NET_IP_ADDR   ip_server,
                            CPU_INT16U    port,
                            NET_ERR      *perr)
{
    NET_SOCK_ID        sock;
    NET_SOCK_ADDR_LEN  len_addr_server;
    NET_SOCK_ADDR_IP   addr_server;
    CPU_INT16U         port_server;
    NET_SOCK_RTN_CODE  rtn_code;


   *perr = POP3c_ERR_NONE;

                                                                /* ------------------ DETERMINE PORT ------------------ */
    if (port != 0) {
        port_server = port;
    } else {
        port_server = POP3c_CFG_IPPORT;
    }

                                                                /* ------------------ OPEN THE SOCKET ----------------- */
    sock = NetSock_Open(NET_SOCK_ADDR_FAMILY_IP_V4,
                        NET_SOCK_TYPE_STREAM,
                        NET_SOCK_PROTOCOL_TCP,
                        perr);

    if (*perr != NET_SOCK_ERR_NONE) {
        *perr  = POP3c_ERR_SOCK_OPEN_FAILED;
        return (-1);
    }

                                                                /* ------------- ESTABLISH TCP CONNECTION ------------- */
    Mem_Set(&addr_server, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);
    addr_server.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
    addr_server.Addr   = NET_UTIL_HOST_TO_NET_32(ip_server);
    addr_server.Port   = NET_UTIL_HOST_TO_NET_16(port_server);
    len_addr_server    = sizeof(addr_server);


    rtn_code           = NetSock_Conn( sock,
                                      (NET_SOCK_ADDR *)&addr_server,
                                       len_addr_server,
                                       perr);

    if ( rtn_code != NET_SOCK_BSD_ERR_NONE) {
         NetSock_Close(sock, perr);
        *perr = POP3c_ERR_SOCK_CONN_FAILED;
         return (-1);
    }

                                                                /* ------------- RECEIVE SERVER'S RESPONSE ------------ */
    POP3c_RespServerSingle(sock, perr);

    if (*perr != POP3c_ERR_NONE) {
         NetSock_Close(sock, perr);
         return (-1);
    }

    return (sock);
}


/*
*********************************************************************************************************
*                                         POP3c_Authenticate()
*
* Description : (1) Log the user into the system.
*                   (a) Invoke USER command
*                   (b) Invoke PASS command
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               username        Username on the POP3 server, or (CPU_CHAR *)0 if not needed.
*               pswd            Password associated with the previous username, or (CPU_CHAR *)0 if not needed.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               POP3c_ERR_NONE                      No error, authentication successful.
*                               POP3c_ERR_ARG_TOO_LONG              See note #1
*
*                                                                   - RETURNED BY POP3c_USER() : -
*
*                                                                   - RETURNED BY POP3c_PASS() : -
*
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : void.
*
* Caller(s)   : Application.
*
* Note(s)     : (1)  As stated in RFC #1939, Section 'Basic Operation' "Each argument may be up
*                    to 40 characters long.
*
*               (2)  Passing a (CPU_CHAR *)0 argument for either the username of the password result in the
*                    associate command not to be send to the server.
*********************************************************************************************************
*/

void  POP3c_Authenticate (NET_SOCK_ID   sock,
                          CPU_CHAR     *username,
                          CPU_CHAR     *pswd,
                          NET_ERR      *perr)
{
   *perr = POP3c_ERR_NONE;

    if (username != (CPU_CHAR *)0) {
        if ( Str_Len(username) > POP3c_ARGUMENT_MAX_LEN) {      /* See Note #1                                          */
            *perr = POP3c_ERR_ARG_TOO_LONG;
             return;
        }
    }

    if (pswd != (CPU_CHAR *)0) {
        if ( Str_Len(pswd) > POP3c_ARGUMENT_MAX_LEN) {          /* See Note #1                                          */
            *perr = POP3c_ERR_ARG_TOO_LONG;
             return;
        }
    }

                                                                /* ---------------- INVOKE USER COMMAND --------------- */
    if (username != (CPU_CHAR *)0) {                            /* See Note #2                                          */
        POP3c_USER(sock, username, perr);
        if (*perr != POP3c_ERR_NONE) {
             return;
        }
    }

                                                                /* ---------------- INVOKE PASS COMMAND --------------- */
    if (pswd != (CPU_CHAR *)0) {                                /* See Note #2                                          */
        POP3c_PASS(sock, pswd, perr);
        if (*perr != POP3c_ERR_NONE) {
             return;
        }
    }
}


/*
*********************************************************************************************************
*                                           POP3c_MboxStat()
*
* Description : (1) Get number of message(s) in the mailbox, as well as its total size.
*                   (a) Invoke STAT command
*                   (c) Parse response
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               msg_qty         Number of messages on the server for this account.
*               mbox_size       Size of this mailbox (in bytes).
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_STAT() : -
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : void.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) If successful, the response for this command is in the format: "+OK nn mm" where
*                   "nn" correspond to the number of messages and "mm" is the total size of the maildrop
*                   in octets.
*
*               (2) The values returned via the pointers (number of message(s) and mailbox size) does
*                   not include message set for deletion, as stated in RFC #1939, Section 'The
*                   TRANSACTION State : STAT Command' "Note that messages marked as deleted are note
*                   counted in either total".
*********************************************************************************************************
*/

void  POP3c_MboxStat (NET_SOCK_ID   sock,
                      CPU_INT32U   *msg_qty,
                      CPU_INT32U   *mbox_size,
                      NET_ERR      *perr)
{
    CPU_CHAR  *resp;


   *perr = POP3c_ERR_NONE;

                                                                /* ---------------- INVOKE STAT COMMAND --------------- */
    resp = POP3c_STAT(sock, perr);
    if (*perr != POP3c_ERR_NONE) {
         return;
    }
                                                                /* ------------------ PARSE RESPONSE ------------------ */
                                                                /* See Notes #1 and #2                                  */
    Str_FmtScan((char const *)resp + 4, "%d %d", msg_qty, mbox_size);
}


/*
*********************************************************************************************************
*                                            POP3c_MsgStat()
*
* Description : (1) Get specific message size from the server.
*                   (a) Invoke LIST command
*                   (c) Parse response
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               msg_nbr         Index of message of interest.
*               msg_size        Size of the message (in bytes) returned by the server.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_LIST() : -
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : void.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) The msg_nbr argument starts at the value "1".  Message "0" does not exist.
*
*               (2) (a) Since this function doesn't allow the invocation of the LIST command without a
*                       message index, the response is guaranteed to be single-line.
*
*                   (b) If successful, the response for this command is in the format: "+OK nn mm" where
*                       "nn" correspond to the message number and "mm" is the total size of the message
*                       in octets.
*********************************************************************************************************
*/

void  POP3c_MsgStat (NET_SOCK_ID   sock,
                     CPU_INT32U    msg_nbr,
                     CPU_INT32U   *msg_size,
                     NET_ERR      *perr)
{
    CPU_CHAR   *resp;
    CPU_INT32U  msg_ix;


   *perr = POP3c_ERR_NONE;

                                                                /* ---------------- INVOKE LIST COMMAND --------------- */
    resp = POP3c_LIST(sock, msg_nbr, perr);
    if (*perr != POP3c_ERR_NONE) {
         return;
    }
                                                                /* ------------------ PARSE RESPONSE ------------------ */
                                                                /* See Notes #2                                         */
    Str_FmtScan((char const *)resp + 4, "%d %d", &msg_ix, msg_size);
}


/*
*********************************************************************************************************
*                                          POP3c_MsgRetrieve()
*
* Description : (1) Retrieve a specific message from the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               msg_nbr         Index of message of interest.
*               dest_buf        Pointer to allocated buffer used to copy the message.
*               buf_size        Size of dest_buf.
*               del_msg         Indicates if the message should be deleted from the server.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_DEST_BUF_TOO_SMALL        dest_buf too small.
*
*                                                                   - RETURNED BY POP3c_RETR() : -
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerMulti() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*                               POP3c_ERR_DEST_BUF_TOO_SMALL        dest_buf too small.
*
*                                                                   - RETURNED BY POP3c_DELE() : -
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
* Return(s)   : void.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) The msg_nbr argument starts at the value "1".  Message "0" does not exist.
*
*               (2) The client software is responsible for providing a large enough buffer in order to
*                   contain the whole message.  Failure to do so might lead to runtime problems.
*                   If the buffer passed is too small for a given message, only the first buf_size - 1
*                   bytes will be copied, followed by a '\0'.  See POP3c_RespServerMulti() for more
*                   information.
*********************************************************************************************************
*/

void  POP3c_MsgRetrieve (NET_SOCK_ID   sock,
                         CPU_INT32U    msg_nbr,
                         CPU_CHAR     *dest_buf,
                         CPU_INT32U    buf_size,
                         CPU_BOOLEAN   del_msg,
                         NET_ERR      *perr)
{
   *perr = POP3c_ERR_NONE;

    if ((dest_buf == (CPU_CHAR *)0) ||
        (buf_size == 0)) {
        *perr = POP3c_ERR_DEST_BUF_TOO_SMALL;
         return;
    }

    POP3c_RETR(sock, msg_nbr, dest_buf, buf_size, perr);

    if (del_msg == DEF_YES) {
        POP3c_MsgDel(sock, msg_nbr, perr);
    }
}


/*
*********************************************************************************************************
*                                            POP3c_MsgRead()
*
* Description : (1) Read and structure a message previously retrieved from a POP3 server.
*                   (a) Parse header and fill structure
*                   (b) Set message body pointer
*
* Argument(s) : msg_buf         Buffer containing a message received with POP3c_MsgRetrieve().
*               buf_size        Size of msg_buf.
*               msg             Pointer to POP3_MSG structure being filled by this function.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_INCOMPLETE_MSG            Incomplete message in msg_buf.
*
* Return(s)   : void.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) From RFC 2822 (Internet Message Format), Section 'Lexical Analysis of Messages :
*                   General Description', "The body is simply a sequence of characters that follows the
*                   header and is separated from the header by an empty line (i.i., a line with nothing
*                   preceding the CRLF)".  Hence, GetLine() will return a length of 2 when it
*                   encounters that delimitation.
*
*               (2) Characters <CRLF> are not copied into the various structure's header fields.
*                   Instead, a '\0' character is appended.
*
*               (3) The leading WSP (after the comma) is(are) not copied into the various structure's
*                   header fields.
*
*               (4) If the character '\0' is found in the msg_buf before the end of the header, it
*                   means that buffer, and hence the message, is incomplete.  However, there is no way
*                   to know if the message body was complete.  One should always check the return value
*                   of POP3c_MsgRetrieve() for confirmation.
*********************************************************************************************************
*/

void  POP3c_MsgRead (CPU_CHAR    *msg_buf,
                     CPU_INT32U   buf_size,
                     POP3c_MSG   *msg,
                     NET_ERR     *perr)
{
    CPU_INT32U    start;
    CPU_INT16U    len;
    CPU_INT32U    start_no_WSP;
    CPU_INT16U    fields_found;
    CPU_BOOLEAN   msg_found;

    CPU_BOOLEAN   next_line;
    CPU_INT16S    str_cmp;

    CPU_INT16U    field_value_len;
    CPU_CHAR      last_char;

    CPU_BOOLEAN   from_found;
    CPU_BOOLEAN   date_found;
    CPU_BOOLEAN   sender_found;
    CPU_BOOLEAN   replyto_found;
    CPU_BOOLEAN   subject_found;


    fields_found  = 0;
    msg_found     = DEF_NO;

    from_found    = DEF_NO;
    date_found    = DEF_NO;
    sender_found  = DEF_NO;
    replyto_found = DEF_NO;
    subject_found = DEF_NO;

    Mem_Set(msg, '\0', sizeof(POP3c_MSG));
    msg->Body = (CPU_CHAR *)0;
   *perr      = POP3c_ERR_NONE;

                                                                /* ------------------- PARSE HEADER ------------------- */
    start = 0;
    do {
        next_line = POP3c_GetLine(msg_buf, &start, &len, buf_size);
        if (next_line == DEF_TRUE) {
            if (len == 2) {                                     /* See note #1                                          */
                msg_found = DEF_YES;
            } else if (fields_found < POP3c_MSG_HDR_FIELDS_QTY) {
                start_no_WSP = start;                           /* Looking for various header fields                    */

                if (from_found == DEF_NO) {
                    str_cmp = Str_Cmp_N(POP3c_HDR_FROM, &msg_buf[start], POP3c_HDR_FROM_LEN);
                    if (str_cmp == 0) {
                                                                /* Removing header and CRLF                             */
                        field_value_len = len - POP3c_HDR_FROM_LEN - 2;

                                                                /* Removing leading WSP                                 */
                        while (msg_buf[start_no_WSP + POP3c_HDR_FROM_LEN] == POP3c_TAB ||
                               msg_buf[start_no_WSP + POP3c_HDR_FROM_LEN] == POP3c_SP  ) {
                            start_no_WSP++;
                            field_value_len--;
                        }

                        if (field_value_len >= POP3c_MSG_ADDR_SIZE) {
                            field_value_len  = POP3c_MSG_ADDR_SIZE - 1;
                        }

                                                                /* Ending field with '\0'                               */
                        last_char = msg_buf[start_no_WSP + POP3c_HDR_FROM_LEN + field_value_len];
                        msg_buf[start_no_WSP + POP3c_HDR_FROM_LEN + field_value_len] = '\0';

                        Str_Copy(msg->From, &msg_buf[start_no_WSP + POP3c_HDR_FROM_LEN]);
                        msg->From[field_value_len] = '\0';

                        msg_buf[start_no_WSP + POP3c_HDR_FROM_LEN + field_value_len] = last_char;

                        from_found = DEF_YES;
                        fields_found++;
                    }
                }

                if (date_found == DEF_NO) {
                    str_cmp = Str_Cmp_N(POP3c_HDR_DATE, &msg_buf[start], POP3c_HDR_DATE_LEN);
                    if (str_cmp == 0) {
                        field_value_len = len - POP3c_HDR_DATE_LEN - 2;

                                                                /* Removing leading WSP                                 */
                        while (msg_buf[start_no_WSP + POP3c_HDR_DATE_LEN] == POP3c_TAB ||
                               msg_buf[start_no_WSP + POP3c_HDR_DATE_LEN] == POP3c_SP  ) {
                            start_no_WSP++;
                            field_value_len--;
                        }

                        if (field_value_len >= POP3c_MSG_DATE_SIZE) {
                            field_value_len  = POP3c_MSG_DATE_SIZE - 1;
                        }

                                                                /* Ending field with '\0'                               */
                        last_char = msg_buf[start_no_WSP + POP3c_HDR_DATE_LEN + field_value_len];
                        msg_buf[start_no_WSP + POP3c_HDR_DATE_LEN + field_value_len] = '\0';

                        Str_Copy(msg->Date, &msg_buf[start_no_WSP + POP3c_HDR_DATE_LEN]);
                        msg->Date[field_value_len] = '\0';

                        msg_buf[start_no_WSP + POP3c_HDR_DATE_LEN + field_value_len] = last_char;

                        date_found = DEF_YES;
                        fields_found++;
                    }
                }

                if (sender_found == DEF_NO) {
                    str_cmp = Str_Cmp_N(POP3c_HDR_SENDER, &msg_buf[start], POP3c_HDR_SENDER_LEN);
                    if (str_cmp == 0) {
                        field_value_len = len - POP3c_HDR_SENDER_LEN - 2;

                                                                /* Removing leading WSP                                 */
                        while (msg_buf[start_no_WSP + POP3c_HDR_SENDER_LEN] == POP3c_TAB ||
                               msg_buf[start_no_WSP + POP3c_HDR_SENDER_LEN] == POP3c_SP  ) {
                            start_no_WSP++;
                            field_value_len--;
                        }

                        if (field_value_len >= POP3c_MSG_ADDR_SIZE) {
                            field_value_len  = POP3c_MSG_ADDR_SIZE - 1;
                        }

                                                                /* Ending field with '\0'                               */
                        last_char = msg_buf[start_no_WSP + POP3c_HDR_SENDER_LEN + field_value_len];
                        msg_buf[start_no_WSP + POP3c_HDR_SENDER_LEN + field_value_len] = '\0';

                        Str_Copy(msg->Sender, &msg_buf[start_no_WSP + POP3c_HDR_SENDER_LEN]);
                        msg->Sender[field_value_len] = '\0';

                        msg_buf[start_no_WSP + POP3c_HDR_SENDER_LEN + field_value_len] = last_char;

                        sender_found = DEF_YES;
                        fields_found++;
                    }
                }

                if (replyto_found == DEF_NO) {
                    str_cmp = Str_Cmp_N(POP3c_HDR_REPLYTO, &msg_buf[start], POP3c_HDR_REPLYTO_LEN);
                    if (str_cmp == 0) {
                        field_value_len = len - POP3c_HDR_REPLYTO_LEN - 2;

                                                               /* Removing leading WSP                                 */
                        while (msg_buf[start_no_WSP + POP3c_HDR_REPLYTO_LEN] == POP3c_TAB ||
                               msg_buf[start_no_WSP + POP3c_HDR_REPLYTO_LEN] == POP3c_SP  ) {
                            start_no_WSP++;
                            field_value_len--;
                        }

                        if (field_value_len >= POP3c_MSG_ADDR_SIZE) {
                            field_value_len  = POP3c_MSG_ADDR_SIZE - 1;
                        }

                                                                /* Ending field with '\0'                               */
                        last_char = msg_buf[start_no_WSP + POP3c_HDR_REPLYTO_LEN + field_value_len];
                        msg_buf[start_no_WSP + POP3c_HDR_REPLYTO_LEN + field_value_len] = '\0';

                        Str_Copy(msg->Reply_to, &msg_buf[start_no_WSP + POP3c_HDR_REPLYTO_LEN]);
                        msg->Reply_to[field_value_len] = '\0';

                        msg_buf[start_no_WSP + POP3c_HDR_REPLYTO_LEN + field_value_len] = last_char;

                        replyto_found = DEF_YES;
                        fields_found++;
                    }
                }

                if (subject_found == DEF_NO) {
                    str_cmp = Str_Cmp_N(POP3c_HDR_SUBJECT, &msg_buf[start], POP3c_HDR_SUBJECT_LEN);
                    if (str_cmp == 0) {
                        field_value_len = len - POP3c_HDR_SUBJECT_LEN - 2;

                                                               /* Removing leading WSP                                 */
                        while (msg_buf[start_no_WSP + POP3c_HDR_SUBJECT_LEN] == POP3c_TAB ||
                               msg_buf[start_no_WSP + POP3c_HDR_SUBJECT_LEN] == POP3c_SP  ) {
                            start_no_WSP++;
                            field_value_len--;
                        }

                        if (field_value_len >= POP3c_MSG_SUBJECT_SIZE) {
                            field_value_len  = POP3c_MSG_SUBJECT_SIZE - 1;
                        }

                                                                /* Ending field with '\0'                               */
                        last_char = msg_buf[start_no_WSP + POP3c_HDR_SUBJECT_LEN + field_value_len];
                        msg_buf[start_no_WSP + POP3c_HDR_SUBJECT_LEN + field_value_len] = '\0';

                        Str_Copy(msg->Subject, &msg_buf[start_no_WSP + POP3c_HDR_SUBJECT_LEN]);
                        msg->Subject[field_value_len] = '\0';

                        msg_buf[start_no_WSP + POP3c_HDR_SUBJECT_LEN + field_value_len] = last_char;

                        subject_found = DEF_YES;
                        fields_found++;
                    }
                }

            }
            start = start + len;
        } else {
            *perr = POP3c_ERR_INCOMPLETE_MSG;                   /* See note #4                                          */
             return;
        }
    } while (msg_found == DEF_NO);

                                                                /* ----------------- SET BODY POINTER ----------------- */
    next_line = POP3c_GetLine(msg_buf, &start, &len, buf_size);
    if (next_line ==  DEF_TRUE) {
        msg->Body  = &msg_buf[start];
    }
}


/*
*********************************************************************************************************
*                                            POP3c_MsgDel()
*
* Description : Delete a specific message from the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               msg_nbr         Index of message to delete.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_DELE() : -
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : void.
*
* Caller(s)   : Application.
*
* Note(s)     : The msg_nbr argument starts at the value "1".  Message "0" does not exist.
*********************************************************************************************************
*/
void  POP3c_MsgDel (NET_SOCK_ID   sock,
                    CPU_INT32U    msg_nbr,
                    NET_ERR      *perr)
{
    *perr = POP3c_ERR_NONE;

    POP3c_DELE(sock, msg_nbr, perr);
}


/*
*********************************************************************************************************
*                                          POP3c_Disconnect()
*
* Description : (1) Closes the connection between client and server.
*                   (a) Invoke QUIT command
*                   (b) Close socket
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               POP3c_ERR_NONE                      No error.
*
* Return(s)   : void.
*
* Caller(s)   : Application.
*
* Note(s)     : The possible error code returned by NetSock_Close is not taken into account.
*********************************************************************************************************
*/

void  POP3c_Disconnect (NET_SOCK_ID   sock,
                        NET_ERR      *perr)
{
    NET_ERR  err;

    POP3c_QUIT(sock, perr);
    NetSock_Close(sock, &err);
}


/*
*********************************************************************************************************
*                                         POP3c_ValidateResp()
*
* Description : (1) Validate server response.
*                   (a) Compare response status operator
*
* Argument(s) : response        Pointer to response returned by server.
*
* Return(s)   : DEF_YES,        if positive response;
*               DEF_NO,         otherwise.
*
* Caller(s)   : POP3c_Connect
*               POP3c_Authenticate
*               POP3c_MboxStat
*               POP3c_MsgStat
*               POP3c_RETR
*               POP3c_RETR.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  POP3c_ValidateResp (CPU_CHAR  *resp)
{
    CPU_INT16S str_cmp;


    str_cmp = Str_Cmp_N(resp, POP3c_POS_STATUS_INDICATOR, POP3c_LEN_POS_STAT_INDICATOR);
    if (str_cmp == 0) {
        return (DEF_YES);
    }

    return (DEF_NO);
}


/*
*********************************************************************************************************
*                                             POP3c_USER()
*
* Description : Build the USER command and send it to the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               username        Username on the POP3 server
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : Single-line response from the server.
*
* Caller(s)   : POP3c_Authenticate.
*
* Note(s)     : (1) A command consists of 4 characters and the termination characters (CRLF).  For every
*                   argument, an additional character is needed (space).
*
*                   As stated in RFC #1939, Section 'Basic Operation', "Each argument may be up to 40
*                   characters long".
*********************************************************************************************************
*/

CPU_CHAR  *POP3c_USER (NET_SOCK_ID   sock,
                       CPU_CHAR     *username,
                       NET_ERR      *perr)
{
                                                                /* See note #1.                                         */
    CPU_CHAR   query[POP3c_COMMAND_MIN_LEN +
                     1 * (POP3c_ARGUMENT_MAX_LEN + POP3c_ARGUMENT_ADDITIONNAL_LEN) + 1];
    CPU_CHAR  *resp;


    Str_FmtPrint((char *)query,
                  POP3c_COMMAND_MIN_LEN +
                  1 * (POP3c_ARGUMENT_MAX_LEN + POP3c_ARGUMENT_ADDITIONNAL_LEN) + 1,
                 "USER %s%c%c",
                  username,
                  POP3c_CR, POP3c_LF);

   *perr = POP3c_QueryServer(sock, query);
    if (*perr != POP3c_ERR_NONE) {
         return ((CPU_CHAR *)0);
    }

    resp = POP3c_RespServerSingle(sock, perr);

    return (resp);
}


/*
*********************************************************************************************************
*                                           POP3c_PASS()
*
* Description : Build the PASS command and send it to the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               pswd            Password on the POP3 server (clear text)
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : Single-line response from the server.
*
* Caller(s)   : POP3c_Authenticate.
*
* Note(s)     : (1) A command consists of 4 characters and the termination characters (CRLF).  For every
*                   argument, an additional character is needed (space).
*
*                   As stated in RFC #1939, Section 'Basic Operation', "Each argument may be up to 40
*                   characters long".
*********************************************************************************************************
*/

CPU_CHAR  *POP3c_PASS (NET_SOCK_ID   sock,
                       CPU_CHAR     *pswd,
                       NET_ERR      *perr)
{
                                                                /* See note #1.                                         */
    CPU_CHAR   query[POP3c_COMMAND_MIN_LEN +
                     1 * (POP3c_ARGUMENT_MAX_LEN + POP3c_ARGUMENT_ADDITIONNAL_LEN) + 1];
    CPU_CHAR  *resp;


    Str_FmtPrint((char *)query,
                  POP3c_COMMAND_MIN_LEN +
                  1 * (POP3c_ARGUMENT_MAX_LEN + POP3c_ARGUMENT_ADDITIONNAL_LEN) + 1,
                 "PASS %s%c%c",
                  pswd,
                  POP3c_CR, POP3c_LF);

   *perr = POP3c_QueryServer(sock, query);
    if (*perr != POP3c_ERR_NONE) {
         return ((CPU_CHAR *)0);
    }

    resp = POP3c_RespServerSingle(sock, perr);

    return (resp);
}


/*
*********************************************************************************************************
*                                             POP3c_QUIT()
*
* Description : Build the QUIT command and send it to the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : Single-line response from the server.
*
* Caller(s)   : POP3c_Disconnect.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_CHAR  *POP3c_QUIT (NET_SOCK_ID   sock,
                       NET_ERR      *perr)
{
    CPU_CHAR   query[POP3c_COMMAND_MIN_LEN + 1];
    CPU_CHAR  *resp;


    Str_FmtPrint((char *)query,
                  POP3c_COMMAND_MIN_LEN + 1,
                 "QUIT%c%c",
                  POP3c_CR,
                  POP3c_LF);

   *perr = POP3c_QueryServer(sock, query);
    if (*perr != POP3c_ERR_NONE) {
         return ((CPU_CHAR *)0);
    }

    resp = POP3c_RespServerSingle(sock, perr);

    return (resp);
}


/*
*********************************************************************************************************
*                                             POP3c_STAT()
*
* Description : Build the STAT command and send it to the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : Single-line response from the server.
*
* Caller(s)   : POP3c_MboxStat.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_CHAR  *POP3c_STAT (NET_SOCK_ID   sock,
                       NET_ERR      *perr)
{
    CPU_CHAR   query[POP3c_COMMAND_MIN_LEN + 1];
    CPU_CHAR  *resp;


    Str_FmtPrint((char *)query,
                  POP3c_COMMAND_MIN_LEN + 1,
                 "STAT%c%c",
                  POP3c_CR,
                  POP3c_LF);

   *perr = POP3c_QueryServer(sock, query);
    if (*perr != POP3c_ERR_NONE) {
         return ((CPU_CHAR *)0);
    }

    resp = POP3c_RespServerSingle(sock, perr);

    return (resp);
}


/*
*********************************************************************************************************
*                                             POP3c_LIST()
*
* Description : Build the LIST command and send it to the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               msg_nbr         Index of message of interest.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : Single-line response from the server.
*
* Caller(s)   : POP3c_MsgStat.
*
* Note(s)     : (1) A command consists of 4 characters and the termination characters (CRLF).  For every
*                   argument, an additional character is needed (space).
*
*                   As stated in RFC #1939, Section 'Basic Operation', "Each argument may be up to 40
*                   characters long".
*********************************************************************************************************
*/

CPU_CHAR  *POP3c_LIST (NET_SOCK_ID   sock,
                       CPU_INT32U    msg_nbr,
                       NET_ERR      *perr)
{
                                                                /* See note #1.                                         */
    CPU_CHAR   query[POP3c_COMMAND_MIN_LEN +
                     1 * (POP3c_ARGUMENT_MAX_LEN + POP3c_ARGUMENT_ADDITIONNAL_LEN) + 1];
    CPU_CHAR  *resp;


    Str_FmtPrint((char *)query,
                  POP3c_COMMAND_MIN_LEN +
                  1 * (POP3c_ARGUMENT_MAX_LEN + POP3c_ARGUMENT_ADDITIONNAL_LEN) + 1,
                 "LIST %d%c%c",
                  msg_nbr,
                  POP3c_CR,
                  POP3c_LF);

   *perr = POP3c_QueryServer(sock, query);
    if (*perr != POP3c_ERR_NONE) {
         return ((CPU_CHAR *)0);
    }

    resp = POP3c_RespServerSingle(sock, perr);

    return (resp);
}


/*
*********************************************************************************************************
*                                             POP3c_RETR()
*
* Description : Build the RETR command and send it to the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               msg_nbr         Index of message of interest.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerMulti() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : The number of bytes written in dest_buf, excluding the final '\0'.
*
* Caller(s)   : POP3c_MsgRetrieve.
*
* Note(s)     : (1) A command consists of 4 characters and the termination characters (CRLF).  For every
*                   argument, an additional character is needed (space).
*
*                   As stated in RFC #1939, Section 'Basic Operation', "Each argument may be up to 40
*                   characters long".
*********************************************************************************************************
*/

static CPU_INT32U  POP3c_RETR  (NET_SOCK_ID   sock,
                                CPU_INT32U    msg_nbr,
                                CPU_CHAR     *dest_buf,
                                CPU_INT32U    buf_size,
                                NET_ERR      *perr)
{
                                                                /* See note #1.                                         */
    CPU_CHAR   query[POP3c_COMMAND_MIN_LEN +
                     1 * (POP3c_ARGUMENT_MAX_LEN + POP3c_ARGUMENT_ADDITIONNAL_LEN) + 1];
    CPU_INT32U resp;

                                                                /* Transmit query to the server.                        */
    Str_FmtPrint((char *)query,
                  POP3c_COMMAND_MIN_LEN +
                  1 * (POP3c_ARGUMENT_MAX_LEN + POP3c_ARGUMENT_ADDITIONNAL_LEN) + 1,
                 "RETR %d%c%c",
                  msg_nbr,
                  POP3c_CR,
                  POP3c_LF);

   *perr = POP3c_QueryServer(sock, query);
    if (*perr != POP3c_ERR_NONE) {
         return (0);
    }

    resp = POP3c_RespServerMulti(sock, dest_buf, buf_size, DEF_YES, perr);

    return (resp);
}


/*
*********************************************************************************************************
*                                             POP3c_DELE()
*
* Description : Build the DELE command and send it to the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               msg_nbr         Index of message of to delete.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : Single-line response from the server.
*
* Caller(s)   : POP3c_MsgRetrieve.
*
* Note(s)     : (1) A command consists of 4 characters and the termination characters (CRLF).  For every
*                   argument, an additional character is needed (space).
*
*                   As stated in RFC #1939, Section 'Basic Operation', "Each argument may be up to 40
*                   characters long".
*********************************************************************************************************
*/

CPU_CHAR  *POP3c_DELE (NET_SOCK_ID   sock,
                       CPU_INT32U    msg_nbr,
                       NET_ERR      *perr)
{
                                                                /* See note #1.                                         */
    CPU_CHAR   query[POP3c_COMMAND_MIN_LEN +
                     1 * (POP3c_ARGUMENT_MAX_LEN + POP3c_ARGUMENT_ADDITIONNAL_LEN) + 1];
    CPU_CHAR  *resp;


    Str_FmtPrint((char *)query,
                  POP3c_COMMAND_MIN_LEN +
                  1 * (POP3c_ARGUMENT_MAX_LEN + POP3c_ARGUMENT_ADDITIONNAL_LEN) + 1,
                 "DELE %d%c%c",
                  msg_nbr,
                  POP3c_CR,
                  POP3c_LF);

   *perr = POP3c_QueryServer(sock, query);
    if (*perr != POP3c_ERR_NONE) {
         return ((CPU_CHAR *)0);
    }

    resp = POP3c_RespServerSingle(sock, perr);

    return (resp);
}


#if 0
/*
*********************************************************************************************************
*                                             POP3c_NOOP()
*
* Description : Build the NOOP command and send it to the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : Single-line response from the server.
*
* Caller(s)   : .
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_CHAR  *POP3c_NOOP (NET_SOCK_ID   sock,
                       NET_ERR      *perr)
{
    CPU_CHAR   query[POP3c_COMMAND_MIN_LEN + 1];
    CPU_CHAR  *resp;


    Str_FmtPrint((char *)query,
                  POP3c_COMMAND_MIN_LEN + 1,
                 "NOOP%c%c",
                  POP3c_CR,
                  POP3c_LF);

   *perr = POP3c_QueryServer(sock, query);
    if (*perr != POP3c_ERR_NONE) {
         return ((CPU_CHAR *)0);
    }

    resp = POP3c_RespServerSingle(sock, perr);

    return (resp);
}


/*
*********************************************************************************************************
*                                             POP3c_RSET()
*
* Description : Build the RSET command and send it to the server.
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                                                                   - RETURNED BY POP3c_QueryServer() : -
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY POP3c_RespServerSingle() : -
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : Single-line response from the server.
*
* Caller(s)   : .
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_CHAR  *POP3c_RSET (NET_SOCK_ID   sock,
                       NET_ERR      *perr)
{
    CPU_CHAR   query[POP3c_COMMAND_MIN_LEN + 1];
    CPU_CHAR  *resp;


    Str_FmtPrint((char *)query,
                  POP3c_COMMAND_MIN_LEN + 1,
                 "RSET%c%c",
                  POP3c_CR,
                  POP3c_LF);

   *perr = POP3c_QueryServer(sock, query);
    if (*perr != POP3c_ERR_NONE) {
         return ((CPU_CHAR *)0);
    }

    resp = POP3c_RespServerSingle(sock, perr);

    return (resp);
}
#endif


/*
*********************************************************************************************************
*                                             POP3c_QueryServer()
*
* Description : (1) Send the query to the server.
*                   (a) Transmit query
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               query           Query in question.
*
* Return(s)   : Error code:
*                               POP3c_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY NetSock_TxData() : -
*                                                                   See uC/TCPIP source code.
*
* Caller(s)   : POP3c_USER
*               POP3c_PASS
*               POP3c_QUIT
*               POP3c_STAT
*               POP3c_LIST
*               POP3c_RETR
*               POP3c_DELE
*               POP3c_NOOP
*               POP3c_RSET
*
* Note(s)     : .
*********************************************************************************************************
*/

NET_ERR  POP3c_QueryServer (NET_SOCK_ID   sock,
                            CPU_CHAR     *query)
{
    NET_SOCK_RTN_CODE  rtn_code;
    NET_ERR            err;
    CPU_INT32U         len;
    CPU_INT32U         cur_pos;

                                                                /* ------------------- TRANSMIT QUERY ----------------- */
    len     = Str_Len(query);
    cur_pos = 0;

    do {
        rtn_code = NetSock_TxData( sock,
                                  &query[cur_pos],
                                   len,
                                   0,
                                  &err);

        cur_pos += rtn_code;
        len     -= rtn_code;

    } while ((len != 0) &&
             (rtn_code != NET_SOCK_BSD_ERR_TX));

    if (rtn_code != NET_SOCK_BSD_ERR_TX) {
        err = POP3c_ERR_NONE;
    }

    return (err);
}


/*
*********************************************************************************************************
*                                       POP3c_RespServerSingle()
*
* Description : (1) Receive the single-line reply from the POP3 server.
*                   (a) Receive response
*                   (b) Append termination character '\0'
*                   (c) Validate response
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*
* Return(s)   : Single-line response from the server.
*
* Caller(s)   : POP3c_Connect
*               POP3c_USER
*               POP3c_PASS
*               POP3c_QUIT
*               POP3c_STAT
*               POP3c_LIST
*               POP3c_DELE
*               POP3c_NOOP
*               POP3c_RSET
*
* Note(s)     : (1) Due to potential receiving problems, NetSock_RxData is called over and over again
*                   until success, or after 15 tries.
*
*               (2) This function may be used for single-line response commands only.
*
*               (3) The response from the server is copied into the file scope buffer
*                   POP3c_Resp_Buf.
*********************************************************************************************************
*/

CPU_CHAR  *POP3c_RespServerSingle (NET_SOCK_ID   sock,
                                   NET_ERR      *perr)
{
    NET_SOCK_RTN_CODE  rtn_code;
    CPU_BOOLEAN        is_valid;
    CPU_INT08U         tries;
                                                                /* ----------------- RECEIVE RESPONSE ----------------- */
                                                                /* See note #1                                          */
    tries = 0;
    do {
        rtn_code = NetSock_RxData(sock,
                                  POP3c_Resp_Buf,
                                  POP3c_RESP_BUF_LEN - 1,
                                  NET_SOCK_FLAG_NONE,
				                  perr);
        tries++;
    } while ((rtn_code == NET_SOCK_BSD_ERR_RX) &&
             (tries < POP3c_CFG_MAX_RX_RETRY));

    if (rtn_code == NET_SOCK_BSD_ERR_RX) {
        *perr = POP3c_ERR_RX_FAILED;
         return ((CPU_CHAR *)0);
    }

    POP3c_Resp_Buf[rtn_code] = '\0';

                                                                /* ---------------- VALIDATE RESPONSE ----------------- */
    is_valid = DEF_NO;
    if (rtn_code >= POP3c_LEN_POS_STAT_INDICATOR) {
        is_valid  = POP3c_ValidateResp(POP3c_Resp_Buf);
    }

    if (is_valid == DEF_NO) {
        *perr = POP3c_ERR_NEG_RESP;
         return ((CPU_CHAR *)0);
    }

   *perr = POP3c_ERR_NONE;
    return (POP3c_Resp_Buf);
}


/*
*********************************************************************************************************
*                                        POP3c_RespServerMulti()
*
* Description : (1) Receive the multi-line reply from the POP3 server.
*                   (a) Receive first packet of response
*                   (b) Validate response
*                   (c) Receive rest of response via the finite state machine
*
* Argument(s) : sock            Socket ID returned from POP3c_Connect.
*               dest_buf        Pre-allocated buffer receiving response.
*               buf_size        Size of buffer dest_buf.
*               is_msg          Boolean indicating whether this multi-line response is a message
*                               or not (more elaborate processing is needed if so, see note #4).
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               POP3c_ERR_NONE                      No error.
*                               POP3c_ERR_NEG_RESP                  Negative response from server (-ERR).
*                               POP3c_ERR_RX_FAILED                 Error receiving data from socket
*                                                                   connection.
*                               POP3c_ERR_DEST_BUF_TOO_SMALL        dest_buf too small.
*
* Return(s)   : The number of bytes written in dest_buf, excluding the final '\0'.
*
* Caller(s)   : POP3c_RETR
*
* Note(s)     : (1) This function should be used for response from multi-line commands only.
*
*               (2) The response from the server is copied into the file scope buffer
*                   POP3c_Resp_Buf for treatment before going into dest_buf.
*
*               (3) If the destination buffer passed in argument is too small to contain the whole
*                   message, perr is set to POP3c_ERR_DEST_BUF_TOO_SMALL and the number of bytes
*                   written to it is returned.  The buffer is '\0' terminated.
*
*               (4) Finite State Machine
*
*                   The state machine used to receive and treat multi-line responses contains 7 states
*                   which are going to be explained here.
*
*                   (a) STATUS_PROCESSING state
*
*                       First state in the machine where the status response from the server is treated.
*                       The response has already been validated, but it is not part of the response.
*
*                   (b) HDR_PROCESSING stage
*
*                       Optional state present when messages are received.  Header fields "unfolding" is
*                       performed here.  Note that data are treated just like they would be in the
*                       DATA_PROCESSING state.
*
*                   (c) DATA_PROCESSING state
*
*                       In this state, characters are simply copied from POP3c_Resp_Buf to dest_buf.
*
*                   (d) CR_FOUND        state
*                       CRLF_FOUND      state
*                       CRLFCF_FOUND    state
*                       CRLFDOT_FOUND   state
*                       CRLFDOTCR_FOUND state
*
*                       These states deal with the presence of special characters, such as CR (carriage
*                       return), LF (line feed) and the '.' character.  See note 4 for more details.
*
*               (5) From RFC #2822 (Internet Message Format), Section 'Lexical Analysis of Messages,
*                   Header Fields, Long Header Fields', "[...] To deal with the 998/78 character
*                   limitations per line, the field body portion of a header field can be split into
*                   multiple line representation;  this is called "folding".  Unfolding is accomplished
*                   by simply removing any CRLF that is immediately followed by WSP.  Each header field
*                   should be treated in its unfolded form for further syntactic and semantic evaluation".
*
*                   The process on "unfolding" long header fields is hence brought forward in here.
*
*               (6) According to RFC 1939 Section 'Basic Operation', "When all lines of the response have
*                   been sent, a final line is sent, consisting of a termination octet (decimal code 046,
*                   ".") and a CRLF pair.  if any line of the multi-line response begins with the
*                   termination octet, the line is "byte-stuffed" by pre-pending the termination octet to
*                   that line of the response.  When examining a multi-line response, the client checks
*                   to see if the line begins with the termination octet"
*
*                   (a) "If so and if CRLF immediately follows the termination character, then the
*                       response from the POP server is ended and the line containing ".CRLF" is not
*                       considered part of the multi-line response."
*
*                   (b) "If so and if octets other than CRLF follow, the first octet of the line (the
*                       termination octet) is stripped away".
*********************************************************************************************************
*/

CPU_INT32U  POP3c_RespServerMulti (NET_SOCK_ID   sock,
                                   CPU_CHAR     *dest_buf,
                                   CPU_INT32U    buf_size,
                                   CPU_BOOLEAN   is_msg,
                                   NET_ERR      *perr)
{
    CPU_INT16U          state;
    CPU_INT16U          last_state;
    CPU_INT16U          ix_rd_rb;
    CPU_INT16U          ix_last_rb;
    CPU_INT32U          ix_wr_db;
    NET_SOCK_RTN_CODE   rtn_code;
    CPU_BOOLEAN         msg_copied;
    CPU_BOOLEAN         is_valid;
    NET_ERR             err;


    if ((dest_buf == (CPU_CHAR *)0) ||
        (buf_size == 0)) {                                      /* See note #3                                          */
         dest_buf[0]  = '\0';
        *perr         = POP3c_ERR_DEST_BUF_TOO_SMALL;
         return (0);
    }

                                                                /* --------- RECEIVE FIRST PACKET OF RESPONSE --------- */
    rtn_code = NetSock_RxData( sock,
                               POP3c_Resp_Buf,
                               POP3c_RESP_BUF_LEN,
                               NET_SOCK_FLAG_NONE,
				              &err);

    if ((rtn_code == NET_SOCK_BSD_ERR_RX) ||
        (rtn_code  < POP3c_LEN_POS_STAT_INDICATOR)) {
         dest_buf[0] = '\0';
        *perr        = POP3c_ERR_RX_FAILED;
         return (0);
    }

                                                                /* ---------------- VALIDATE RESPONSE ----------------- */
    is_valid = POP3c_ValidateResp(POP3c_Resp_Buf);
    if ( is_valid == DEF_NO) {
         dest_buf[0] = '\0';
        *perr        = POP3c_ERR_NEG_RESP;
         return (0);
    }

                                                                /* ------------- BEGINNING OF STATE MACHINE ----------- */
    msg_copied = DEF_NO;
    state      = POP3c_STATUS_PROCESSING;
    ix_wr_db   = 0;
    ix_rd_rb   = 0;
    ix_last_rb = rtn_code - 1;

    do {
                                                                /* See note #3                                          */
        if ((*perr == POP3c_ERR_NONE) &&
            (ix_wr_db == buf_size - 1)) {
             dest_buf[ix_wr_db] = '\0';
            *perr = POP3c_ERR_DEST_BUF_TOO_SMALL;
        }

        if (ix_rd_rb > ix_last_rb) {                            /* Temporary buffer empty, receiving more data          */
            rtn_code = NetSock_RxData( sock,
                                       POP3c_Resp_Buf,
                                       POP3c_RESP_BUF_LEN,
                                       NET_SOCK_FLAG_NONE,
			                          &err);

            if ( rtn_code == NET_SOCK_BSD_ERR_RX) {
                 dest_buf[ix_wr_db] = '\0';
                *perr = POP3c_ERR_RX_FAILED;
                 return (0);
            }

            ix_rd_rb = 0;
            ix_last_rb = rtn_code - 1;
        }

        switch (state) {
            case POP3c_STATUS_PROCESSING:                             /* See note #4 (a)                                      */
                 if (POP3c_Resp_Buf[ix_rd_rb] == POP3c_CR) {
                     last_state = state;
                     state      = POP3c_CR_FOUND;
                 }
                 break;

            case POP3c_HDR_PROCESSING:                                /* See note #4 (b)                                      */

            case POP3c_DATA_PROCESSING:                               /* See note #4 (c)                                      */
                 if (POP3c_Resp_Buf[ix_rd_rb] == POP3c_CR) {
                     last_state = state;
                     state      = POP3c_CR_FOUND;
                 }

                 if (*perr == POP3c_ERR_NONE) {
                      dest_buf[ix_wr_db++] = POP3c_Resp_Buf[ix_rd_rb];
                 }
                 break;

             case POP3c_CR_FOUND:                                      /* See note #4 (d)                                      */
                 if (POP3c_Resp_Buf[ix_rd_rb] == POP3c_LF) {
                     state = POP3c_CRLF_FOUND;
                 } else {
                     state = last_state;
                 }

                 if (last_state != POP3c_STATUS_PROCESSING) {
                     if (*perr == POP3c_ERR_NONE) {
                          dest_buf[ix_wr_db++] = POP3c_Resp_Buf[ix_rd_rb];
                     }
                 }
                 break;

            case POP3c_CRLF_FOUND:                                    /* See note #4 (d)                                      */
                 if (POP3c_Resp_Buf[ix_rd_rb] == '.') {
                     state = POP3c_CRLFDOT_FOUND;
                 } else {
                     if (POP3c_Resp_Buf[ix_rd_rb] == POP3c_CR) {
                         state = POP3c_CRLFCR_FOUND;
                     } else if ((*perr == POP3c_ERR_NONE) &&
                                ( last_state == POP3c_HDR_PROCESSING) &&
                                 ((POP3c_Resp_Buf[ix_rd_rb] == POP3c_TAB) ||
                                  (POP3c_Resp_Buf[ix_rd_rb] == POP3c_SP))) {
                         ix_wr_db = ix_wr_db - 2;
                         state = POP3c_HDR_PROCESSING;
                     } else if (last_state == POP3c_STATUS_PROCESSING) {
                         if (is_msg) {
                             state = POP3c_HDR_PROCESSING;
                         } else {
                             state = POP3c_DATA_PROCESSING;
                         }
                     } else {
                         state = last_state;
                     }

                     if (*perr == POP3c_ERR_NONE) {
                          dest_buf[ix_wr_db++] = POP3c_Resp_Buf[ix_rd_rb];
                     }
                 }
                 break;

            case POP3c_CRLFCR_FOUND:                                  /* See note #4 (d)                                      */
                 if (POP3c_Resp_Buf[ix_rd_rb] == POP3c_LF) {
                     last_state = POP3c_DATA_PROCESSING;
                     state = POP3c_CRLF_FOUND;
                 } else {
                     state = last_state;
                 }

                 if (*perr == POP3c_ERR_NONE) {
                      dest_buf[ix_wr_db++] = POP3c_Resp_Buf[ix_rd_rb];
                 }
                 break;

            case POP3c_CRLFDOT_FOUND:                                 /* See note #4 (d)                                      */
                 if (POP3c_Resp_Buf[ix_rd_rb] == POP3c_CR) {
                     state = POP3c_CRLFDOTCR_FOUND;
                 } else {
                     state = last_state;
                     if (*perr == POP3c_ERR_NONE) {
                          dest_buf[ix_wr_db++] = POP3c_Resp_Buf[ix_rd_rb];
                     }
                 }
                 break;

            case POP3c_CRLFDOTCR_FOUND:                               /* See note #4 (d)                                      */
                 if (POP3c_Resp_Buf[ix_rd_rb] == POP3c_LF) {
                     msg_copied = DEF_YES;
                     if (*perr == POP3c_ERR_NONE) {
                          dest_buf[ix_wr_db] = '\0';
                     }
                 } else {
                     if (POP3c_Resp_Buf[ix_rd_rb] == POP3c_CR) {
                         state = POP3c_CR_FOUND;
                     } else {
                         state = last_state;
                     }

                     if (*perr == POP3c_ERR_NONE) {
                          dest_buf[ix_wr_db++] = POP3c_CR;
                          dest_buf[ix_wr_db++] = POP3c_Resp_Buf[ix_rd_rb];
                     }
                 }
                 break;

            default:
                 break;
        }

        ix_rd_rb++;

    } while (msg_copied == DEF_NO);

    return (ix_wr_db);
}


/*
*********************************************************************************************************
*                                            POP3c_GetLine()
*
* Description : (1) Point to the next line in the buffer
*                   (a) Search for sequence <CRLF>
*                   (b) Count the number of character on the line
*
* Argument(s) : buf             Buffer containing the multi-line message received with POP3c_RETR().
*               start           Index of pointing somewhere in current line.
*               len             Value set by function to indicate the number of valid character on the
*                               line, including <CRLF> ('\x0D\x0A'), but excluding '\0' (if any).
*               buf_size        Size of buf.
*
* Return(s)   : DEF_TRUE        If line is terminated by <CRLF> (complete line)
*               DEF_FALSE       Otherwise ('\0' encountered, or buf_size reached)
*
* Caller(s)   : POP3c_MsgRead
*
* Note(s)     : (1) The buffer passed in argument is terminated by '\0'.
*********************************************************************************************************
*/

CPU_BOOLEAN  POP3c_GetLine (CPU_CHAR    *buf,
                            CPU_INT32U  *start,
                            CPU_INT16U  *len,
                            CPU_INT32U   buf_size)
{
    CPU_BOOLEAN  ret_val;
    CPU_INT32U   ix;


    ret_val  =  DEF_FALSE;
    ix       = *start;
   *len      =  0;
                                                                /* ----------------- SEARCH FOR <CRLF> ---------------- */
    while (ix < buf_size) {
        if (buf[ix] == POP3c_CR) {
            if ((ix + 1) < buf_size) {
                if (buf[ix + 1] == POP3c_LF) {
                    ix      =  ix  + 2;
                   *len     = *len + 2;
                    ret_val = DEF_TRUE;
                    break;
                }
            }
        } else if (buf[ix] == '\0') {
            ret_val = DEF_FALSE;
            break;
        }
        ix++;
       *len = *len + 1;
    }

    return (ret_val);
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
