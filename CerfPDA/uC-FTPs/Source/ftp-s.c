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
* Filename      : ftp-s.c
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   FTSs_MODULE
#include  <includes.h>
#include  <net.h>
#include  <ftp-s.h>
#include  <fs_api.h>


/*
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* Used to keep track of the number of control tasks    */
static  CPU_INT32U    FTPs_CtrlTasks;                           /* running on the system.  Actually 0 or 1.             */

                                                                /* Used to set the public IP address (the IP address    */
                                                                /* your NAT (router) on internet.  Needed only if you   */
static  NET_IP_ADDR   FTPs_PublicAddr;                          /* use the FTP passive mode.                            */

                                                                /* Used to set the public IP port (the IP port you have */
                                                                /* opened in your router and redirected to the internal */
                                                                /* IP address and port of the machine on which this     */
                                                                /* server runs.  Needed only if you use he FTP passive  */
static  NET_PORT_NBR  FTPs_PublicPort;                          /* mode.                                                */


/*
*********************************************************************************************************
*                                           INITIALIZED DATA
*********************************************************************************************************
*/

                                                                /* This structure is used to build a table of command   */
                                                                /* codes and their corresponding string.  The context   */
                                                                /* is the state(s) in which the command is allowed.     */
static  const  FTPs_CMD_STRUCT  FTPs_Cmd[] = {
                         /*    L        L        G        G        G    */
                         /*    O        O        O        O        O    */
                         /*    G        G        T        T        T    */
                         /*    O        I        U        R        R    */
                         /*    U        N        S        N        E    */
                         /*    T                 E        F        S    */
                         /*                      R        R        T    */
    {FTP_CMD_NOOP, "NOOP", {DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON }},
    {FTP_CMD_QUIT, "QUIT", {DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON }},
    {FTP_CMD_REIN, "REIN", {DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON }},
    {FTP_CMD_SYST, "SYST", {DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON }},
    {FTP_CMD_FEAT, "FEAT", {DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON }},
    {FTP_CMD_HELP, "HELP", {DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON,  DEF_ON }},
    {FTP_CMD_USER, "USER", {DEF_ON,  DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_PASS, "PASS", {DEF_OFF, DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF}},
    {FTP_CMD_MODE, "MODE", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_TYPE, "TYPE", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_STRU, "STRU", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_PASV, "PASV", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_PORT, "PORT", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_PWD,  "PWD" , {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_CWD,  "CWD" , {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_CDUP, "CDUP", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_MKD,  "MKD" , {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_RMD,  "RMD" , {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_NLST, "NLST", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_LIST, "LIST", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_RETR, "RETR", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_ON }},
    {FTP_CMD_STOR, "STOR", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_ON }},
    {FTP_CMD_APPE, "APPE", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_REST, "REST", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_DELE, "DELE", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_RNFR, "RNFR", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_RNTO, "RNTO", {DEF_OFF, DEF_OFF, DEF_OFF, DEF_ON,  DEF_OFF}},
    {FTP_CMD_SIZE, "SIZE", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
    {FTP_CMD_MDTM, "MDTM", {DEF_OFF, DEF_ON,  DEF_OFF, DEF_OFF, DEF_OFF}},
                                                                /* This line MUST be the LAST!                          */
    {FTP_CMD_MAX,  "MAX" , {DEF_OFF, DEF_OFF, DEF_OFF, DEF_OFF, DEF_OFF}}
};

                                                                /* This table is used to match the incoming reply code  */
                                                                /* to its corresponding message string.                 */
static  const  FTPs_REPLY_STRUCT  FTPs_Reply[] = {
    {FTP_REPLY_CODE_OKAYOPENING,     "150 File status okay; about to open data connection."},
    {FTP_REPLY_CODE_OKAY,            "200 Command okay."},
    {FTP_REPLY_CODE_SYSTEMSTATUS,    "211-Extensions supported:\n" \
                                     " REST STREAM\n" \
                                     " MDTM\n" \
                                     " SIZE\n" \
                                     "211 End"},
    {FTP_REPLY_CODE_FILESTATUS,      "213 File status."},
    {FTP_REPLY_CODE_HELPMESSAGE,     "214-Commands recognized:\n" \
                                     " NOOP  QUIT  REIN  SYST  FEAT  HELP  USER  PASS\n" \
                                     " MODE  TYPE  STRU  PASV  PORT  PWD   CWD   CDUP\n" \
                                     " MKD   RMD   NLST  LIST  RETR  STOR  APPE  REST\n" \
                                     " DELE  RNFR  RNTO  SIZE  MDTM\n" \
                                     "214 End"},
    {FTP_REPLY_CODE_SYSTEMTYPE,      "215 UNIX Type: L8."},
    {FTP_REPLY_CODE_SERVERREADY,     "220 Service ready for new user."},
    {FTP_REPLY_CODE_SERVERCLOSING,   "221 Service closing control connection."},
    {FTP_REPLY_CODE_CLOSINGSUCCESS,  "226 Closing data connection."},
    {FTP_REPLY_CODE_ENTERPASVMODE,   "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)."},
    {FTP_REPLY_CODE_LOGGEDIN,        "230 User logged in, proceed."},
    {FTP_REPLY_CODE_ACTIONCOMPLETE,  "250 Requested file action okay, completed."},
    {FTP_REPLY_CODE_PATHNAME,        "257 \"%s\" created."},
    {FTP_REPLY_CODE_NEEDPASSWORD,    "331 User name okay, need password."},
    {FTP_REPLY_CODE_NEEDMOREINFO,    "350 Requested file action pending further information."},
    {FTP_REPLY_CODE_NOSERVICE,       "421 Service not available, closing control connection."},
    {FTP_REPLY_CODE_CANTOPENDATA,    "425 Can't open data connection."},
    {FTP_REPLY_CODE_CLOSEDCONNABORT, "426 Connection closed; transfer aborted."},
    {FTP_REPLY_CODE_PARMSYNTAXERR,   "501 Syntax error in parameters or arguments."},
    {FTP_REPLY_CODE_CMDNOSUPPORT,    "502 Command not implemented."},
    {FTP_REPLY_CODE_CMDBADSEQUENCE,  "503 Bad sequence of commands."},
    {FTP_REPLY_CODE_PARMNOSUPPORT,   "504 Command not implemented for that parameter."},
    {FTP_REPLY_CODE_NOTLOGGEDIN,     "530 Not logged in."},
    {FTP_REPLY_CODE_NOTFOUND,        "550 Requested action not taken. %s unavailable."},
    {FTP_REPLY_CODE_ACTIONABORTED,   "551 Requested action aborted."},
    {FTP_REPLY_CODE_NOSPACE,         "552 Requested file action aborted. Exceeded storage allocation."},
    {FTP_REPLY_CODE_NAMEERR,         "553 Requested action not taken. File name not allowed."},
};

                                                                /* Table used to display month name abbreviation in     */
                                                                /* directory listings like the UNIX command ls.         */
static  const  CPU_CHAR  *FTPs_Month_Name[] = {
    "jan",
    "feb",
    "mar",
    "apr",
    "may",
    "jun",
    "jul",
    "aug",
    "sep",
    "oct",
    "nov",
    "dec",
};


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                                /* Convert FTP (Unix) style path to FS style path.      */
static  void          FTPs_ToFSStylePath   (CPU_CHAR              *path);

                                                                /* Find and tokenize next argument.                     */
static  CPU_CHAR     *FTPs_FindArg         (CPU_CHAR             **pp_buf);

                                                                /* Find file name (last argument).                      */
static  CPU_CHAR     *FTPs_FindFileName    (CPU_CHAR             **pp_buf);

                                                                /* Do all path name parsing and generation stuff.       */
static  CPU_BOOLEAN   FTPs_BuildPath       (CPU_CHAR              *full_abs_path,
                                            CPU_INT32U             full_abs_path_len,
                                            CPU_CHAR              *full_rel_path,
                                            CPU_INT32U             full_rel_path_len,
                                            CPU_CHAR              *parent_abs_path,
                                            CPU_INT32U             parent_abs_path_len,
                                            CPU_CHAR              *cur_entry,
                                            CPU_INT32U             cur_entry_len,
                                            CPU_CHAR              *base_path,
                                            CPU_CHAR              *rel_path,
                                            CPU_CHAR              *new_path);

                                                                /* Send reply message to a command.                     */
static  NET_ERR       FTPs_SendReply       (CPU_INT32S             sock_id,
                                            CPU_INT32S             reply_nbr,
                                            CPU_CHAR              *reply_msg);

                                                                /* Start passive mode listenning socket.                */
static  void          FTPs_StartPasvMode   (FTPs_SESSION_STRUCT   *ftp_session);

                                                                /* Stop passive mode listenning socket.                 */
static  void          FTPs_StopPasvMode    (FTPs_SESSION_STRUCT   *ftp_session);

                                                                /* Core execution routines of the FTP commands.         */
static  void          FTPs_ProcessCtrlCmd  (FTPs_SESSION_STRUCT   *ftp_session);

                                                                /* Core execution routines of the FTP data transfer     */
                                                                /* commands.                                            */
static  void          FTPs_ProcessDtpCmd   (FTPs_SESSION_STRUCT   *ftp_session);

                                                                /* Data transfer task:                                  */
                                                                /* data transfer session with the client.               */
static  void          FTPs_Dtp_Task        (void                  *p_arg);

/*
*********************************************************************************************************
*                                            LOCAL FUNCTIONS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         FTPs_ToFSStylePath()
*
* Description : Convert FTP (Unix) style path to FS style path.
*
* Argument(s) : path        path to be converted.
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_ProcessCtrlCmd
*********************************************************************************************************
*/

static  void  FTPs_ToFSStylePath (CPU_CHAR  *path)
{
    while (*path != (CPU_CHAR)0) {
        if (*path == FTPs_PATH_SEP_CHAR) {
            *path  = FTPs_FS_PATH_SEP_CHAR;
        }
        path++;
    }
}


/*
*********************************************************************************************************
*                                            FTPs_FindArg()
*
* Description : Find and tokenize next argument.
*
* Argument(s) : pp_buf      buffer to find next argument.
*
* Return(s)   : Network error code.
*
* Caller(s)   : Pointer to the found argument.
*
* Note        : This function will find the next argument.
*               First, every characters that verifies the Str_IsSpace() function are skipped.
*               Second, every characters that don't verifies the Str_IsSpace() function are considerated to
*               be the next argument.  NULL character is inserted at the end of the argument.
*               The return value is a pointer to the beginning of the argument.  pp_buf is modified and
*               points to the character next to the inserted NULL character (ready to find another
*               argument.  You can call this function over and over to find all arguments.  pp_buf will
*               points to a NULL character when there is no more arguments.
*********************************************************************************************************
*/

static  CPU_CHAR* FTPs_FindArg (CPU_CHAR  **pp_buf)
{
    CPU_CHAR    *p_arg;
    CPU_CHAR    *p_buf;
    CPU_INT32S   rtn_val;


                                                                /* Find the beginning of the argument.                  */
    p_buf = *pp_buf;
    while (*p_buf != (CPU_CHAR)0) {
        rtn_val = Str_IsSpace(*p_buf);
        if (rtn_val == 0) {
            break;
        }
        p_buf++;
    }
    p_arg = p_buf;

                                                                /* Find the end of the argument.                        */
    while (*p_buf != (CPU_CHAR)0) {
        rtn_val = Str_IsSpace(*p_buf);
        if (rtn_val != 0) {
            break;
        }
        p_buf++;
    }

                                                                /* If not the end of the command line, prepare to fetch */
                                                                /* next paramter.                                       */
    if (*p_buf != (CPU_CHAR)0) {
        *p_buf = (CPU_CHAR)0;
        p_buf++;
    }

    *pp_buf = p_buf;
    return (p_arg);
}


/*
*********************************************************************************************************
*                                          FTPs_FindFileName()
*
* Description : Find file name (last argument).
*
* Argument(s) : pp_buf      buffer to find file name.
*
* Return(s)   : Network error code.
*
* Caller(s)   : Pointer to the found argument.
*
* Note        : This function will find a file name in the string.
*               First, every characters that verifies the Str_IsSpace() function are skipped.
*               Second, every characters that verifies the Str_IsPrint() function are considerated to
*               be the next argument.  NULL character is inserted at the end of the argument.
*********************************************************************************************************
*/

static  CPU_CHAR* FTPs_FindFileName (CPU_CHAR  **pp_buf)
{
    CPU_CHAR    *p_arg;
    CPU_CHAR    *p_buf;
    CPU_INT32S   rtn_val;


                                                                /* Find the beginning of the argument.                  */
    p_buf = *pp_buf;
    while (*p_buf != (CPU_CHAR)0) {
        rtn_val = Str_IsSpace(*p_buf);
        if (rtn_val == 0) {
            break;
        }
        p_buf++;
    }
    p_arg = p_buf;

                                                                /* Find the end of the argument.                        */
    while (*p_buf != (CPU_CHAR)0) {
        rtn_val = Str_IsPrint(*p_buf);
        if (rtn_val == 0) {
            break;
        }
        p_buf++;
    }

    *p_buf = (CPU_CHAR)0;
    return (p_arg);
}


/*
*********************************************************************************************************
*                                           FTPs_BuildPath()
*
* Description : Do all path name parsing and generation stuff.
*
* Argument(s) : full_abs_path           caller-allocated buffer to contain new fully-qualified path.
*               full_abs_path_len       length of full_abs_path.
*               full_rel_path           caller-allocated buffer to contain new relative path.
*               full_rel_path_len       length of full_rel_path.
*               parent_abs_path         caller-allocated buffer to contain new fully-qualified parent path.
*               parent_abs_path_len     length of parent_abs_path.
*               cur_entry               caller-allocated buffer to contain new directory or file name only.
*               cur_entry_len           length of cur_entry.
*               base_path               user base path.  Relative path will be build from this path.
*               rel_path                current relative path.  This is the path returned by the PWD command.
*               new_path                new directory or file name.
*
* Return(s)   : DEF_OK:         build path succedded.
*               DEF_FAIL:       build path failed.
*
* Caller(s)   : FTPs_Ctrl_Task()
*
* Note        : More explainations about the path generated by this function:
*                   1. base_path is defined for each user and represent the higher level of path that the
*                      user can access.  Ex.: if base_path = "/FTPROOT", when user does "CWD /", the
*                      system will actually points to "/FTPROOT" in the filesystem, but the user see "/".
*                   2. base_path can be set to "/".  With this setting, user will see all the filesystem.
*                      This configuration is, however, insecure if you want to hide some files to user.
*                   3. rel_path is the current path that the user see.  Is is concatened to the base_path
*                      for filesystem translation.  Ex.: if user types "CWD /test" and base_path is
*                      "/TFTPROOT", rel_path will be "/test", but the directory accessed on filesystem
*                      will be "/TFTPROOT/test".
*                   4. new_path is the directory or file accessed.  Generation of new paths depends
*                      greatly if new_path is a relative or absolute path.  If new_path is an absolute
*                      path, it will be copied directly into full_rel_path.
*                   5. If new_path is a relative path, it will be contatened with rel_path to create
*                      full_rel_path.
*
*               I will now represent generation of path by arithmetics.  I define '+' as the operator of
*               concatenation, '=' is the operator of affectation, parent() returns the parent
*               directory and entry() returns the file of leaf directory name only.
*
*                   If new_path is an absolute path :
*                       1.  full_rel_path   = new_path
*                       2.  full_abs_path   = base_path + full_rel_path
*                       3.  parent_abs_path = base_path + parent(full_rel_path)
*                       4.  cur_entry       = entry(full_rel_path)
*                   If new_path is a relative path :
*                       if new_path == "."
*                           1.  full_rel_path   = rel_path
*                           2.  full_abs_path   = base_path + full_rel_path
*                           3.  parent_abs_path = base_path + parent(full_rel_path)
*                           4.  cur_entry       = entry(full_rel_path)
*                       if new_path == ".."
*                           2.  full_rel_path   = parent(rel_path)
*                           3.  full_abs_path   = base_path + full_rel_path
*                           4.  parent_abs_path = base_path + parent(full_rel_path)
*                           5.  cur_entry       = entry(full_rel_path)
*                       else
*                           1.  full_rel_path   = rel_path + new_path
*                           2.  full_abs_path   = base_path + full_rel_path
*                           3.  parent_abs_path = base_path + parent(full_rel_path)
*                           4.  cur_entry       = entry(full_rel_path)
*
*               PATH SEPARATOR CHARACTER: FTP uses FTPs_PATH_SEP_CHAR path separator character.
*               This function assumes and generate path with FTPs_PATH_SEP_CHAR character only.
*
*********************************************************************************************************
*/

static  CPU_BOOLEAN  FTPs_BuildPath (CPU_CHAR    *full_abs_path,
                                     CPU_INT32U   full_abs_path_len,
                                     CPU_CHAR    *full_rel_path,
                                     CPU_INT32U   full_rel_path_len,
                                     CPU_CHAR    *parent_abs_path,
                                     CPU_INT32U   parent_abs_path_len,
                                     CPU_CHAR    *cur_entry,
                                     CPU_INT32U   cur_entry_len,
                                     CPU_CHAR    *base_path,
                                     CPU_CHAR    *rel_path,
                                     CPU_CHAR    *new_path)
{
    CPU_INT32U   cmp_value;
    CPU_CHAR    *p_buf;


                                                                /* An empty new_path means current path.                */
    if (*new_path == (CPU_CHAR)0) {
        new_path = ".";
    }

                                                                /* Remove the ending FTPs_PATH_SEP_CHAR, if any.        */
    cmp_value = Str_Cmp(new_path, FTPs_ROOT_PATH);
    if (cmp_value != 0) {
        cmp_value = Str_Len(new_path);
        if (new_path[cmp_value - 1] == FTPs_PATH_SEP_CHAR) {
            new_path[cmp_value - 1] = (CPU_CHAR)0;
        }
    }

                                                                /* Determine if new_path is an absolute path, a re-     */
                                                                /* lative path, FTPs_CURRENT_PATH or FTPs_PARENT_PATH.  */

    if (*new_path == FTPs_PATH_SEP_CHAR) {                      /* Absolute path.                                       */
                                                                /* 1.  full_rel_path   = new_path                       */
        Str_FmtPrint((char *)full_rel_path,
                       full_rel_path_len,
                      "%s",
                       new_path);
    } else {
                                                                /* Current path.                                        */
                                                                /* 1.  full_rel_path   = rel_path                       */
       cmp_value = Str_Cmp(new_path, FTPs_CURRENT_PATH);
        if (cmp_value == 0) {
            Str_FmtPrint((char *)full_rel_path,
                           full_rel_path_len,
                          "%s",
                           rel_path);
        } else {
                                                                /* Parent path.                                         */
                                                                /* 1.  full_rel_path   = parent(rel_path)               */
            cmp_value = Str_Cmp(new_path, FTPs_PARENT_PATH);
            if (cmp_value == 0) {
                Str_FmtPrint((char *)full_rel_path,
                               full_rel_path_len,
                              "%s",
                               rel_path);
                p_buf = Str_Char_Last(full_rel_path, FTPs_PATH_SEP_CHAR);
                if (p_buf == full_rel_path) {
                    p_buf++;
                }
                *p_buf = (CPU_CHAR)0;
            } else {
                                                                /* Relative path.                                       */
                                                                /* 1.  full_rel_path   = rel_path + new_path            */
                cmp_value = Str_Cmp(rel_path, FTPs_ROOT_PATH);
                if (cmp_value == 0) {
                    Str_FmtPrint((char *)full_rel_path,
                                   full_rel_path_len,
                                  "%s%s",
                                   rel_path,
                                   new_path);
                } else {
                    Str_FmtPrint((char *)full_rel_path,
                                   full_rel_path_len,
                                  "%s%c%s",
                                   rel_path,
                                   FTPs_PATH_SEP_CHAR,
                                   new_path);
                }
            }
        }
    }

                                                                /* 2.  full_abs_path   = base_path + full_rel_path      */
    cmp_value = Str_Cmp(base_path, FTPs_ROOT_PATH);
    if (cmp_value == 0) {
        Str_FmtPrint((char *)full_abs_path,
                       full_abs_path_len,
                      "%s",
                       full_rel_path);
    } else {
        Str_FmtPrint((char *)full_abs_path,
                       full_abs_path_len,
                      "%s%s",
                       base_path,
                       full_rel_path);
    }

                                                                /* 3.  parent_abs_path = base_path +                    */
                                                                /*                       parent(full_rel_path)          */
    Str_FmtPrint((char *)parent_abs_path, parent_abs_path_len, "%s", full_abs_path);

    p_buf = Str_Char_Last(parent_abs_path, FTPs_PATH_SEP_CHAR);

                                                                /* 4.  cur_entry       = entry(full_rel_path)           */
    Str_FmtPrint((char *)cur_entry, cur_entry_len, "%s", p_buf + 1);

    if (p_buf == parent_abs_path) {
        p_buf++;
    }
    *p_buf = (CPU_CHAR)0;

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                               FTPs_Tx()
*
* Description : Transmit data to TCP socket, handling transient errors and incomplete buffer transmit.
*
* Argument(s) : sock_id             TCP socket ID.
*               net_buf             buffer to send.
*               net_buf_len         length of buffer to send.
*               net_err             contains error message returned.
*
* Return(s)   : DEF_FAIL        transmission failed.
*               DEF_OK          transmission successful.
*
* Caller(s)   : Various.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  FTPs_Tx (CPU_INT32S   sock_id,
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
           (tx_retry_cnt <  FTPs_CFG_DTP_MAX_TX_RETRY) &&       /* ... & tx retry   < MAX     ...   */
           (tx_done      == DEF_NO)) {                          /* ... & tx NOT done;         ...   */

        if (tx_dly == DEF_YES) {                                /* Dly tx, if req'd.                */
            FTPs_OS_TxDly();
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


/*
*********************************************************************************************************
*                                           FTPs_SendReply()
*
* Description : Send reply message to a command.
*
* Argument(s) : sock_id         control socket ID.
*               reply_nbr       reply message number.
*               reply_msg       reply message (set to NULL to send the official message related
*                               to reply_nbr).
*
* Return(s)   : Network error code.
*
* Caller(s)   : FTPs_Ctrl_Task().
*
* Note        : This function is a convenience function to build and send a reply message back to a
*               client.  If the incoming reply_msg string is NULL, then the FTPs_Reply[] table is
*               searched for reply code's corresponding message string.  If the reply_msg string pointer
*               is not NULL, then it is used instead of the default reply message.
*********************************************************************************************************
*/

static  NET_ERR  FTPs_SendReply (CPU_INT32S   sock_id,
                                 CPU_INT32S   reply_nbr,
                                 CPU_CHAR    *reply_msg)
{
    CPU_CHAR    net_buf[FTPs_NET_BUF_LEN];
    CPU_INT32S  net_buf_len;
    NET_ERR     net_err;


    if (reply_msg == (CPU_CHAR *)0) {
        net_buf_len = Str_FmtPrint((char *)net_buf, sizeof(net_buf), "%s\n",
                                     FTPs_Reply[reply_nbr].ReplyStr);
    } else {
        net_buf_len = Str_FmtPrint((char *)net_buf, sizeof(net_buf), "%s\n",
                                     reply_msg);
    }

    FTPs_TRACE_INFO(("FTPs TX: %s", net_buf));

    FTPs_Tx(sock_id, net_buf, net_buf_len, &net_err);
    if (net_err != NET_SOCK_ERR_NONE) {
        FTPs_TRACE_DBG(("FTPs FTPs_Tx() failed: reply #%d, error #%d, line #%d.\n",
                          reply_nbr, net_err, __LINE__));
    }
    return (net_err);
}


/*
*********************************************************************************************************
*                                         FTPs_StartPasvMode()
*
* Description : Start passive mode listenning socket.
*
* Argument(s) : ftp_session     structure that contains FTP session states and control data.
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_Ctrl_Task()
*********************************************************************************************************
*/

static  void  FTPs_StartPasvMode (FTPs_SESSION_STRUCT  *ftp_session)
{
    CPU_INT32S  pasv_sock_id;
    NET_ERR     net_err;


    if (ftp_session->DtpPasv == DEF_NO) {
                                                /* Open a socket.                                       */
        FTPs_TRACE_INFO(("FTPs OPEN passive DTP socket.\n"));
        pasv_sock_id = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                                     NET_SOCK_TYPE_STREAM,
                                     NET_SOCK_PROTOCOL_TCP,
                                    &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
            FTPs_TRACE_DBG(("FTPs NetSock_Open() failed: error #%d, line #%d.\n", net_err, __LINE__));
            return;
        }

                                                                /* Bind a local address so the clients can send to us.  */
        NetSock_Bind(pasv_sock_id,
                     (NET_SOCK_ADDR *)&ftp_session->DtpSockAddr,
                     sizeof(ftp_session->DtpSockAddr),
                     &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
            NetSock_Close(pasv_sock_id, &net_err);
            FTPs_TRACE_DBG(("FTPs NetSock_Bind() failed: error #%d, line #%d.\n", net_err, __LINE__));
            return;
        }

                                                                /* Listen to the socket for clients.                    */
        NetSock_Listen(pasv_sock_id, FTPs_DTP_CONN_Q_SIZE, &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
            NetSock_Close(pasv_sock_id, &net_err);
            FTPs_TRACE_DBG(("FTPs NetSock_Listen() failed: error #%d, line #%d.\n", net_err, __LINE__));
            return;
        }

        ftp_session->DtpPasv       = DEF_YES;
        ftp_session->DtpPasvSockID = pasv_sock_id;
    }
}


/*
*********************************************************************************************************
*                                          FTPs_StopPasvMode()
*
* Description : Stop passive mode listenning socket.
*
* Argument(s) : ftp_session     structure that contains FTP session states and control data.
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_Ctrl_Task()
*********************************************************************************************************
*/

static  void  FTPs_StopPasvMode (FTPs_SESSION_STRUCT  *ftp_session)
{
    NET_ERR  net_err;


    if (ftp_session->DtpPasv == DEF_YES) {
        FTPs_TRACE_INFO(("FTPs CLOSE passive DTP socket.\n"));
        NetSock_Close(ftp_session->DtpPasvSockID, &net_err);
        ftp_session->DtpPasv =  DEF_NO;
    }
}


/*
*********************************************************************************************************
*                                         FTPs_ProcessDtpCmd()
*
* Description : Core execution routines of the FTP data transfer commands.
*
* Argument(s) : ftp_session     structure that contains FTP session states and control data.
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_Dtp_Task()
*********************************************************************************************************
*/

static  void  FTPs_ProcessDtpCmd(FTPs_SESSION_STRUCT  *ftp_session)
{
    CPU_CHAR      net_buf[FTPs_NET_BUF_LEN];
    CPU_INT16S    net_len;
    CPU_INT32S    fs_len;
    CPU_INT32U    str_len_ttl;
    CPU_INT32U    str_len;
    CPU_CHAR     *prn_buf;
    CPU_INT16S    prn_buf_len;

    FS_FILE      *p_file;
    FS_DIR       *p_dir;
    FS_DIRENT    *p_dirent;
    FS_FILETIME   file_time;
    CPU_CHAR      attr_dir;
    CPU_CHAR      attr_ro;
    NET_ERR       net_err;
    CPU_INT32S    fs_err;


    str_len_ttl = 0;
    p_file      = (FS_FILE   *)0;
    p_dir       = (FS_DIR    *)0;
    p_dirent    = (FS_DIRENT *)0;
    net_err     = NET_SOCK_ERR_NONE;

    switch (ftp_session->DtpCmd) {
        case FTP_CMD_NLST:
             p_dir    = FS_OpenDir((char *)ftp_session->CurEntry);
             p_dirent = FS_ReadDir(p_dir);
             while (p_dirent != (FS_DIRENT *)0) {
                 prn_buf     = net_buf + str_len_ttl;
                 prn_buf_len = sizeof(net_buf) - str_len_ttl;
                 str_len = Str_FmtPrint((char *)prn_buf,
                                          prn_buf_len,
                                         "%s\n",
                                          p_dirent->DirName);
                 if (str_len_ttl + str_len >= sizeof(net_buf)) {
                     FTPs_Tx(ftp_session->DtpSockID, net_buf, str_len_ttl, &net_err);
                     if (net_err != NET_SOCK_ERR_NONE) {
                         FTPs_TRACE_DBG(("FTPs FTPs_Tx() failed: error #%d, line #%d.\n", net_err, __LINE__));
                         str_len_ttl = 0;
                         break;
                     }

                     str_len_ttl = 0;
                     continue;
                 }

                 str_len_ttl += str_len;
                 p_dirent = FS_ReadDir(p_dir);
             }

             if (str_len_ttl > 0) {
                 FTPs_Tx(ftp_session->DtpSockID, net_buf, str_len_ttl, &net_err);
                 if (net_err != NET_SOCK_ERR_NONE) {
                     FTPs_TRACE_DBG(("FTPs FTPs_Tx() failed: error #%d, line #%d.\n", net_err, __LINE__));
                     break;
                 }
             }
             FS_CloseDir(p_dir);

             if (net_err == NET_SOCK_ERR_NONE) {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CLOSINGSUCCESS, (CPU_CHAR *)0);
             } else {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CLOSEDCONNABORT, (CPU_CHAR *)0);
             }
             break;

        case FTP_CMD_LIST:
             p_dir    = FS_OpenDir((char *)ftp_session->CurEntry);
             p_dirent = FS_ReadDir(p_dir);
             while (p_dirent != (FS_DIRENT *)0) {
                 if ((p_dirent->Attributes & (FS_ATTR_HIDDEN | FS_ATTR_SYSTEM)) == 0) {
                     if ((p_dirent->Attributes & FS_ATTR_DIRECTORY) == FS_ATTR_DIRECTORY) {
                         attr_dir = 'd';
                     } else {
                         attr_dir = '-';
                     }
                     if ((p_dirent->Attributes & FS_ATTR_READ_ONLY) == FS_ATTR_READ_ONLY) {
                         attr_ro  = '-';
                     } else {
                         attr_ro  = 'w';
                     }

                     FS_TimeStampToFileTime(p_dirent->TimeStamp, &file_time);

                     prn_buf     = net_buf + str_len_ttl;
                     prn_buf_len = sizeof(net_buf) - str_len_ttl;
                     str_len = Str_FmtPrint((char *)prn_buf,
                                              prn_buf_len,
                                             "%cr%c-r%c-r%c-   1 user     group    %8d %3s %2d  %4d %s\n",
                                              attr_dir,
                                              attr_ro, attr_ro, attr_ro,
                                              p_dirent->Size,
                                              FTPs_Month_Name[file_time.Month - 1],
                                              file_time.Day,
                                              file_time.Year,
                                              p_dirent->DirName);

                     if (str_len_ttl + str_len >= sizeof(net_buf)) {
                         FTPs_Tx(ftp_session->DtpSockID, net_buf, str_len_ttl, &net_err);
                         if (net_err != NET_SOCK_ERR_NONE) {
                             FTPs_TRACE_DBG(("FTPs FTPs_Tx() failed: error #%d, line #%d.\n", net_err, __LINE__));
                             str_len_ttl = 0;
                             break;
                         }

                         str_len_ttl = 0;
                         continue;
                     }

                     str_len_ttl += str_len;
                 }
                 p_dirent = FS_ReadDir(p_dir);
             }

             if (str_len_ttl > 0) {
                 FTPs_Tx(ftp_session->DtpSockID, net_buf, str_len_ttl, &net_err);
                 if (net_err != NET_SOCK_ERR_NONE) {
                     FTPs_TRACE_DBG(("FTPs FTPs_Tx() failed: error #%d, line #%d.\n", net_err, __LINE__));
                     break;
                 }
             }
             FS_CloseDir(p_dir);

             if (net_err == NET_SOCK_ERR_NONE) {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CLOSINGSUCCESS, (CPU_CHAR *)0);
             } else {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CLOSEDCONNABORT, (CPU_CHAR *)0);
             }
             break;

        case FTP_CMD_RETR:
             p_file = FS_FOpen((char *)ftp_session->CurEntry, "rb");
             if (p_file == (FS_FILE *)0) {
                 Str_FmtPrint((char *)net_buf, sizeof(net_buf), "551 Cannot open %s: access denied.", ftp_session->CurEntry);
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_ACTIONABORTED, net_buf);
                 break;
             }

             if (ftp_session->CtrlState == FTPs_STATE_GOTREST) {
                 fs_err = FS_FSeek(p_file, ftp_session->DtpOffset, FS_SEEK_SET);
                 if (fs_err != 0) {
                     FS_FClose(p_file);
                     Str_FmtPrint((char *)net_buf, sizeof(net_buf), "551 Cannot seek file %s to offset %d.",
                                    ftp_session->CurEntry, ftp_session->DtpOffset);
                     FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_ACTIONABORTED, net_buf);
                     break;
                 }
             }

             while (DEF_TRUE) {
                 fs_len = FS_FRead(net_buf, 1, sizeof(net_buf), p_file);
                 fs_err = FS_FError(p_file);
                 if (fs_len == 0) {
                     if (fs_err != FS_ERR_EOF) {
                         FTPs_TRACE_DBG(("FTPs FS_FRead() failed: error #%d, line #%d.\n", fs_err, __LINE__));
                     }
                     break;
                 }

                 FTPs_Tx(ftp_session->DtpSockID, net_buf, fs_len, &net_err);
                 if (net_err != NET_SOCK_ERR_NONE) {
                     FTPs_TRACE_DBG(("FTPs FTPs_Tx() failed: error #%d, line #%d.\n", net_err, __LINE__));
                     break;
                 }
                 if (fs_len != sizeof(net_buf)) {
                     break;
                 }
             }
             FS_FClose(p_file);

             if ((net_err == NET_SOCK_ERR_NONE) &&
                 ((fs_err == FS_ERR_OK) ||
                  (fs_err == FS_ERR_EOF))) {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CLOSINGSUCCESS, (CPU_CHAR *)0);
             } else {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CLOSEDCONNABORT, (CPU_CHAR *)0);
             }
             break;

        case FTP_CMD_STOR:
        case FTP_CMD_APPE:
             if ((ftp_session->CtrlCmd   == FTP_CMD_STOR) &&
                 (ftp_session->CtrlState != FTPs_STATE_GOTREST)) {
                 p_file = FS_FOpen((char *)ftp_session->CurEntry, "wb");
             } else {
                 p_file = FS_FOpen((char *)ftp_session->CurEntry, "ab");
             }

             if (p_file == (FS_FILE *)0) {
                 Str_FmtPrint((char *)net_buf, sizeof(net_buf), "551 Cannot open %s: access denied.", ftp_session->CurEntry);
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_ACTIONABORTED, net_buf);
                 break;
             }

             if (ftp_session->CtrlState == FTPs_STATE_GOTREST) {
                 fs_err = FS_FSeek(p_file, ftp_session->DtpOffset, FS_SEEK_SET);
                 if (fs_err != 0) {
                     FS_FClose(p_file);
                     Str_FmtPrint((char *)net_buf, sizeof(net_buf), "551 Cannot seek file %s to offset %d.",
                                    ftp_session->CurEntry, ftp_session->DtpOffset);
                     FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_ACTIONABORTED, net_buf);
                     break;
                 }
             }

             NetOS_TCP_RxQ_TimeoutSet(ftp_session->DtpSockID, FTPs_CFG_DTP_MAX_RX_TIMEOUT_S * DEF_TIME_NBR_mS_PER_SEC, &net_err);
             while (DEF_TRUE) {
                 net_len = NetSock_RxData(ftp_session->DtpSockID, net_buf, sizeof(net_buf), NET_SOCK_FLAG_NONE, &net_err);
                 if ((net_err != NET_SOCK_ERR_NONE) &&
                     (net_err != NET_SOCK_ERR_RX_Q_CLOSED) &&
                     (net_err != NET_SOCK_ERR_RX_Q_EMPTY)) {
                     FTPs_TRACE_DBG(("FTPs NetSock_RxData() failed: error #%d, line #%d.\n", net_err, __LINE__));
                     break;
                 }

                                                            /* In this case, a timeout represent an end-of-file     */
                                                            /* condition.                                           */
                 if ((net_err == NET_SOCK_ERR_RX_Q_CLOSED) ||
                     (net_err == NET_SOCK_ERR_RX_Q_EMPTY)) {
                     break;
                 }
                 fs_len = FS_FWrite(net_buf, 1, net_len, p_file);
                 fs_err = FS_FError(p_file);
                 if (fs_len != net_len) {
                     if (fs_err == FS_ERR_DISKFULL) {
                         FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NOSPACE, (CPU_CHAR *)0);
                     } else {
                         FTPs_TRACE_DBG(("FTPs FS_FWrite() failed: error #%d, line #%d.\n", fs_err, __LINE__));
                     }
                     break;
                 }
             }
             FS_FClose(p_file);

             if (((net_err == NET_SOCK_ERR_NONE) ||
                  (net_err == NET_SOCK_ERR_RX_Q_CLOSED) ||
                  (net_err == NET_SOCK_ERR_RX_Q_EMPTY)) &&
                  (fs_err  == FS_ERR_OK)) {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CLOSINGSUCCESS, (CPU_CHAR *)0);
             } else {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CLOSEDCONNABORT, (CPU_CHAR *)0);
             }
             break;

        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                         FTPs_ProcessCtrlCmd()
