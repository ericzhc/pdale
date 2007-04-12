/*
*********************************************************************************************************
*                                               uC/SMTPc
*                                Simple Mail Transfert Protocol (client)
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
*                                              SMTP CLIENT
*
* Filename      : smtp-c.c
* Version       : V1.02
* Programmer(s) : SR
*                 JDH
*********************************************************************************************************
*
* This code implements a subset of the SMTP protocol (RFC 2821).  More precisely, the following
* commands have been implemented:
*   HELO
*   MAIL
*   RCPT
*   DATA
*   RSET
*   QUIT
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define    SMTPc_MODULE
#include  <app_cfg.h>
#include  <net.h>
#include  <smtp-c.h>


/*
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*/

static  CPU_CHAR  SMTPc_Comm_Buf[SMTPc_COMM_BUF_LEN];


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static CPU_CHAR    *SMTPc_RxReply     (NET_SOCK_ID   sock,
                                       NET_ERR      *perr);

static void         SMTPc_ParseReply  (CPU_CHAR     *server_reply,
                                       CPU_INT32U   *completion_code,
                                       NET_ERR      *perr);

static void         SMTPc_QueryServer (NET_SOCK_ID   sock,
                                       CPU_CHAR     *query,
                                       CPU_INT32U    len,
                                       NET_ERR      *perr);

static void         SMTPc_SendBody    (NET_SOCK_ID   sock,
                                       SMTPc_MSG    *msg,
                                       NET_ERR      *perr);

static CPU_INT32U   SMTPc_BuildHdr    (NET_SOCK_ID   sock,
                                       CPU_CHAR     *buf,
                                       CPU_INT32U    buf_size,
                                       CPU_INT32U    buf_wr_ix,
                                       CPU_CHAR     *hdr,
                                       CPU_CHAR     *val,
                                       CPU_INT32U   *line_len,
                                       NET_ERR      *perr);

static CPU_CHAR    *SMTPc_HELO        (NET_SOCK_ID   sock,
                                       NET_IP_ADDR   client_addr,
                                       CPU_INT32U   *completion_code,
                                       NET_ERR      *perr);

static CPU_CHAR    *SMTPc_MAIL        (NET_SOCK_ID   sock,
                                       CPU_CHAR     *from,
                                       CPU_INT32U   *completion_code,
                                       NET_ERR      *perr);

static CPU_CHAR    *SMTPc_RCPT        (NET_SOCK_ID   sock,
                                       CPU_CHAR     *to,
                                       CPU_INT32U   *completion_code,
                                       NET_ERR      *perr);

static CPU_CHAR    *SMTPc_DATA        (NET_SOCK_ID   sock,
                                       CPU_INT32U   *completion_code,
                                       NET_ERR      *perr);

static CPU_CHAR    *SMTPc_RSET        (NET_SOCK_ID   sock,
                                       CPU_INT32U   *completion_code,
                                       NET_ERR      *perr);

static CPU_CHAR    *SMTPc_QUIT        (NET_SOCK_ID   sock,
                                       CPU_INT32U   *completion_code,
                                       NET_ERR      *perr);


/*
*********************************************************************************************************
*                                            SMTPc_Connect()
*
* Description : (1) Establish a TCP connection to the SMTP server and initiate the SMTP session.
*                   (a) Determine port
*                   (b) Open the socket
*                   (c) Establish a TCP connection
*                   (d) Receive server's reply & validate
*                   (e) Initiate SMTP session
*
* Argument(s) : ip_server       IP address of the SMTP server to contact.
*               port            TCP port to use.  If "0", SMTPc_DFLT_PORT is used.
*               client_addr     Address literal helping identifying the client system.
*               init_extended   Whether or not to attempt an extended session initialization (EHLO)
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error, TCP connection established.
*                               SMTPc_ERR_SOCK_OPEN_FAILED          Error opening socket.
*                               SMTPc_ERR_SOCK_CONN_FAILED          Error connecting to server.
*                               SMTPc_ERR_RX_FAILED                 Error receiving server reply.
*                               SMTPc_ERR_REP                       Error with reply.
*
* Return(s)   : Socket descriptor/handle identifier, if NO error.
*               -1,                                  otherwise.
*
* Caller(s)   : Application
*
* Note(s)     : (1) If anything goes wrong while trying to connect to the server, the socket is
*                   closed by calling NetSock_Close.  Hence, all data structures are returned to
*                   their original state in case of a failure to establish the TCP connection.
*
*                   If the failure occurs when initiating the session, the application is responsible
*                   of the appropriate action(s) to be taken.
*
*               (2) The server will send a 220 "Service ready" reply when the connection is completed.
*                   The SMTP protocol allows a server to formally reject a transaction while still
*                   allowing the initial connection by responding with a 554 "Transaction failed"
*                   reply.
*
*               (3) In the current implementation, the extended session initialization (using EHLO) is
*                   not supported.  The session is hence established using HELO independently of the
*                   init_extended argument value.
*********************************************************************************************************
*/

