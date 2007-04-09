/*
*********************************************************************************************************
*                                    APPLICATION SPECIFIC RTOS SETUP
*
*                             (c) Copyright 2005, Micrium, Inc., Weston, FL
*                                          All Rights Reserved
*
*                                          CONFIGURATION FILE
*
* File : app_os_cfg.h
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#define  LIB_STR_CFG_FP_EN                      DEF_ENABLED

#define  VIC_VECT_OS_TICK                          0
#define  VIC_VECT_NIC                              1

/*
*********************************************************************************************************
*                                        MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

#define  APP_TCPIP_EN                           DEF_ENABLED

#define  APP_CLK_EN                             DEF_DISABLED
#define  APP_DHCPc_EN                           DEF_ENABLED
#define  APP_DNSc_EN                            DEF_ENABLED
#define  APP_FS_EN                              DEF_ENABLED
#define  APP_FTPc_EN                            DEF_DISABLED
#define  APP_FTPs_EN                            DEF_DISABLED
#define  APP_HTTPs_EN                           DEF_ENABLED
#define  APP_POP3c_EN                           DEF_DISABLED
#define  APP_SMTPc_EN                           DEF_ENABLED
#define  APP_SNTPc_EN                           DEF_ENABLED
#define  APP_TFTPs_EN                           DEF_ENABLED
#define  APP_TTCP_EN                            DEF_ENABLED

/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

#define  CLK_OS_CFG_TASK_NAME                  "RealTime Clock"
#define  DHCPc_OS_CFG_TASK_NAME                "DHCP (Client)"
#define  FTPs_SRV_OS_CFG_TASK_NAME             "FTP (Server)"
#define  FTPs_CTRL_OS_CFG_TASK_NAME            "FTP (Control)"
#define  HTTPs_OS_CFG_TASK_NAME                "HTTP (Server)"
#define  TFTPs_OS_CFG_TASK_NAME                "TFTP (Server)"
#define  TTCP_OS_CFG_TASK_NAME                 "TTCP"

/*
*********************************************************************************************************
*                                           TASKS PRIORITIES
*********************************************************************************************************
*/

#define  APP_START_TASK_PRIO                       5

#define  TCP_RX_TASK_PRIO						   22
#define  TCP_TX_TASK_PRIO                          23

#define  NET_OS_CFG_IF_RX_TASK_PRIO               10
#define  NET_OS_CFG_TMR_TASK_PRIO                 11
#define  HTTPs_OS_CFG_TASK_PRIO                   12
#define  TFTPs_OS_TASK_PRIO                       16

#define  APP_1_TASK_PRIO                          18
#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)


/*
*********************************************************************************************************
*                                              STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_START_TASK_STK_SIZE                1024

#define  TCP_RX_TASK_STK_SIZE                   2048
#define  TCP_TX_TASK_STK_SIZE                   2048

#define  NET_OS_CFG_TMR_TASK_STK_SIZE           1024
#define  NET_OS_CFG_IF_RX_TASK_STK_SIZE         1024
#define  HTTPs_OS_CFG_TASK_STK_SIZE             2048
#define  TFTPs_OS_TASK_STK_SIZE                 1024

#define  APP_1_TASK_STK_SIZE                    1024


/*
*********************************************************************************************************
*                                                 DHCPc
*********************************************************************************************************
*/

                                                                /* DHCP server IP port. Default is 67.                  */
#define  DHCPc_CFG_IP_PORT_SERVER                         67
                                                                /* DHCP client IP port. Default is 68.                  */
#define  DHCPc_CFG_IP_PORT_CLIENT                         68

                                                                /* Maximum number of attempts to negotiate lease with   */
                                                                /* the DHCP server.                                     */
#define  DHCPc_CFG_MAX_REQ_LEASE_RETRY                     5

                                                                /* Once the DHCP server has assigned the client an      */
                                                                /* address, the later may perform a final check prior   */
                                                                /* to use this address in order to make sure it is not  */
                                                                /* being used by another host on the network.           */
#define  DHCPc_CFG_ADDR_VALIDATE_EN             DEF_ENABLED

                                                                /* Number of seconds to wait for a possible ARP reply   */
                                                                /* when performing the final check on the suggested     */
                                                                /* address.  The minimum is 1 second.                   */