*
* Description : Core execution routines of the FTP commands.
*
* Argument(s) : ftp_session     structure that contains FTP session states and control data.
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_Ctrl_Task()
*********************************************************************************************************
*/

static  void  FTPs_ProcessCtrlCmd (FTPs_SESSION_STRUCT  *ftp_session)
{
    CPU_CHAR      full_abs_path[FTPs_MAX_PATH_LEN];
    CPU_CHAR      full_rel_path[FTPs_MAX_PATH_LEN];
    CPU_CHAR      parent_abs_path[FTPs_MAX_PATH_LEN];
    CPU_CHAR      cur_entry[FTPs_MAX_PATH_LEN];
    CPU_CHAR      ren_abs_path[FTPs_MAX_PATH_LEN];
    CPU_CHAR      ren_rel_path[FTPs_MAX_PATH_LEN];

    CPU_CHAR      net_buf[FTPs_NET_BUF_LEN];
    CPU_CHAR     *p_cmd_arg;
    CPU_CHAR     *p_file_time;

    CPU_CHAR     *p_addr;
    CPU_CHAR     *p_port;
    FS_DIR       *p_dir;
    FS_DIRENT    *p_dirent;
    FS_FILETIME   file_time;
    FS_U32        time_stamp;

    CPU_INT32U    cmp_val;
    CPU_BOOLEAN   rtn_val;
    CPU_INT32U    fs_rtn_val;
    CPU_INT32U    i;


    p_dir    = (FS_DIR    *)0;
    p_dirent = (FS_DIRENT *)0;

                                                                /* Execute the command.                                 */
    switch (ftp_session->CtrlCmd) {
                                                                /* NOOP:   No operation (keep-alive).                   */
                                                                /* Syntax: NOOP                                         */
        case FTP_CMD_NOOP:
             FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_OKAY, (CPU_CHAR *)0);
             break;

                                                                /* QUIT:   Terminate the FTP session.                   */
                                                                /* Syntax: QUIT                                         */
        case FTP_CMD_QUIT:
             FTPs_StopPasvMode(ftp_session);
             FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_SERVERCLOSING, (CPU_CHAR *)0);
             break;

                                                                /* REIN:   Reinitialize the FTP session.                */
                                                                /* Syntax: REIN                                         */
        case FTP_CMD_REIN:
             FTPs_StopPasvMode(ftp_session);
             ftp_session->CtrlState = FTPs_STATE_LOGOUT;
             ftp_session->DtpMode   = FTP_MODE_STREAM;
             ftp_session->DtpType   = FTP_TYPE_ASCII;
             ftp_session->DtpForm   = FTP_FORM_NONPRINT;
             ftp_session->DtpStru   = FTP_STRU_FILE;
             ftp_session->DtpCmd    = FTP_CMD_NOOP;
             FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_OKAY, (CPU_CHAR *)0);
             break;

                                                                /* SYST:   Get system name.                             */
                                                                /* Syntax: SYST                                         */
        case FTP_CMD_SYST:
             FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_SYSTEMTYPE, (CPU_CHAR *)0);
             break;

                                                                /* FEAT:   Advertise server features.                   */
                                                                /* Syntax: FEAT                                         */
        case FTP_CMD_FEAT:
             FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_SYSTEMSTATUS, (CPU_CHAR *)0);
             break;

                                                                /* HELP:   Advertise server help.                       */
                                                                /* Syntax: HELP                                         */
        case FTP_CMD_HELP:
             FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_HELPMESSAGE, (CPU_CHAR *)0);
             break;

                                                                /* USER:   Set username.                                */
                                                                /* Syntax: USER <username>                              */
        case FTP_CMD_USER:
             p_cmd_arg = FTPs_FindArg(&ftp_session->CtrlCmdArgs);
             if (*p_cmd_arg != (CPU_CHAR)0) {
                 Str_Copy_N(ftp_session->User, p_cmd_arg, sizeof(ftp_session->User));
                 ftp_session->CtrlState = FTPs_STATE_GOTUSER;
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NEEDPASSWORD, (CPU_CHAR *)0);
             } else {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PARMSYNTAXERR, (CPU_CHAR *)0);
             }
             break;

                                                                /* PASS:   Enter password.                              */
                                                                /* Syntax: PASS <password>                              */
        case FTP_CMD_PASS:
             p_cmd_arg = FTPs_FindArg(&ftp_session->CtrlCmdArgs);
             if (*p_cmd_arg != (CPU_CHAR)0) {
                 Str_Copy_N(ftp_session->Pass, p_cmd_arg, sizeof(ftp_session->Pass));
                 rtn_val = FTPs_AuthUser(ftp_session);
                 if (rtn_val == DEF_OK) {
                     ftp_session->CtrlState = FTPs_STATE_LOGIN;
                     FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_LOGGEDIN, (CPU_CHAR *)0);
                 } else {
                     ftp_session->CtrlState = FTPs_STATE_LOGOUT;
                     FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NOTLOGGEDIN, (CPU_CHAR *)0);
                 }
             } else {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PARMSYNTAXERR, (CPU_CHAR *)0);
             }
             break;

                                                                /* MODE:   Set transfer mode (stream, block, compress). */
                                                                /* Syntax: MODE <S|B|C>                                 */
                                                                /* NOTE:   Server supports STREAM mode only.            */
        case FTP_CMD_MODE:
             p_cmd_arg = FTPs_FindArg(&ftp_session->CtrlCmdArgs);
             switch (*p_cmd_arg) {
                 case FTP_MODE_STREAM:
                      ftp_session->DtpMode = FTP_MODE_STREAM;
                      FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_OKAY, (CPU_CHAR *)0);
                      break;

                 case FTP_MODE_BLOCK:
                 case FTP_MODE_COMPRESSED:
                      FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PARMNOSUPPORT, (CPU_CHAR *)0);
                      break;

                 default:
                      FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PARMSYNTAXERR, (CPU_CHAR *)0);
                      break;
             }
             break;

                                                                /* TYPE:   Set data representation type.                */
                                                                /* Syntax: TYPE <A|E|I|L> [<N|T|C|bytesize>]            */
                                                                /* NOTE:   Server supports ASCII NON_PRINT or IMAGE.    */
        case FTP_CMD_TYPE:
             p_cmd_arg = FTPs_FindArg(&ftp_session->CtrlCmdArgs);
             switch (*p_cmd_arg) {
                 case FTP_TYPE_ASCII:
                      p_cmd_arg = FTPs_FindArg(&ftp_session->CtrlCmdArgs);
                      switch (*p_cmd_arg) {
                          case (CPU_CHAR)0:
                          case FTP_FORM_NONPRINT:
                               ftp_session->DtpType = FTP_TYPE_ASCII;
                               ftp_session->DtpForm = FTP_FORM_NONPRINT;
                               FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_OKAY, (CPU_CHAR *)0);
                               break;

                          case FTP_FORM_TELNET:
                          case FTP_FORM_CARGCTRL:
                               FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PARMNOSUPPORT, (CPU_CHAR *)0);
                               break;

                          default:
                               FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PARMSYNTAXERR, (CPU_CHAR *)0);
                               break;
                      }
                      break;

                 case FTP_TYPE_IMAGE:
                      ftp_session->DtpType = FTP_TYPE_IMAGE;
                      FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_OKAY, (CPU_CHAR *)0);
                      break;

                 case FTP_TYPE_LOCAL:
                 case FTP_TYPE_EBCDIC:
                      FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PARMNOSUPPORT, (CPU_CHAR *)0);
                      break;

                 default:
                      FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PARMSYNTAXERR, (CPU_CHAR *)0);
                      break;
             }
             break;

                                                                /* STRU:   Set file structure (file, record, page).     */
                                                                /* Syntax: STRU <F|R|P>                                 */
                                                                /* NOTE:   Server supports FILE structure only.         */
        case FTP_CMD_STRU:
             p_cmd_arg = FTPs_FindArg(&ftp_session->CtrlCmdArgs);
             switch (*p_cmd_arg) {
                 case FTP_STRU_FILE:
                      ftp_session->DtpStru = FTP_STRU_FILE;
                      FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_OKAY, (CPU_CHAR *)0);
                      break;

                 case FTP_STRU_RECORD:
                 case FTP_STRU_PAGE:
                      FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PARMNOSUPPORT, (CPU_CHAR *)0);
                      break;

                 default:
                      FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PARMSYNTAXERR, (CPU_CHAR *)0);
                      break;
             }
             break;

                                                                /* PASV:   Enter passive mode.                          */
                                                                /* Syntax: PASV                                         */
        case FTP_CMD_PASV:
             ftp_session->DtpSockAddr.Addr = NET_UTIL_HOST_TO_NET_32(INADDR_ANY);
             ftp_session->DtpSockAddr.Port = NET_UTIL_HOST_TO_NET_16(FTPs_CFG_DTP_IPPORT);
             FTPs_StartPasvMode(ftp_session);
             if (ftp_session->DtpPasv == DEF_YES) {
                 p_addr = (CPU_CHAR *)&FTPs_PublicAddr;
                 p_port = (CPU_CHAR *)&FTPs_PublicPort;

                 Str_FmtPrint((char *)net_buf, sizeof(net_buf),
                               (char *)FTPs_Reply[FTP_REPLY_ENTERPASVMODE].ReplyStr,
                                p_addr[0], p_addr[1], p_addr[2], p_addr[3],
                                p_port[0], p_port[1]);
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_ENTERPASVMODE, net_buf);
             } else {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CANTOPENDATA, (CPU_CHAR *)0);
             }
             break;

                                                                /* PORT:   Set data port configuration.                 */
                                                                /* Syntax: PORT <ip0, ip1, ip2, ip3, port0, port1>      */
        case FTP_CMD_PORT:
             FTPs_StopPasvMode(ftp_session);
             if (ftp_session->DtpPasv == DEF_NO) {
                 p_addr = (CPU_CHAR *)&ftp_session->DtpSockAddr.Addr;
                 p_port = (CPU_CHAR *)&ftp_session->DtpSockAddr.Port;

                 for (i = 0; i <= 3; i++) {
                     cmp_val = Str_IsDigit(*ftp_session->CtrlCmdArgs);
                     while ((cmp_val == 0) && (ftp_session->CtrlCmdArgs != (CPU_CHAR *)0)) {
                         ftp_session->CtrlCmdArgs++;
                         cmp_val = Str_IsDigit(*ftp_session->CtrlCmdArgs);
                     }
                     p_addr[i] = Str_ToLong(ftp_session->CtrlCmdArgs, &ftp_session->CtrlCmdArgs, 10);
                 }
                 for (i = 0; i <= 1; i++) {
                     cmp_val = Str_IsDigit(*ftp_session->CtrlCmdArgs);
                     while ((cmp_val == 0) && (ftp_session->CtrlCmdArgs != 0)) {
                         ftp_session->CtrlCmdArgs++;
                         cmp_val = Str_IsDigit(*ftp_session->CtrlCmdArgs);
                     }
                     p_port[i] = Str_ToLong(ftp_session->CtrlCmdArgs, &ftp_session->CtrlCmdArgs, 10);
                 }

                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_OKAY, (CPU_CHAR *)0);
             } else {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CANTOPENDATA, (CPU_CHAR *)0);
             }
             break;

                                                                /* REST:   Next transfert will start at offset <offset>.*/
                                                                /* Syntax: REST <offset>                                */
        case FTP_CMD_REST:
             p_cmd_arg = FTPs_FindArg(&ftp_session->CtrlCmdArgs);
             if (*p_cmd_arg != (CPU_CHAR)0) {
                 ftp_session->DtpOffset = Str_ToLong(p_cmd_arg, 0, 10);
                 ftp_session->CtrlState = FTPs_STATE_GOTREST;
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NEEDMOREINFO, (CPU_CHAR *)0);
             }
             break;

                                                                /* PWD:    Get present working directry.                */
                                                                /* Syntax: PWD                                          */

                                                                /* CWD:    Change working directory.                    */
                                                                /* Syntax: CWD <dirname>                                */

                                                                /* CDUP:   Change to parent directory.                  */
                                                                /* Syntax: CDUP <filename>                              */

                                                                /* MKD:    Create directory.                            */
                                                                /* Syntax: MKD <dirname>                                */

                                                                /* RMD:    Delete directory.                            */
                                                                /* Syntax: RMD <dirname>                                */

                                                                /* NLST:   Get brief directory entries list.            */
                                                                /* Syntax: NLST [<pathname>]                            */

                                                                /* LIST:   Get detailed directory entries list.         */
                                                                /* Syntax: LIST [<pathname>]                            */

                                                                /* RETR:   Retrieve file.                               */
                                                                /* Syntax: RETR <filename>                              */

                                                                /* STOR:   Store file.                                  */
                                                                /* Syntax: STOR <filename>                              */

                                                                /* APPE:   Apped file.                                  */
                                                                /* Syntax: APPE <filename>                              */

                                                                /* DELE:   Delete file.                                 */
                                                                /* Syntax: DELE <filename>                              */

                                                                /* RNFR:   Rename file from name.                       */
                                                                /* Syntax: RNFR <filename>                              */

                                                                /* RNTO:   Rename file to name.                         */
                                                                /* Syntax: RNTO <filename>                              */

                                                                /* SIZE:   Get file size.                               */
                                                                /* Syntax: SIZE <filename>                              */

                                                                /* MDTM:   Get file modification date/time.             */
                                                                /* Syntax: MDTM <filename>                              */
        case FTP_CMD_PWD:
        case FTP_CMD_CWD:
        case FTP_CMD_CDUP:
        case FTP_CMD_MKD:
        case FTP_CMD_RMD:
        case FTP_CMD_NLST:
        case FTP_CMD_LIST:
        case FTP_CMD_RETR:
        case FTP_CMD_STOR:
        case FTP_CMD_APPE:
        case FTP_CMD_DELE:
        case FTP_CMD_RNFR:
        case FTP_CMD_RNTO:
        case FTP_CMD_SIZE:
        case FTP_CMD_MDTM:
                                                                /* Parameter handling.                                  */
             if (ftp_session->CtrlCmd == FTP_CMD_PWD) {
                 p_cmd_arg = (CPU_CHAR *)".";
             } else if (ftp_session->CtrlCmd == FTP_CMD_CDUP) {
                 p_cmd_arg = (CPU_CHAR *)"..";
             } else if (ftp_session->CtrlCmd == FTP_CMD_MDTM) {
                                                                /* MDTM command may have one or two arguments           */
                                                                /* 1. If the first argument are a file name, return     */
                                                                /*    file date and time.                               */
                                                                /* 2. If the first argument are a date and time and the */
                                                                /*    second argument are a file name, modify the file  */
                                                                /*    date and time.                                    */
                 p_cmd_arg   = FTPs_FindFileName(&ftp_session->CtrlCmdArgs);
                 p_file_time = FTPs_FindArg(&ftp_session->CtrlCmdArgs);

                                                                /* Check if first argument is a string of 14 digits.    */
                                                                /* The date and time string has exactly 14 digits.      */
                 cmp_val = Str_Len(p_file_time);
                 if (cmp_val == 14) {
                    for (i = 0; i < 14; i++) {
                        cmp_val = Str_IsDigit(p_file_time[i]);
                        if (cmp_val == 0) {
                            break;
                        }
                    }
                    if (i == 14) {
                                                                /* Yes: The file name starts after.                     */
                                                                /* No:  The file name IS the first argument.            */
                        p_cmd_arg = FTPs_FindFileName(&ftp_session->CtrlCmdArgs);
                    } else {
                        p_file_time = "";
                    }
                 } else {
                    p_file_time = "";
                 }
             } else {
                 p_cmd_arg = FTPs_FindFileName(&ftp_session->CtrlCmdArgs);
             }

                                                                /* Skip "-xxxx" argument.                               */
             if (*p_cmd_arg == '-') {
                 p_cmd_arg = (CPU_CHAR *)".";
             }

             rtn_val = FTPs_BuildPath(full_abs_path,   sizeof(full_abs_path),
                                      full_rel_path,   sizeof(full_rel_path),
                                      parent_abs_path, sizeof(parent_abs_path),
                                      cur_entry,       sizeof(cur_entry),
                                      ftp_session->BasePath,
                                      ftp_session->RelPath,
                                      p_cmd_arg);

             FTPs_ToFSStylePath(full_abs_path);
             FTPs_ToFSStylePath(parent_abs_path);

                                                                /* Verify presence of CurEntry/directory/parent         */
                                                                /* directory.                                           */
             if (rtn_val == DEF_OK) {
                 switch (ftp_session->CtrlCmd) {
                     case FTP_CMD_PWD:
                     case FTP_CMD_CWD:
                     case FTP_CMD_CDUP:
                     case FTP_CMD_RMD:
                     case FTP_CMD_NLST:
                     case FTP_CMD_LIST:
                          p_dir = FS_OpenDir((char *)full_abs_path);
                          if (p_dir == (FS_DIR *)0) {
                              rtn_val = DEF_FAIL;
                          } else {
                              rtn_val = DEF_OK;
                          }
                          FS_CloseDir(p_dir);
                          break;

                     case FTP_CMD_MKD:
                     case FTP_CMD_RETR:
                     case FTP_CMD_STOR:
                     case FTP_CMD_APPE:
                     case FTP_CMD_DELE:
                     case FTP_CMD_RNFR:
                     case FTP_CMD_RNTO:
                     case FTP_CMD_SIZE:
                     case FTP_CMD_MDTM:
                          p_dir = FS_OpenDir((char *)parent_abs_path);
                          if (p_dir == (FS_DIR *)0) {
                              rtn_val = DEF_FAIL;
                          } else {
                              p_dirent = FS_ReadDir(p_dir);
                              while (p_dirent != (FS_DIRENT *)0) {
                                  cmp_val = Str_Cmp((CPU_CHAR *)p_dirent->DirName, cur_entry);
                                  if (cmp_val == 0) {
                                      break;
                                  }
                                  p_dirent = FS_ReadDir(p_dir);
                              }
                              if (p_dirent == (FS_DIRENT *)0) {
                                  switch (ftp_session->CtrlCmd) {
                                      case FTP_CMD_MKD:
                                      case FTP_CMD_STOR:
                                      case FTP_CMD_APPE:
                                      case FTP_CMD_RNTO:
                                           rtn_val = DEF_OK;
                                           break;

                                      default:
                                           rtn_val = DEF_FAIL;
                                           break;
                                  }
                              } else {
                                  switch (ftp_session->CtrlCmd) {
                                      case FTP_CMD_MKD:
                                      case FTP_CMD_RNTO:
                                           rtn_val = DEF_FAIL;
                                           break;

                                      default:
                                           rtn_val = DEF_OK;
                                           break;
                                  }
                              }
                          }
                          FS_CloseDir(p_dir);
                          break;

                     default:
                          break;
                 }

                                                                /* Do action.                                           */
                 if (rtn_val == DEF_OK) {
                     switch (ftp_session->CtrlCmd) {
                         case FTP_CMD_PWD:
                              Str_FmtPrint((char *)net_buf, sizeof(net_buf),
                                            "257 \"%s\" is current directory.",
                                             full_rel_path);
                              FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PATHNAME, net_buf);
                              break;

                         case FTP_CMD_CWD:
                         case FTP_CMD_CDUP:
                              Str_Copy(ftp_session->RelPath, full_rel_path);
                              FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_ACTIONCOMPLETE, (CPU_CHAR *)0);
                              break;

                         case FTP_CMD_NLST:
                         case FTP_CMD_LIST:
                         case FTP_CMD_RETR:
                         case FTP_CMD_STOR:
                         case FTP_CMD_APPE:
                              FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_OKAYOPENING, (CPU_CHAR *)0);
                              ftp_session->DtpCmd = ftp_session->CtrlCmd;
                              Str_Copy(ftp_session->CurEntry, full_abs_path);
                              FTPs_Dtp_Task((void *)ftp_session);
                              ftp_session->DtpOffset = 0;
                              ftp_session->CtrlState = FTPs_STATE_LOGIN;
                              break;

                         case FTP_CMD_MKD:
                              fs_rtn_val = FS_MkDir((char *)full_abs_path);
                              if (fs_rtn_val == 0) {
                                  Str_FmtPrint((char *)net_buf, sizeof(net_buf),
                                                (char *)FTPs_Reply[FTP_REPLY_PATHNAME].ReplyStr,
                                                 full_rel_path);
                                  FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_PATHNAME, net_buf);
                              } else {
                                  Str_FmtPrint((char *)net_buf, sizeof(net_buf),
                                                (char *)FTPs_Reply[FTP_REPLY_NOTFOUND].ReplyStr,
                                                 full_rel_path);
                                  FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NOTFOUND, net_buf);
                              }
                              break;

                         case FTP_CMD_RMD:
                              fs_rtn_val = FS_RmDir((char *)full_abs_path);
                              if (fs_rtn_val == 0) {
                                  FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_ACTIONCOMPLETE, (CPU_CHAR *)0);
                              } else {
                                  Str_FmtPrint((char *)net_buf, sizeof(net_buf),
                                                (char *)FTPs_Reply[FTP_REPLY_NOTFOUND].ReplyStr,
                                                 full_rel_path);
                                  FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NOTFOUND, net_buf);
                              }
                              break;

                         case FTP_CMD_DELE:
                              fs_rtn_val = FS_Remove((char *)full_abs_path);
                              if (fs_rtn_val == 0) {
                                  FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_ACTIONCOMPLETE, (CPU_CHAR *)0);
                              } else {
                                  Str_FmtPrint((char *)net_buf, sizeof(net_buf),
                                                (char *)FTPs_Reply[FTP_REPLY_NOTFOUND].ReplyStr,
                                                 full_rel_path);
                                  FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NOTFOUND, net_buf);
                              }
                              break;

                         case FTP_CMD_RNFR:
                              Str_Copy(ren_abs_path, full_abs_path);
                              Str_Copy(ren_rel_path, full_rel_path);
                              ftp_session->CtrlState = FTPs_STATE_GOTRNFR;
                              FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NEEDMOREINFO, (CPU_CHAR *)0);
                              break;

                         case FTP_CMD_RNTO:
                              fs_rtn_val = FS_Move((char *)ren_abs_path, (char *)full_abs_path);
                              ftp_session->CtrlState = FTPs_STATE_LOGIN;
                              if (fs_rtn_val == 0) {
                                  FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_ACTIONCOMPLETE, (CPU_CHAR *)0);
                              } else {
                                  Str_FmtPrint((char *)net_buf, sizeof(net_buf),
                                                (char *)FTPs_Reply[FTP_REPLY_NOTFOUND].ReplyStr,
                                                 ren_rel_path);
                                  FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NOTFOUND, net_buf);
                              }
                              break;

                         case FTP_CMD_SIZE:
                              Str_FmtPrint((char *)net_buf, sizeof(net_buf), "213 %d", p_dirent->Size);
                              FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_FILESTATUS, net_buf);
                              break;

                         case FTP_CMD_MDTM:
                              cmp_val = Str_Cmp(p_file_time, "");
                              if (cmp_val == 0) {
                                                                /* Return file date and time.                           */
                                  FS_TimeStampToFileTime(p_dirent->TimeStamp, &file_time);
                                  Str_FmtPrint((char *)net_buf, sizeof(net_buf), "213 %04hu%02hu%02hu%02hu%02hu%02hu",
                                                 file_time.Year,
                                                 file_time.Month,
                                                 file_time.Day,
                                                 file_time.Hour,
                                                 file_time.Minute,
                                                 file_time.Second);
                                  FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_FILESTATUS, net_buf);
                              } else {
                                                                /* Modify file date and time.                           */
                                  Str_FmtScan((char *)p_file_time, "%04hu%02hu%02hu%02hu%02hu%02hu",
                                               &file_time.Year,
                                               &file_time.Month,
                                               &file_time.Day,
                                               &file_time.Hour,
                                               &file_time.Minute,
                                               &file_time.Second);
                                  FS_FileTimeToTimeStamp(&file_time, &time_stamp);
                                  FS_SetFileTime((char *)full_abs_path, time_stamp);
                                  FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_ACTIONCOMPLETE, (CPU_CHAR *)0);
                              }
                              break;

                         default:
                            break;
                     }
                 } else {
                     Str_FmtPrint((char *)net_buf, sizeof(net_buf),
                                   (char *)FTPs_Reply[FTP_REPLY_NOTFOUND].ReplyStr,
                                    full_rel_path);
                     FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NOTFOUND, net_buf);
                 }
             } else {
                 FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_NAMEERR, (CPU_CHAR *)0);
             }
             break;

        default:
             FTPs_SendReply(ftp_session->CtrlSockID, FTP_REPLY_CMDNOSUPPORT, (CPU_CHAR *)0);
             break;
    }
}