NET_SOCK_ID  SMTPc_Connect (NET_IP_ADDR   ip_server,
                            CPU_INT16U    port,
                            NET_IP_ADDR   client_addr,
                            CPU_BOOLEAN   init_extended,
                            NET_ERR      *perr)
{
    NET_SOCK_ID         sock;
    CPU_INT16U          port_server;
    NET_SOCK_ADDR_IP    addr_server;
    NET_SOCK_ADDR_LEN   len_addr_server;
    NET_SOCK_RTN_CODE   rtn_code;
    CPU_INT32U          completion_code;
    CPU_CHAR           *reply;
    NET_ERR             err;


                                                                /* ------------------ DETERMINE PORT ------------------ */
    if (port != 0) {
        port_server = port;
    } else {
        port_server = SMTPc_CFG_IPPORT;
    }

                                                                /* ------------------ OPEN THE SOCKET ----------------- */
    sock = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                         NET_SOCK_TYPE_STREAM,
                         NET_SOCK_PROTOCOL_TCP,
                        &err);

    if ( err  != NET_SOCK_ERR_NONE) {
        *perr  = SMTPc_ERR_SOCK_OPEN_FAILED;
        return (-1);
    }

                                                                /* ------------- ESTABLISH TCP CONNECTION ------------- */
    Mem_Set(&addr_server, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);
    addr_server.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
    addr_server.Addr   = NET_UTIL_HOST_TO_NET_32(ip_server);
    addr_server.Port   = NET_UTIL_HOST_TO_NET_16(port_server);
    len_addr_server    = sizeof(addr_server);

    rtn_code = NetSock_Conn( sock,
                            (NET_SOCK_ADDR *)&addr_server,
                             len_addr_server,
                            &err);

    if ( rtn_code != NET_SOCK_BSD_ERR_NONE) {
         NetSock_Close(sock, &err);
        *perr = SMTPc_ERR_SOCK_CONN_FAILED;
         return (-1);
    }

                                                                /* ------- RECEIVE SERVER'S RESPONSE & VALIDATE ------- */
    reply = SMTPc_RxReply(sock, perr);                          /* See note #2                                          */
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_RX_FAILED;
         return (-1);
    }

    SMTPc_ParseReply(reply, &completion_code, perr);            /* See note #3                                          */
    if (*perr == SMTPc_ERR_REP_TOO_SHORT) {
        *perr  = SMTPc_ERR_REP;
         return (-1);
    }

    if ( completion_code == SMTPc_REP_250) {
        *perr = SMTPc_ERR_NONE;
    } else if (*perr == SMTPc_ERR_REP_POS) {                    /* Reply other than "250"                               */
        *perr = SMTPc_ERR_NONE;                                 /*     ... but accept any positive reply                */
    } else {
        *perr = SMTPc_ERR_REP;
    }

    if (*perr != SMTPc_ERR_NONE) {
         SMTPc_Disconnect(sock, &err);
         return (-1);
    }

                                                                /* -------------- INITIATE SMTP SESSION --------------- */
                                                                /* See note #3                                          */
    reply = SMTPc_HELO(sock, client_addr, &completion_code, perr);
    if (*perr != SMTPc_ERR_NONE) {
         SMTPc_Disconnect(sock, &err);
         return (-1);
    }

    return (sock);
}


/*
*********************************************************************************************************
*                                            SMTPc_SendMsg()
*
* Description : (1) Send a message (an instance of the SMTPc_MSG structure) to the SMTP server.
*                   (a) Invoke the MAIL command
*                   (b) Invoke the RCPT command for every recipient
*                   (c) Invoke the DATA command
*                   (d) Build and send the actual data
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               msg             SMTPc_MSG structure encapsulating the message to send.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_NULL_ARG                  Mandatory argument(s) missing.
*                               SMTPc_ERR_RX_FAILED                 Error receiving server reply.
*                               SMTPc_ERR_REP                       Error with reply.
*
*                                                                   - RETURNED BY SMTPc_MAIL() : -
*                                                                   - RETURNED BY SMTPc_RCPT() : -
*                                                                   - RETURNED BY SMTPc_DATA() : -
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.
*                               SMTPc_ERR_RX_FAILED                 Error receiving server reply.
*                               SMTPc_ERR_REP                       Error with reply.
*
*                                                                   - RETURNED BY SMTPc_SendBody() : -
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.
*                               SMTPc_ERR_LINE_TOO_LONG             Line limit exceeded.
*
* Return(s)   : void
*
* Caller(s)   : Application
*
* Note(s)     : (1) The function SMTPcADT_InitSMTPcMsg has to be called before being able to send a
*                   message.
*
*               (2) The message has to have at least one receiver, either "To", "CC", or "BCC".
*********************************************************************************************************
*/

void  SMTPc_SendMsg (NET_SOCK_ID   sock,
                     SMTPc_MSG    *msg,
                     NET_ERR      *perr)
{
    CPU_INT08U  i;
    CPU_INT32U  completion_code;
    NET_ERR     err;


                                                                /* See note #1                                          */
    if ( msg->From == (SMTPc_MBOX *)0) {
         SMTPc_TRACE_DBG(("Error SMTPc_SendMsg.  NULL from parameter\n\r"));
        *perr = SMTPc_ERR_NULL_ARG;
         return;
    }

                                                                /* See note #2                                          */
    if ((msg->ToArray[0]  == (SMTPc_MBOX *)0) &&
        (msg->CCArray[0]  == (SMTPc_MBOX *)0) &&
        (msg->BCCArray[0] == (SMTPc_MBOX *)0)) {
         SMTPc_TRACE_DBG(("Error SMTPc_SendMsg.  NULL parameter(s)\n\r"));
        *perr = SMTPc_ERR_NULL_ARG;
         return;
    }

                                                                /* ------------- INVOKE THE MAIL COMMAND -------------- */
    SMTPc_MAIL(sock, msg->From->Addr, &completion_code, perr);
    if (*perr != SMTPc_ERR_NONE) {
         SMTPc_TRACE_DBG(("Error MAIL.  Code: %d\n\r", completion_code));
         SMTPc_RSET(sock, &completion_code, &err);
         return;
    }

                                                                /* ------------- INVOKE THE RCTP COMMAND -------------- */
                                                                /* The RCPT command is issued for every recipient,      */
                                                                /* including CCs and BCCs                               */
    for (i = 0; i < SMTPc_CFG_MSG_MAX_TO; i++) {
        if (msg->ToArray[i] == (SMTPc_MBOX *)0) {
            break;
        }

        SMTPc_RCPT(sock, msg->ToArray[i]->Addr, &completion_code, perr);
        if (*perr != SMTPc_ERR_NONE) {
             SMTPc_TRACE_DBG(("Error RCPT (TO %d).  Code: %d\n\r", i, completion_code));
             SMTPc_RSET(sock, &completion_code, &err);          /* RSET message if invalid RCPT fails                   */
             return;
        }
    }

                                                                /* CCs                                                  */
    for (i = 0; i < SMTPc_CFG_MSG_MAX_CC; i++) {
        if (msg->CCArray[i] == (SMTPc_MBOX *)0) {
            break;
        }
        SMTPc_RCPT(sock, msg->CCArray[i]->Addr, &completion_code, perr);
        if (*perr != SMTPc_ERR_NONE) {
             SMTPc_TRACE_DBG(("Error RCPT (CC %d).  Code: %d\n\r", i, completion_code));
             SMTPc_RSET(sock, &completion_code, &err);          /* RSET message if invalid RCPT fails                   */
             return;
        }
    }

                                                                /* BCCs                                                 */
    for (i = 0; i < SMTPc_CFG_MSG_MAX_BCC; i++) {
        if (msg->BCCArray[i] == (SMTPc_MBOX *)0) {
            break;
        }

        SMTPc_RCPT(sock, msg->BCCArray[i]->Addr, &completion_code, perr);
        if (*perr != SMTPc_ERR_NONE) {
             SMTPc_TRACE_DBG(("Error RCPT (BCC %d).  Code: %d\n\r", i, completion_code));
             SMTPc_RSET(sock, &completion_code, &err);          /* RSET message if invalid RCPT fails                   */
             return;
        }
    }

                                                                /* ------------- INVOKE THE DATA COMMAND -------------- */
    SMTPc_DATA(sock, &completion_code, perr);
    if (*perr != SMTPc_ERR_NONE) {
         SMTPc_TRACE_DBG(("Error DATA.  Code: %d\n\r", completion_code));
         SMTPc_RSET(sock, &completion_code, &err);
         return;
    }

                                                                /* -------- BUILD AND SEND THE ACTUAL MESSAGE --------- */
    SMTPc_SendBody(sock, msg, perr);
    if (*perr != SMTPc_ERR_NONE) {
         SMTPc_TRACE_DBG(("Error SMTPc_SendBody.  Error: %d\n\r", *perr));
         SMTPc_RSET(sock, &completion_code, &err);
    }
}