#define  DHCPc_CFG_ADDR_VALIDATE_WAIT_TIME_S               3

                                                                /* The broadcast bit is used to instruct the server to  */
                                                                /* use broadcast packets instead of unicast.            */
                                                                /* Useful when the IP stack cannot receive unicast      */
                                                                /* packets when not fully configured.                   */
                                                                /* This is our case, so the value MUST be DEF_ENABLED.  */
#define  DHCPc_CFG_BROADCAST_BIT                DEF_ENABLED
#define  DHCP_CFG_BROADCAST_BIT                DEF_ON

                                                                /* Size of array used for the client identifier,        */
                                                                /* typically a 48-bit MAC/net address.                  */
                                                                /* The minimum size value is 2.                         */
#define  DHCPc_CFG_CLIENT_ID_SIZE               NET_IF_ADDR_SIZE

                                                                /* Size of array used for the vendor class identifier.  */
                                                                /* Set this value according to the argument length      */
                                                                /* passed to the DHCPc_SetVendorClassID function.       */
                                                                /* The minimum size value is 1.                         */
#define  DHCPc_CFG_VENDOR_CLASS_ID_SIZE                   11

                                                                /* Size of the array used for the requested parameter   */
                                                                /* list.  Defines the maximum number of parameters to   */
                                                                /* request when invoking DHCPc_SetParamReqList.         */
#define  DHCPc_CFG_PARAM_REQ_LIST_SIZE                    10


/*
*********************************************************************************************************
*                                              HTTPs
*********************************************************************************************************
*/

#define  HTTPs_CFG_IPPORT                                 80    /* HTTP server IP port. Default is 80.                  */

#define  HTTPs_CFG_MAX_ACCEPT_TIMEOUT_S                   -1    /* Maximum inactivity time (s) on ACCEPT.               */
#define  HTTPs_CFG_MAX_RX_TIMEOUT_S                       30    /* Maximum inactivity time (s) on RX.                   */
#define  HTTPs_CFG_MAX_TX_TIMEOUT_S                       30    /* Maximum inactivity time (s) on TX.                   */

#define  HTTPs_CFG_MAX_ACCEPT_RETRY                       -1    /* Maximum number of retries on ACCEPT.                 */
#define  HTTPs_CFG_MAX_RX_RETRY                            3    /* Maximum number of retries on RX.                     */
#define  HTTPs_CFG_MAX_TX_RETRY                            3    /* Maximum number of retries on TX.                     */

#define  HTTPs_CFG_FILE_RD_BUF_LEN                      1024    /* Length of buffer used to read file.                  */
#define  HTTPs_CFG_FILE_TX_BUF_LEN                      1024    /* Length of buffer used to send file.                  */

#define  HTTPs_CFG_TOK_PARSE_EN                 DEF_ENABLED     /* Enable / disable token parsing (${}).                */
                                                                /* Size of parse table.  Set to "1" if                  */
                                                                /* HTTPs_CFG_TOKEN_PARSE_EN is disabled in order to     */
#define  HTTPs_CFG_TOK_PARSE_TBL_SIZE                    256    /* minimize memory consumption.                         */

                                                                /* Root path for HTTP documents in filesystem.          */
#define  HTTPs_CFG_FS_ROOT                     "ide:"           /* Filesystem-specific symbols can be used.             */
#define  HTTPs_CFG_DFLT_FILE                   "index.htm"      /* Default file to load if no filename specified in URL.*/

                                                                /* Default HTML document returned when the requested    */
                                                                /* HTML document is not found (HTTP error #404).        */
#define  HTTPs_CFG_MAX_VAR_LEN                            80
#define  HTTPs_CFG_MAX_VAL_LEN                            80

#define  HTTPs_CFG_ERR_MSG_HTML_NOT_FOUND      "<HTML>\r\n" \
                                               "<BODY>\r\n" \
                                               "<HEAD><TITLE>SYSTEM ERROR</TITLE></HEAD>\r\n" \
                                               "<H1>NOT FOUND</H1>\r\n" \
                                               "The requested object does not exist on this server.\r\n" \
                                               "</BODY>\r\n" \
                                               "</HTML>\r\n"