/*
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            FTPs_Dtp_Task()
*
* Description : FTP data transfer task
*
* Argument(s) : p_arg       argument passed to the task (cast to control socket ID).
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_Ctrl_OS_Task().
*
* Note        : This task implements the "DTP" (Data Transfer Process) as described in RFC 959.  The
*               means by which the connection is established with the client depends on whether or not
*               the DTP is to be passive or not.
*********************************************************************************************************
*/

void  FTPs_Dtp_Task (void  *p_arg)
{
    FTPs_SESSION_STRUCT  *ftp_session;
    CPU_INT32S            pasv_sock_id;
    CPU_INT32S            dtp_sock_id;
    NET_SOCK_ADDR         client_addr;
    CPU_INT16S            client_addr_len;
    CPU_INT32U            retry_cnt;
    NET_ERR               net_err;


    ftp_session = (FTPs_SESSION_STRUCT *)p_arg;

    if (ftp_session->DtpPasv == DEF_YES) {
        pasv_sock_id = ftp_session->DtpPasvSockID;

        retry_cnt = 0;
        while (DEF_TRUE) {
                                                                /* When a client make a request, accept it and create a */
                                                                /* new socket for it.                                   */
            client_addr_len = sizeof(client_addr);

                                                                /* Wait on socket, accept with timeout.                 */
            FTPs_TRACE_INFO(("FTPs ACCEPT passive DTP socket.\n"));
            dtp_sock_id = NetSock_Accept( pasv_sock_id,
                                         &client_addr,
                                         &client_addr_len,
                                         &net_err);
            switch (net_err) {
                case NET_SOCK_ERR_NONE:
                     retry_cnt = 0;
                     break;

                case NET_ERR_INIT_INCOMPLETE:
                case NET_SOCK_ERR_NULL_PTR:
                case NET_SOCK_ERR_NONE_AVAIL:
                case NET_SOCK_ERR_CONN_ACCEPT_Q_NONE_AVAIL:
                case NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT:
                case NET_OS_ERR_LOCK:
                     retry_cnt++;
                     if (retry_cnt < FTPs_CFG_DTP_MAX_ACCEPT_RETRY) {
                         continue;                              /* Ignore transitory socket error.                      */
                     }
                     retry_cnt = 0;
                     break;                                     /* Close server socket on too many tansitory errors.    */

                case NET_SOCK_ERR_NOT_USED:
                case NET_SOCK_ERR_INVALID_SOCK:
                case NET_SOCK_ERR_INVALID_TYPE:
                case NET_SOCK_ERR_INVALID_FAMILY:
                case NET_SOCK_ERR_INVALID_STATE:
                case NET_SOCK_ERR_INVALID_OP:
                case NET_SOCK_ERR_CONN_FAIL:
                default:
                     retry_cnt = 0;
                     break;
            }

            if (net_err != NET_SOCK_ERR_NONE) {
                                                                /* Close server socket on fatal error.                  */
                FTPs_TRACE_DBG(("FTPs NetSock_Accept() failed: error #%d, line #%d.\n", net_err, __LINE__));
            }
            break;
        }
    } else {
                                                                /* Open a socket.                                       */
        FTPs_TRACE_INFO(("FTPs OPEN active DTP socket.\n"));
        dtp_sock_id = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                                    NET_SOCK_TYPE_STREAM,
                                    NET_SOCK_PROTOCOL_TCP,
                                   &net_err);
        if (net_err == NET_SOCK_ERR_NONE) {
            NetSock_Conn(dtp_sock_id,
                         (NET_SOCK_ADDR *)&ftp_session->DtpSockAddr,
                         sizeof(NET_SOCK_ADDR),
                         &net_err);
            if ((net_err != NET_SOCK_ERR_NONE) &&
                (net_err != NET_SOCK_ERR_RX_Q_EMPTY)) {
                FTPs_TRACE_DBG(("FTPs NetSock_Conn() failed: error #%d, line #%d.\n", net_err, __LINE__));
            }
            if (net_err == NET_SOCK_ERR_RX_Q_EMPTY) {
                FTPs_TRACE_DBG(("FTPs NetSock_Conn() timeout, line #%d.\n", __LINE__));
            }
        } else {
            FTPs_TRACE_DBG(("FTPs NetSock_Open() failed: error #%d, line #%d.\n", net_err, __LINE__));
        }
    }

    if (net_err == NET_SOCK_ERR_NONE) {
        ftp_session->DtpSockID = dtp_sock_id;
        FTPs_TRACE_INFO(("FTPs START transfer.\n"));
        FTPs_ProcessDtpCmd(ftp_session);
        FTPs_TRACE_INFO(("FTPs STOP transfer.\n"));
    }

    FTPs_TRACE_INFO(("FTPs CLOSE DTP socket.\n"));
    NetSock_Close(ftp_session->DtpSockID, &net_err);
}