/*
*********************************************************************************************************
*                                          SMTPc_Disconnect()
*
* Description : (1) Close the connection between client and server.
*                   (a) Send QUIT command
*                   (b) Close socket
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*
* Return(s)   : void
*
* Caller(s)   : Application
*               SMTPc_Connect
*
* Note(s)     : (1) The receiver (client) MUST NOT intentionally close the transmission channel until
*                   it receives and replies to a QUIT command.
*
*               (2) The receiver of the QUIT command MUST send an OK reply, and then close the
*                   transmission channel.
*********************************************************************************************************
*/

void  SMTPc_Disconnect (NET_SOCK_ID   sock,
                        NET_ERR      *perr)
{
    CPU_INT32U  completion_code;


    SMTPc_QUIT(sock, &completion_code, perr);

    SMTPc_RxReply(sock, perr);
    NetSock_Close(sock, perr);

   *perr = SMTPc_ERR_NONE;
}


/*
*********************************************************************************************************
*                                            SMTPc_RxReply()
*
* Description : (1) Receive and process reply from the SMTP server.
*                   (a) Receive reply
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_RX_FAILED                 Error receiving the reply.
*
* Return(s)   : Complete reply from the server, if NO reception error.
*
*               (CPU_CHAR *)0,                  otherwise.
*
* Caller(s)   : SMTPc_Connect
*               SMTPc_SendMsg
*               SMTPc_HELO
*               SMTPc_MAIL
*               SMTPc_RCPT
*               SMTPc_DATA
*               SMTPc_RSET
*               SMTPc_QUIT
*
* Note(s)     : (1) Server reply is at least 3 characters long (3 digits), plus CRLF.  Hence, receiving
*                   less than that automatically indicates an error.
*********************************************************************************************************
*/

CPU_CHAR  *SMTPc_RxReply (NET_SOCK_ID   sock,
                          NET_ERR      *perr)
{
    NET_SOCK_RTN_CODE  rtn_code;


                                                                /* ------------------- RECEIVE REPLY ------------------ */
    rtn_code = NetSock_RxData(sock,
                              SMTPc_Comm_Buf,
                              SMTPc_COMM_BUF_LEN - 1,
                              NET_SOCK_FLAG_NONE,
                              perr);

    if ((rtn_code == NET_SOCK_BSD_ERR_RX) ||
        (rtn_code <= 4)) {                                      /* See note #1                                          */
        *perr = SMTPc_ERR_RX_FAILED;
         return ((CPU_CHAR *)0);
    }

     SMTPc_Comm_Buf[rtn_code] = '\0';

    *perr = SMTPc_ERR_NONE;
     return (SMTPc_Comm_Buf);
}


/*
*********************************************************************************************************
*                                          SMTPc_ParseReply()
*
* Description : (1) Process reply received from the SMTP server.
*                   (a) Interpret reply
*
* Argument(s) : server_reply    Complete reply received from the server
*               completion_code Numeric value returned by server indication success of failure.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_REP_TOO_SHORT             Reply not long enough.
*                               SMTPc_ERR_REP_POS                   No error, positive reply received.
*                               SMTPc_ERR_REP_INTER                 No error, intermediate reply received.
*                               SMTPc_ERR_REP_NEG                   Negative reply received.
*
* Return(s)   : void
*
* Caller(s)   : SMTPc_Connect
*               SMTPc_SendMsg
*               SMTPc_HELO
*               SMTPc_MAIL
*               SMTPc_RCPT
*               SMTPc_DATA
*               SMTPc_RSET
*               SMTPc_QUIT
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  SMTPc_ParseReply (CPU_CHAR    *server_reply,
                        CPU_INT32U  *completion_code,
                        NET_ERR     *perr)
{
    CPU_INT08U  code_first_dig;
    CPU_INT08U  len;

                                                                /* -------------------- PARSE REPLY  ------------------ */
    len = Str_Len(server_reply);                                /* Make sure string is at least 3 + 1 character long    */
    if ( len < 4) {
        *perr = SMTPc_ERR_REP_TOO_SHORT;
         return;
    }

    Str_FmtScan((char const *)server_reply, "%d", completion_code);
    SMTPc_TRACE_DBG(("Code: %d\n\r", *completion_code));

                                                                /* ------------------ INTERPRET REPLY ----------------- */
    code_first_dig = *completion_code / 100;
    switch (code_first_dig) {
        case  SMTPc_REP_POS_COMPLET_GRP:                        /* Positive reply                                       */
             *perr = SMTPc_ERR_REP_POS;
              break;

        case  SMTPc_REP_POS_PRELIM_GRP:                         /* Intermediate reply                                   */
        case  SMTPc_REP_POS_INTER_GRP:
             *perr = SMTPc_ERR_REP_INTER;
              break;

        case  SMTPc_REP_NEG_TRANS_COMPLET_GRP:                  /* Negative reply                                       */
        case  SMTPc_REP_NEG_COMPLET_GRP:
             *perr = SMTPc_ERR_REP_NEG;
              break;

        default:                                                /* Should never happen, interpreted as negative         */
             *perr = SMTPc_ERR_REP_NEG;
              break;
    }
}