/*
*********************************************************************************************************
*                                            FTPs_Ctrl_Task()
*
* Description : FTP control task
*
* Argument(s) : p_arg       argument passed to the task (cast to control socket ID).
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_Ctrl_OS_Task().
*
* Note        : This task manages the control connection of FTP.  The control connection is used for the
*               transfer of commands (which describes the functions to be performed) and the replies to
*               these commands.
*
*               Section 5.3.1 (FTP COMMANDS) of RFC959 lists the syntax of each ftp command that can be
*               sent by a client.
*
*               Section 5.1 (MINIMUM IMPLEMENTATION) of RFC959 lists the set of commands that must be
*               supported.
*
*               TYPE - ASCII Non-Print
*               MODE - Stream
*               STRUCTURE - File, Record
*               COMMANDS - USER, QUIT, PORT, TYPE, MODE, STRU, RETR, STOR, NOOP
*********************************************************************************************************
*/

void  FTPs_Ctrl_Task (void  *p_arg)
{
    FTPs_SESSION_STRUCT   ftp_session;

    CPU_CHAR              net_buf[FTPs_NET_BUF_LEN];
    CPU_INT32U            net_buf_len;
    CPU_CHAR             *p_net_buf;
    CPU_CHAR             *p_net_buf2;
    CPU_CHAR             *p_cmd;

    NET_ERR               net_err;
    CPU_INT32S            pkt_len;
    CPU_INT32U            cmp_val;
    CPU_INT32U            i;


    FTPs_CtrlTasks++;

    ftp_session.DtpSockAddr.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
    ftp_session.DtpSockAddr.Addr   = NET_UTIL_HOST_TO_NET_32(INADDR_ANY);
    ftp_session.DtpSockAddr.Port   = NET_UTIL_HOST_TO_NET_16(FTPs_CFG_DTP_IPPORT);

    ftp_session.CtrlSockID         = (CPU_INT32S)p_arg;
    ftp_session.CtrlState          = FTPs_STATE_LOGOUT;
    ftp_session.CtrlCmd            = FTP_CMD_NOOP;

    ftp_session.DtpSockID          = 0;
    ftp_session.DtpPasv            = DEF_NO;

                                                                /* Defaults specified in RFC959.                        */
    ftp_session.DtpMode            = FTP_MODE_STREAM;
    ftp_session.DtpType            = FTP_TYPE_ASCII;
    ftp_session.DtpForm            = FTP_FORM_NONPRINT;
    ftp_session.DtpStru            = FTP_STRU_FILE;

    ftp_session.DtpCmd             = FTP_CMD_NOOP;

    ftp_session.DtpOffset          = 0;

    FTPs_SendReply(ftp_session.CtrlSockID, FTP_REPLY_SERVERREADY, (CPU_CHAR *)0);

    while (DEF_TRUE) {
                                                                /* Receive data until NEWLINE and replace it by a NULL. */
        p_net_buf   = net_buf;
        net_buf_len = sizeof(net_buf);
        NetOS_TCP_RxQ_TimeoutSet( ftp_session.CtrlSockID,
                                  FTPs_CFG_CTRL_MAX_RX_TIMEOUT_S * DEF_TIME_NBR_mS_PER_SEC,
                                 &net_err);
        while (DEF_TRUE) {
            pkt_len = NetSock_RxData( ftp_session.CtrlSockID,
                                      p_net_buf,
                                      net_buf_len,
                                      NET_SOCK_FLAG_NONE,
                                     &net_err);
            if ((net_err != NET_SOCK_ERR_NONE) &&
                (net_err != NET_SOCK_ERR_RX_Q_EMPTY)) {
                FTPs_TRACE_DBG(("FTPs NetSock_RxData() failed: error #%d, line #%d.\n", net_err, __LINE__));
                break;
            }
            if (net_err == NET_SOCK_ERR_RX_Q_EMPTY) {
                FTPs_TRACE_DBG(("FTPs NetSock_RxData() timeout, line #%d.\n", __LINE__));
                break;
            }

            p_net_buf2 = (CPU_CHAR *)Str_Char_N(p_net_buf, pkt_len, '\n');
            if ( p_net_buf2  != (CPU_CHAR *)0) {
                *p_net_buf2   = (CPU_CHAR)0;
                 net_buf_len  = p_net_buf2 - net_buf;
                 break;
            }

            net_buf_len -= pkt_len;
            p_net_buf   += pkt_len;
        }

        if (net_err != NET_SOCK_ERR_NONE) {
            FTPs_StopPasvMode(&ftp_session);
            FTPs_SendReply(ftp_session.CtrlSockID, FTP_REPLY_SERVERCLOSING, (CPU_CHAR *)0);
            break;
        }

                                                                /* Process the received line.                           */
                                                                /*                                                      */
                                                                /* The line will be a command of the format             */
                                                                /* <COMMAND> [<ARG1>] [...]                             */
                                                                /*                                                      */
                                                                /* Where:                                               */
                                                                /* COMMAND is a 3 or 4 letter command.                  */
                                                                /* ARG1 is command specific.                            */
                                                                /* ARG2 is command specific.                            */
                                                                /* ...                                                  */

                                                                /* Find the command.                                    */
        FTPs_TRACE_INFO(("FTPs RX: %s\n", net_buf));

        p_net_buf = net_buf;
        p_cmd     = FTPs_FindArg(&p_net_buf);
        if (*p_cmd == (CPU_CHAR)0) {
            continue;
        }

                                                                /* Convert command to uppercase.                        */
        p_net_buf2 = p_cmd;
        while (*p_net_buf2 != (CPU_CHAR)0) {
            *p_net_buf2 = Str_ToUpper(*p_net_buf2);
             p_net_buf2++;
        }

                                                                /* Find the command code.                               */
        i = 0;
        while (FTPs_Cmd[i].CmdCode != FTP_CMD_MAX) {
            cmp_val = Str_Cmp(p_cmd, FTPs_Cmd[i].CmdStr);
            if (cmp_val == 0) {
                ftp_session.CtrlCmd = FTPs_Cmd[i].CmdCode;
                break;
            }
            i++;
        }
        if (FTPs_Cmd[i].CmdCode == FTP_CMD_MAX) {
            FTPs_SendReply(ftp_session.CtrlSockID, FTP_REPLY_CMDNOSUPPORT, (CPU_CHAR *)0);
            continue;
        }

                                                                /* Determine if the command entered is compatible with  */
                                                                /* the current state (context check).                   */
        if (FTPs_Cmd[ftp_session.CtrlCmd].CmdCntxt[ftp_session.CtrlState] == DEF_OFF) {
            if (ftp_session.CtrlState == FTPs_STATE_LOGOUT) {
                FTPs_SendReply(ftp_session.CtrlSockID, FTP_REPLY_NOTLOGGEDIN, (CPU_CHAR *)0);
            } else {
                FTPs_SendReply(ftp_session.CtrlSockID, FTP_REPLY_CMDBADSEQUENCE, (CPU_CHAR *)0);
                ftp_session.CtrlState = FTPs_STATE_LOGIN;
            }
            continue;
        }

        ftp_session.CtrlCmdArgs = p_net_buf;
        FTPs_ProcessCtrlCmd(&ftp_session);

        if (ftp_session.CtrlCmd == FTP_CMD_QUIT) {
            break;
        }
    }

    FTPs_TRACE_INFO(("FTPs CLOSE CTRL socket.\n"));
    NetSock_Close(ftp_session.CtrlSockID, &net_err);

    FTPs_TRACE_INFO(("FTPs DELETE CTRL task.\n"));
    FTPs_CtrlTasks--;
    FTPs_OS_TaskDelete();
}