/*
*********************************************************************************************************
*                                           SMTPc_QueryServer()
*
* Description : (1) Send a query (or any thing else in fact) to the server.
*                   (a) Transmit query
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               query           Query in question.
*               len             Length of message to transmit
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*
*                                                                   - RETURNED BY NetSock_TxData() : -
*                                                                   See uC/TCPIP source code.
*
* Return(s)   : void
*
* Caller(s)   : SMTPc_SendBody
*               SMTPc_BuildHdr
*               SMTPc_HELO
*               SMTPc_MAIL
*               SMTPc_RCPT
*               SMTPc_DATA
*               SMTPc_RSET
*               SMTPc_QUIT
*
* Note(s)     :
*********************************************************************************************************
*/

void  SMTPc_QueryServer (NET_SOCK_ID   sock,
                         CPU_CHAR     *query,
                         CPU_INT32U    len,
                         NET_ERR      *perr)
{
    NET_SOCK_RTN_CODE  rtn_code;
    CPU_INT32U         cur_pos;

                                                                /* ------------------- TRANSMIT QUERY ----------------- */
    cur_pos = 0;

    do {
        rtn_code = NetSock_TxData( sock,
                                  &query[cur_pos],
                                   len,
                                   0,
                                   perr);

        cur_pos  = cur_pos + rtn_code;
        len      = len - rtn_code;

    } while ((len != 0) &&
             (rtn_code != NET_SOCK_BSD_ERR_TX));

    if (rtn_code != NET_SOCK_BSD_ERR_TX) {
        *perr = SMTPc_ERR_NONE;
    }
}


/*
*********************************************************************************************************
*                                           SMTPc_SendBody()
*
* Description : (1) Prepare and send the actual data of the message, i.e. the body part of the message
*                   content.
*                   (a) Built headers and transmit
*                   (b) Transmit body content
*                   (c) Prepare and transmit attachment(s)
*                   (d) Transmit "end of mail data" indicator
*                   (e) Receive the confirmation reply
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               msg             SMTPc_MSG structure encapsulating the message to send.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.

*                                                                   - RETURNED BY SMTPc_BuildHdr() : -
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_LINE_TOO_LONG             Line limit exceeded.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.
*
* Return(s)   : void
*
* Caller(s)   : SMTPc_SendMsg
*
* Note(s)     : (1) The current implementation does not insert the names of the mailbox owners (member
*                   NameDisp of structure SMTPc_MBOX).
*********************************************************************************************************
*/

void  SMTPc_SendBody (NET_SOCK_ID   sock,
                      SMTPc_MSG    *msg,
                      NET_ERR      *perr)
{
    CPU_SIZE_T   len;
    CPU_INT32U   cur_wr_ix;
    CPU_INT08U   i;
    CPU_INT32U   line_len;
    CPU_CHAR    *hdr;
    CPU_CHAR    *reply;
    CPU_INT32U   completion_code;


    cur_wr_ix = 0;
    line_len  = 0;

                                                                /* ------------------- BUILT HEADERS ------------------ */
                                                                /* Header "From: "                                      */
    hdr       = SMTPc_HDR_FROM;
    cur_wr_ix = SMTPc_BuildHdr( sock,                           /*   Address                                            */
                                SMTPc_Comm_Buf,
                                SMTPc_COMM_BUF_LEN,
                                cur_wr_ix,
                                hdr,
                                msg->From->Addr,
                               &line_len,
                                perr);
    if (*perr != SMTPc_ERR_NONE) {
         return;
    }


    if (msg->Sender != (SMTPc_MBOX *)0) {                       /* Header "Sender: "                                    */
        hdr       = SMTPc_HDR_SENDER;
        cur_wr_ix = SMTPc_BuildHdr( sock,                       /*   Address                                            */
                                    SMTPc_Comm_Buf,
                                    SMTPc_COMM_BUF_LEN,
                                    cur_wr_ix,
                                    hdr,
                                    msg->Sender->Addr,
                                   &line_len,
                                    perr);
        if (*perr != SMTPc_ERR_NONE) {
             return;
        }
    }

                                                                /* Header "To: "                                        */
    hdr = SMTPc_HDR_TO;
    for (i = 0; (i < SMTPc_CFG_MSG_MAX_TO) && (msg->ToArray[i] != (SMTPc_MBOX *)0); i++) {
        cur_wr_ix = SMTPc_BuildHdr( sock,
                                    SMTPc_Comm_Buf,
                                    SMTPc_COMM_BUF_LEN,
                                    cur_wr_ix,
                                    hdr,
                                    msg->ToArray[i]->Addr,
                                   &line_len,
                                    perr);
        if (*perr != SMTPc_ERR_NONE) {
             return;
        }
        hdr = (CPU_CHAR *)0;
    }


                                                                /* Header "Reply-to: "                                  */
    hdr = SMTPc_HDR_REPLYTO;
    if (msg->ReplyTo != (SMTPc_MBOX *)0) {
        cur_wr_ix = SMTPc_BuildHdr( sock,
                                    SMTPc_Comm_Buf,
                                    SMTPc_COMM_BUF_LEN,
                                    cur_wr_ix,
                                    hdr,
                                    msg->ReplyTo->Addr,
                                   &line_len,
                                    perr);
        if (*perr != SMTPc_ERR_NONE) {
             return;
        }
        hdr = (CPU_CHAR *)0;
    }


                                                                /* Header "CC: "                                        */
    hdr = SMTPc_HDR_CC;
    for (i = 0; (i < SMTPc_CFG_MSG_MAX_CC) && (msg->CCArray[i] != (SMTPc_MBOX *)0); i++) {
        cur_wr_ix = SMTPc_BuildHdr( sock,
                                    SMTPc_Comm_Buf,
                                    SMTPc_COMM_BUF_LEN,
                                    cur_wr_ix,
                                    hdr,
                                    msg->CCArray[i]->Addr,
                                   &line_len,
                                    perr);
        if (*perr != SMTPc_ERR_NONE) {
             return;
        }
        hdr = (CPU_CHAR *)0;
    }


    if (msg->Subject != (CPU_CHAR *)0) {                        /* Header "Subject: "                                   */
        cur_wr_ix = SMTPc_BuildHdr( sock,
                                    SMTPc_Comm_Buf,
                                    SMTPc_COMM_BUF_LEN,
                                    cur_wr_ix,
                                    SMTPc_HDR_SUBJECT,
                                    msg->Subject,
                                   &line_len,
                                    perr);
        if (*perr != SMTPc_ERR_NONE) {
             return;
        }
    }

                                                                /* ------------- TRANSMIT CONTENT HEADERS ------------- */
    SMTPc_QueryServer(sock, SMTPc_Comm_Buf, cur_wr_ix, perr);
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_TX_FAILED;
        return;
    }


                                                                /* --------------- TRANSMIT BODY CONTENT -------------- */
    SMTPc_QueryServer(sock, msg->ContentBodyMsg, msg->ContentBodyMsgLen, perr);
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_TX_FAILED;
        return;
    }
                                                                /* ----------- PREPARE AND TRANSMIT ATTACHMENT(S) ----- */
                                                                /* Not currently implemented                            */



                                                                /* --------- TRANSMIT END OF MAIL DATA INDICATOR ------ */
    Str_FmtPrint((char *)SMTPc_Comm_Buf,
                  SMTPc_COMM_BUF_LEN,
                 "%s",
                  SMTPc_EOM);

    len = Str_Len(SMTPc_Comm_Buf);
    SMTPc_QueryServer(sock, SMTPc_Comm_Buf, len, perr);
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_TX_FAILED;
        return;
    }

                                                                /* ----------- RECEIVE CONFIRMATION REPLY ------------- */
    reply = SMTPc_RxReply(sock, perr);
    if (*perr != SMTPc_ERR_NONE) {
         SMTPc_RSET(sock, &completion_code, perr);
        *perr  = SMTPc_ERR_RX_FAILED;
        return;
    }

    SMTPc_ParseReply(reply, &completion_code, perr);
    if (*perr == SMTPc_ERR_REP_TOO_SHORT) {
         SMTPc_RSET(sock, &completion_code, perr);
        *perr  = SMTPc_ERR_REP;
        return;
    }

    if (completion_code == SMTPc_REP_250) {
        *perr = SMTPc_ERR_NONE;
    } else {
         SMTPc_RSET(sock, &completion_code, perr);
        *perr = SMTPc_ERR_REP;
    }
}


/*
*********************************************************************************************************
*                                           SMTPc_BuildHdr()
*
* Description : (1) Prepare (and send if necessary) the message content's headers.
*                   (a) Calculate needed space
*                   (b) Send data, if necessary
*                   (c) Build header
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               buf             Buffer used to store the headers prior to their expedition.
*               buf_size        Size of buffer.
*               buf_wr_ix       Index of current "write" position.
*               hdr             Header name.
*               val             Value associated with header.
*               line_len        Current line total length.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_LINE_TOO_LONG             Line limit exceeded.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.
*
* Return(s)   : "Write" position in buffer.
*
* Caller(s)   : SMTPc_SendBody
*
* Note(s)     : (1) If the parameter "hdr" is (CPU_CHAR *)0, et means that it's already been passed in a
*                   previous call.  Hence, a "," will be inserted in the buffer prior to the value.
*
*               (2) If the SMTP line limit is exceeded, perr is set to SMTPc_ERR_LINE_TOO_LONG and the
*                   function returns without having added the header.
*
*               (3) This implementation transmit the headers buffer if the next header is too large to
*                   be inserted in the remaining buffer space.
*
*                   Note that NO EXACT calculations are performed here;  a conservative approach is
*                   brought forward, without actually optimizing the process (i.e. a buffer could be
*                   sent even though a few more characters could have been inserted).
*
*               (4) CRLF is inserted even though more entries are still to come for a particular header
*                   (line folding is performed even if unnecessary).
*********************************************************************************************************
*/