/*
*********************************************************************************************************
*                                            FTPs_Srv_Task()
*
* Description : FTP server initialization
*
* Argument(s) : p_arg       argument passed to the task (ignored).
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_Srv_OS_Task().
*
* Note        : This task creates a socket on the appropriate IP port and listens.  If a connection
*               request is received and there is currently no other active connection, the control task
*               is started and interaction with the client begins.  If a connection request is received
*               and there is currently an active connection, then a reply code indicating this is sent,
*               and the client is denied access.
*********************************************************************************************************
*/

void  FTPs_Srv_Task (void  *p_arg)
{
    CPU_INT32S         srv_sock_id;
    CPU_INT32S         ctrl_sock_id;
    NET_SOCK_ADDR      server_addr;
    NET_SOCK_ADDR_IP  *p_server_addr;
    NET_SOCK_ADDR      client_addr;
    CPU_INT16S         client_addr_len;
    NET_ERR            net_err;
    CPU_BOOLEAN        rtn_val;


    (void)p_arg;

    while (DEF_TRUE) {
                                                                /* Open a socket.                                       */
        FTPs_TRACE_INFO(("FTPs OPEN SRV socket.\n"));
        srv_sock_id = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                                    NET_SOCK_TYPE_STREAM,
                                    NET_SOCK_PROTOCOL_TCP,
                                   &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
            FTPs_TRACE_DBG(("FTPs NetSock_Open() failed: error #%d, line #%d.\n", net_err, __LINE__));
            NetSock_Close(srv_sock_id, &net_err);
            FTPs_OS_TaskSuspend();
        }

                                                                /* Bind a local address so the clients can send to us.  */
        Mem_Set(&server_addr, (CPU_CHAR)0, sizeof(server_addr));
        p_server_addr         = (NET_SOCK_ADDR_IP *)&server_addr;
        p_server_addr->Family =  NET_SOCK_ADDR_FAMILY_IP_V4;
        p_server_addr->Addr   =  NET_UTIL_HOST_TO_NET_32(INADDR_ANY);
        p_server_addr->Port   =  NET_UTIL_HOST_TO_NET_16(FTPs_CFG_CTRL_IPPORT);

        NetSock_Bind( srv_sock_id,
                     &server_addr,
                      NET_SOCK_ADDR_SIZE,
                     &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
            FTPs_TRACE_DBG(("FTPs NetSock_Bind() failed: error #%d, line #%d.\n", net_err, __LINE__));
            NetSock_Close(srv_sock_id, &net_err);
            FTPs_OS_TaskSuspend();
        }

                                                                /* Listen to the socket for clients.                    */
        NetSock_Listen( srv_sock_id,
                        FTPs_CTRL_CONN_Q_SIZE,
                       &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
            FTPs_TRACE_DBG(("FTPs NetSock_Listen() failed: error #%d, line #%d.\n", net_err, __LINE__));
            NetSock_Close(srv_sock_id, &net_err);
            FTPs_OS_TaskSuspend();
        }

        while (DEF_TRUE) {
                                                                /* When a client make a request, accept it and create a */
                                                                /* new socket for it.                                   */
            client_addr_len = sizeof(client_addr);

                                                                /* Wait on socket, accept with timeout.                 */
            FTPs_TRACE_INFO(("FTPs ACCEPT CTRL socket.\n"));
            ctrl_sock_id = NetSock_Accept( srv_sock_id,
                                          &client_addr,
                                          &client_addr_len,
                                          &net_err);
            switch (net_err) {
                case NET_SOCK_ERR_NONE:
                     break;

                case NET_ERR_INIT_INCOMPLETE:
                case NET_SOCK_ERR_NULL_PTR:
                case NET_SOCK_ERR_NONE_AVAIL:
                case NET_SOCK_ERR_CONN_ACCEPT_Q_NONE_AVAIL:
                case NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT:
                case NET_OS_ERR_LOCK:
                     continue;                                  /* Ignore transitory socket error.                      */

                case NET_SOCK_ERR_NOT_USED:
                case NET_SOCK_ERR_INVALID_SOCK:
                case NET_SOCK_ERR_INVALID_TYPE:
                case NET_SOCK_ERR_INVALID_FAMILY:
                case NET_SOCK_ERR_INVALID_STATE:
                case NET_SOCK_ERR_INVALID_OP:
                case NET_SOCK_ERR_CONN_FAIL:
                default:
                     break;
            }

            if (net_err != NET_SOCK_ERR_NONE) {
                                                                /* Close server socket on fatal error.                  */
                FTPs_TRACE_DBG(("FTPs NetSock_Accept() failed: error #%d, line #%d.\n", net_err, __LINE__));
                NetSock_Close(srv_sock_id, &net_err);
                break;                                          /* Break accept loop, re-open socket.                   */
            }

                                                                /* If a control process is already active, then tell    */
                                                                /* this client that the service is not available now.   */
            if (FTPs_CtrlTasks >= FTPs_CTRL_TASKS_MAX) {
                FTPs_SendReply(ctrl_sock_id, FTP_REPLY_NOSERVICE, (CPU_CHAR *)0);
                NetSock_Close(ctrl_sock_id, &net_err);
                continue;
            }

                                                                /* Create a task for FTP session control and pass       */
                                                                /* socket ID as argument to the task.                   */
            FTPs_TRACE_INFO(("FTPs CREATE CTRL task.\n"));
            rtn_val = FTPs_Ctrl_OS_TaskCreate((void*) ctrl_sock_id);
            if (rtn_val == DEF_FAIL) {
                FTPs_SendReply(ctrl_sock_id, FTP_REPLY_NOSERVICE, (CPU_CHAR *)0);
                NetSock_Close(ctrl_sock_id, &net_err);
            }
        }

        if (net_err != NET_SOCK_ERR_NONE) {
            continue;                                           /* Re-open socket on accept errors.                     */
        }
    }
}