CPU_INT32U  SMTPc_BuildHdr (NET_SOCK_ID   sock,
                            CPU_CHAR     *buf,
                            CPU_INT32U    buf_size,
                            CPU_INT32U    buf_wr_ix,
                            CPU_CHAR     *hdr,
                            CPU_CHAR     *val,
                            CPU_INT32U   *line_len,
                            NET_ERR      *perr)
{
    CPU_INT32U  hdr_len;
    CPU_INT32U  val_len;
    CPU_INT32U  total_len;

                                                                /* ------------- CALCULATE NECESSARY SPACE ------------ */
    if (hdr == (CPU_CHAR *)0) {
        hdr_len = 0;
    } else {
        hdr_len = Str_Len(hdr);
    }

    if (val == (CPU_CHAR *)0) {
        val_len = 0;
    } else {
        val_len = Str_Len(val);
    }

    total_len = hdr_len + val_len + 2;

    if (*line_len + total_len > SMTPc_LINE_LEN_LIM) {           /* See note #2                                          */
        *perr = SMTPc_ERR_LINE_TOO_LONG;
         return (buf_wr_ix);
    }
                                                                /* -------------- SEND DATA, IF NECESSARY ------------- */
                                                                /* See note #3                                          */
    if ((buf_size - buf_wr_ix) < total_len) {
        SMTPc_QueryServer(sock, buf, buf_wr_ix, perr);
        if (*perr != SMTPc_ERR_NONE) {
            *perr = SMTPc_ERR_TX_FAILED;
             return buf_wr_ix;
        }
        buf_wr_ix = 0;
    }


                                                                /* ------------------ BUILDING HEADER ----------------- */
    if ( hdr != (CPU_CHAR *)0) {
         Mem_Copy(buf + buf_wr_ix, hdr, hdr_len);
         buf_wr_ix += hdr_len;
        *line_len += hdr_len;
    } else {                                                    /* Not first item, adding ','                           */
         Mem_Copy(buf + buf_wr_ix, " ,", 2);
         buf_wr_ix += 2;
        *line_len += 2;
    }
    if ( val != (CPU_CHAR *)0) {
         Mem_Copy(buf + buf_wr_ix, val, val_len);
         buf_wr_ix += val_len;
        *line_len += val_len;
    }

     Mem_Copy(buf + buf_wr_ix, SMTPc_CRLF, 2);                  /* See note #4                                          */
     buf_wr_ix += 2;
    *line_len  = 0;


     buf[buf_wr_ix] = '\0';
     SMTPc_TRACE_DBG(("String: %s\n", buf));

    *perr = SMTPc_ERR_NONE;
     return (buf_wr_ix);
}


/*
*********************************************************************************************************
*                                             SMTPc_HELO()
*
* Description : Build the HELO command, send it to the server and validate reply.
*               (a) Send command to the server
*               (b) Receive server's reply and validate
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               client_addr     Address literal helping identifying the client system.
*               completion_code Numeric value returned by server indication success of failure.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.
*                               SMTPc_ERR_RX_FAILED                 Error receiving server reply.
*                               SMTPc_ERR_REP                       Error with reply.
*
* Return(s)   : Complete reply from the server, if NO reception error.
*               (CPU_CHAR *)0,                  otherwise.
*
* Caller(s)   : SMTPc_Connect
*
* Note(s)     : (1) From RFC #2821, "the HELO command is used to identify the SMTP client to the SMTP
*                   server".
*
*               (2) The server will send a 250 "Requested mail action okay, completed" reply upon
*                   success.  A positive reply is the only reply that will lead to a  "SMTPc_ERR_NONE"
*                   error code return.
*
*               (3) This implementation will accept reply 250, as well as any other positive reply.
*********************************************************************************************************
*/

CPU_CHAR  *SMTPc_HELO (NET_SOCK_ID   sock,
                       NET_IP_ADDR   client_addr,
                       CPU_INT32U   *completion_code,
                       NET_ERR      *perr)
{
    CPU_CHAR     client_addr_literal[NET_ASCII_LEN_MAX_ADDR_IP];
    CPU_CHAR    *reply;
    CPU_SIZE_T   len;

                                                                /* ------------- SEND COMMAND TO SERVER --------------- */
    NetASCII_IP_to_Str(client_addr, client_addr_literal, DEF_NO, perr);

    Str_FmtPrint((char *)SMTPc_Comm_Buf,
                  SMTPc_COMM_BUF_LEN,
                 "%s %s%s",
                  SMTPc_CMD_HELO,
                  client_addr_literal,
                  SMTPc_CRLF);

    len = Str_Len(SMTPc_Comm_Buf);
    SMTPc_QueryServer(sock, SMTPc_Comm_Buf, len, perr);
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_TX_FAILED;
         return ((CPU_CHAR *)0);
    }

                                                                /* --------- RECEIVE SERVER'S REPLY & VALIDATE -------- */
    reply = SMTPc_RxReply(sock, perr);                          /* See note #2                                          */
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_RX_FAILED;
         return ((CPU_CHAR *)0);
    }

    SMTPc_ParseReply(reply, completion_code, perr);             /* See note #3                                          */
    if (*perr == SMTPc_ERR_REP_TOO_SHORT) {
        *perr  = SMTPc_ERR_REP;
         return ((CPU_CHAR *)0);
    }

    if (*completion_code == SMTPc_REP_250) {
        *perr = SMTPc_ERR_NONE;
    } else if (*perr == SMTPc_ERR_REP_POS) {                    /* Reply other than "250"                               */
        *perr = SMTPc_ERR_NONE;                                 /*     ... but accept any positive reply                */
    } else {
        *perr = SMTPc_ERR_REP;
    }

    return (reply);
}


/*
*********************************************************************************************************
*                                             SMTPc_MAIL()
*
* Description : Build the MAIL command, send it to the server and validate reply.
*               (a) Send command to the server
*               (b) Receive server's reply and validate
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               from            Argument of the "MAIL" command (sender mailbox).
*               completion_code Numeric value returned by server indication success of failure.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.
*                               SMTPc_ERR_RX_FAILED                 Error receiving server reply.
*                               SMTPc_ERR_REP                       Error with reply.
*
* Return(s)   : Complete reply from the server, if NO reception error.
*               (CPU_CHAR *)0,                  otherwise.
*
* Caller(s)   : SMTPc_SendMsg
*
* Note(s)     : (1) From RFC #2821, "the MAIL command is used to initiate a mail transaction in which
*                   the mail data is delivered to an SMTP server [...]".
*
*               (2) The server will send a 250 "Requested mail action okay, completed" reply upon
*                   success.  A positive reply is the only reply that will lead to a  "SMTPc_ERR_NONE"
*                   error code return.
*
*               (3) This implementation will accept reply 250, as well as any other positive reply.
*********************************************************************************************************
*/
CPU_CHAR  *SMTPc_MAIL (NET_SOCK_ID   sock,
                       CPU_CHAR     *from,
                       CPU_INT32U   *completion_code,
                       NET_ERR      *perr)
{
    CPU_CHAR    *reply;
    CPU_SIZE_T   len;

                                                                /* ------------- SEND COMMAND TO SERVER --------------- */
    Str_FmtPrint((char *)SMTPc_Comm_Buf,
                  SMTPc_COMM_BUF_LEN,
                 "%s FROM:%s%s",
                  SMTPc_CMD_MAIL,
                  from,
                  SMTPc_CRLF);

    len = Str_Len(SMTPc_Comm_Buf);
    SMTPc_QueryServer(sock, SMTPc_Comm_Buf, len, perr);
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_TX_FAILED;
        return ((CPU_CHAR *)0);
    }

                                                                /* ------- RECEIVE SERVER'S RESPONSE & VALIDATE ------- */
    reply = SMTPc_RxReply(sock, perr);                          /* See note #2                                          */
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_RX_FAILED;
         return ((CPU_CHAR *)0);
    }


    SMTPc_ParseReply(reply, completion_code, perr);             /* See note #3                                          */
    if (*perr == SMTPc_ERR_REP_TOO_SHORT) {
        *perr  = SMTPc_ERR_REP;
         return ((CPU_CHAR *)0);
    }

    if (*completion_code == SMTPc_REP_250) {
        *perr = SMTPc_ERR_NONE;
    } else if (*perr == SMTPc_ERR_REP_POS) {                    /* Reply other than "250"                               */
        *perr = SMTPc_ERR_NONE;                                 /*     ... but accept any positive reply                */
    } else {
        *perr = SMTPc_ERR_REP;
    }

    return (reply);
}


/*
*********************************************************************************************************
*                                             SMTPc_RCPT()
*
* Description : Build the RCPT command, send it to the server and validate reply.
*               (a) Send command to the server
*               (b) Receive server's reply and validate
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               to              Argument of the "RCPT" command (receiver mailbox).
*               completion_code Numeric value returned by server indication success of failure.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.
*                               SMTPc_ERR_RX_FAILED                 Error receiving server reply.
*                               SMTPc_ERR_REP                       Error with reply.
*
* Return(s)   : Complete reply from the server, if NO reception error.
*               (CPU_CHAR *)0,                  otherwise.
*
* Caller(s)   : SMTPc_SendMsg
*
* Note(s)     : (1) From RFC #2821, "the RCPT command is used to identify an individual recipient of the
*                   mail data; multiple recipients are specified by multiple use of this command".
*
*               (2) The server will send a 250 "Requested mail action okay, completed"  or a 251 "User
*                   not local; will forwarded to <forward-path>" reply upon success.
*
*               (3) This implementation will accept replies 250 and 251 as positive replies.  Reply 551
*                   "User not local; please try <forward-path>" will result in an error.
*********************************************************************************************************
*/

CPU_CHAR  *SMTPc_RCPT (NET_SOCK_ID   sock,
                       CPU_CHAR     *to,
                       CPU_INT32U   *completion_code,
                       NET_ERR      *perr)
{
    CPU_CHAR    *reply;
    CPU_SIZE_T   len;

                                                                /* ------------- SEND COMMAND TO SERVER --------------- */
    Str_FmtPrint((char *)SMTPc_Comm_Buf,
                  SMTPc_COMM_BUF_LEN,
                 "%s TO:%s%s",
                  SMTPc_CMD_RCPT,
                  to,
                  SMTPc_CRLF);

    len = Str_Len(SMTPc_Comm_Buf);
    SMTPc_QueryServer(sock, SMTPc_Comm_Buf, len, perr);
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_TX_FAILED;
         return ((CPU_CHAR *)0);
    }

                                                                /* ------- RECEIVE SERVER'S RESPONSE & VALIDATE ------- */
    reply = SMTPc_RxReply(sock, perr);                          /* See note #2                                          */
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_RX_FAILED;
         return ((CPU_CHAR *)0);
    }

    SMTPc_ParseReply(reply, completion_code, perr);             /* See note #3                                          */
    if (*perr == SMTPc_ERR_REP_TOO_SHORT) {
        *perr  = SMTPc_ERR_REP;
         return ((CPU_CHAR *)0);
    }

    if ((*completion_code == SMTPc_REP_250) ||
        (*completion_code == SMTPc_REP_251)) {
         *perr = SMTPc_ERR_NONE;
    } else {
         *perr = SMTPc_ERR_REP;
    }

    return (reply);
}


/*
*********************************************************************************************************
*                                             SMTPc_DATA()
*
* Description : Build the DATA command, send it to the server and validate reply.
*               (a) Send command to the server
*               (b) Receive server's reply and validate
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               completion_code Numeric value returned by server indication success of failure.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.
*                               SMTPc_ERR_RX_FAILED                 Error receiving server reply.
*                               SMTPc_ERR_REP                       Error with reply.
*
* Return(s)   : Complete reply from the server, if NO reception error.
*               (CPU_CHAR *)0,                  otherwise.
*
* Caller(s)   : SMTPc_SendMsg
*
* Note(s)     : (1) The DATA command is used to indicate to the SMTP server that all the following lines
*                   up to but not including the end of mail data indicator are to be considered as the
*                   message text.
*
*               (2) The receiver normally sends a 354 "Start mail input" reply and then treats the lines
*                   following the command as mail data from the sender.
*********************************************************************************************************
*/

CPU_CHAR   *SMTPc_DATA (NET_SOCK_ID   sock,
                        CPU_INT32U   *completion_code,
                        NET_ERR      *perr)
{
    CPU_CHAR    *reply;
    CPU_SIZE_T   len;

                                                                /* ------------- SEND COMMAND TO SERVER --------------- */
    Str_FmtPrint((char *)SMTPc_Comm_Buf,
                  SMTPc_COMM_BUF_LEN,
                 "%s%s",
                  SMTPc_CMD_DATA,
                  SMTPc_CRLF);

    len = Str_Len(SMTPc_Comm_Buf);
    SMTPc_QueryServer(sock, SMTPc_Comm_Buf, len, perr);
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_TX_FAILED;
         return ((CPU_CHAR *)0);
    }

                                                                /* ------- RECEIVE SERVER'S RESPONSE & VALIDATE ------- */
    reply = SMTPc_RxReply(sock, perr);                          /* See note #2                                          */
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_RX_FAILED;
         return ((CPU_CHAR *)0);
    }

    SMTPc_ParseReply(reply, completion_code, perr);
    if (*perr == SMTPc_ERR_REP_TOO_SHORT) {
        *perr  = SMTPc_ERR_REP;
         return ((CPU_CHAR *)0);
    }

    if (*completion_code == SMTPc_REP_354) {
        *perr = SMTPc_ERR_NONE;
    } else {
        *perr = SMTPc_ERR_REP;
    }

    return (reply);
}