/*
*********************************************************************************************************
*                                              FTPs_Init()
*
* Description : FTP server initialization
*
* Argument(s) : public_addr     for passive mode, you have to provide your public IP address (i.e. the
*                               IP address use to reach you over the internet).  It may be your router's
*                               public IP address.
*               public_port     for passive mode, you have to provice the public port you have opened
*                               and routed to this host to the FTPs_DTP_IPPORT port.
*
* Return(s)   : DEF_YES:        task created.
*               DEF_NO:         task not created.
*
* Caller(s)   : Application code.
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPs_Init (NET_IP_ADDR   public_addr,
                        NET_PORT_NBR  public_port)
{
    CPU_BOOLEAN  rtn_val;


    FTPs_CtrlTasks  = 0;
    FTPs_PublicAddr = NET_UTIL_HOST_TO_NET_32(public_addr);
    FTPs_PublicPort = NET_UTIL_HOST_TO_NET_16(public_port);

    FTPs_TRACE_INFO(("FTPs CREATE SRV Task.\n"));
    rtn_val = FTPs_Srv_OS_TaskCreate((void *)0);
    if (rtn_val == DEF_FAIL) {
        FTPs_TRACE_DBG(("FTPs FTPs_Srv_OS_TaskCreate() failed.\n"));
    }

    return (rtn_val);
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