/*
*********************************************************************************************************
*                                             SMTPc_RSET()
*
* Description : Build the RSET command, send it to the server and validate reply.
*               (a) Send command to the server
*               (b) Receive server's reply and validate
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               completion_code Numeric value returned by server indication success of failure.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.
*                               SMTPc_ERR_RX_FAILED                 Error receiving server reply.
*                               SMTPc_ERR_REP                       Error with reply.
*
* Return(s)   : Complete reply from the server, if NO reception error.
*               (CPU_CHAR *)0,                  otherwise.
*
* Caller(s)   : SMTPc_SendMsg
*
* Note(s)     : (1) From RFC #2821, "the RSET command specifies that the current mail transaction will
*                   be aborted.  Any stored sender, recipients, and mail data MUST be discarded, and all
*                   buffers and state tables cleared".
*
*               (2) The server MUST send a 250 "Requested mail action okay, completed" reply to a RSET
*                   command with no arguments.
*********************************************************************************************************
*/
CPU_CHAR  *SMTPc_RSET (NET_SOCK_ID   sock,
                       CPU_INT32U   *completion_code,
                       NET_ERR      *perr)
{
    CPU_CHAR    *reply;
    CPU_SIZE_T   len;

                                                                /* ------------- SEND COMMAND TO SERVER --------------- */
    Str_FmtPrint((char *)SMTPc_Comm_Buf,
                  SMTPc_COMM_BUF_LEN,
                 "%s%s",
                  SMTPc_CMD_RSET,
                  SMTPc_CRLF);

    len = Str_Len(SMTPc_Comm_Buf);
    SMTPc_QueryServer(sock, SMTPc_Comm_Buf, len, perr);
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_TX_FAILED;
         return ((CPU_CHAR *)0);
    }

                                                                /* ------- RECEIVE SERVER'S RESPONSE & VALIDATE ------- */
    reply = SMTPc_RxReply(sock, perr);                          /* See note #2                                          */
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_RX_FAILED;
         return ((CPU_CHAR *)0);
    }

    SMTPc_ParseReply(reply, completion_code, perr);
    if (*perr == SMTPc_ERR_REP_TOO_SHORT) {
        *perr  = SMTPc_ERR_REP;
         return ((CPU_CHAR *)0);
    }

    if (*completion_code == SMTPc_REP_250) {
        *perr = SMTPc_ERR_NONE;
    } else {
        *perr = SMTPc_ERR_REP;
    }

    return (reply);
}


/*
*********************************************************************************************************
*                                             SMTPc_QUIT()
*
* Description : Build the QUIT command, send it to the server and validate reply.
*               (a) Send command to the server
*               (b) Receive server's reply and validate
*
* Argument(s) : sock            Socket ID returned from SMTPc_Connect.
*               completion_code Numeric value returned by server indication success of failure.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error.
*                               SMTPc_ERR_TX_FAILED                 Error querying server.
*                               SMTPc_ERR_RX_FAILED                 Error receiving server reply.
*                               SMTPc_ERR_REP                       Error with reply.
*
* Return(s)   : Complete reply from the server, if NO reception error.
*               (CPU_CHAR *)0,                  otherwise.
*
* Caller(s)   : SMTPc_Disconnect
*
* Note(s)     : (1) From RFC #2821, "the QUIT command specifies that the receiver MUST send an OK reply,
*                   and then close the transmission channel.  The receiver MUST NOT intentionally close
*                   the transmission channel until it receives and replies to a QUIT command (even if
*                   there was an error).  the sender MUST NOT intentionally close the transmission
*                   channel until it sends a QUIT command and SHOULD wait until it receives the reply
*                   (even if there was an error response to a previous command)".
*
*               (2) The server MUST send a 221 "Service closing transmission channel" reply to a QUIT
*                   command.
*********************************************************************************************************
*/

CPU_CHAR  *SMTPc_QUIT (NET_SOCK_ID   sock,
                       CPU_INT32U   *completion_code,
                       NET_ERR      *perr)
{
    CPU_CHAR    *reply;
    CPU_SIZE_T   len;

                                                                /* ------------- SEND COMMAND TO SERVER --------------- */
    Str_FmtPrint((char *)SMTPc_Comm_Buf,
                  SMTPc_COMM_BUF_LEN,
                 "%s%s",
                  SMTPc_CMD_QUIT,
                  SMTPc_CRLF);

    len = Str_Len(SMTPc_Comm_Buf);
    SMTPc_QueryServer(sock, SMTPc_Comm_Buf, len, perr);
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_TX_FAILED;
         return ((CPU_CHAR *)0);
    }

                                                                /* ------- RECEIVE SERVER'S RESPONSE & VALIDATE ------- */
    reply = SMTPc_RxReply(sock, perr);                          /* See note #2.                                         */
    if (*perr != SMTPc_ERR_NONE) {
        *perr  = SMTPc_ERR_RX_FAILED;
         return ((CPU_CHAR *)0);
    }

    SMTPc_ParseReply(reply, completion_code, perr);
    if (*perr == SMTPc_ERR_REP_TOO_SHORT) {
        *perr  = SMTPc_ERR_REP;
         return ((CPU_CHAR *)0);
    }

    if (*completion_code == SMTPc_REP_221) {
        *perr  = SMTPc_ERR_NONE;
    } else {
        *perr  = SMTPc_ERR_REP;
    }

    return (reply);
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
