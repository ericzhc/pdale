/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                                 (c) Copyright 2004, Micrium, Weston, FL
*                                          All Rights Reserved
*
*                                            LogicPD LH79520
*                                              Sample code
* File : APP.C
* By   : Jean J. Labrosse
* 
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                             SERVEUR DE DONNEE UDP
*
* Filename      : app.c
* Version       : V1
* Programmer(s) : FB
*                 MEL
*				  JSC
*				  RL
*********************************************************************************************************
*
* Ce code implemente un serveur udp de donnée qui retourne les données reçues de 8 capteurs
* et il les envoit  vers un client dont l'adresse ip et le port udp sont définit via un 
* un page web de configuration. La page web est disponible en tapant l'adresse ip de la carte
* dans un navigateur. Le délais de transimission est aussi configurable via cette page. Un serveur
* tftp est diponible pour transféré les fichiers sur la carte flash. Les capteurs sont reliés sur les 
* broches 27,28,31,32,34,37,38,42 du connecteur J39 de la carte ZOOM. 
*
*********************************************************************************************************
*/

#include <includes.h>
#include "..\..\..\..\..\CPU\Sharp\LH79520\LH79520_iocon.h"
#include "..\..\..\..\..\CPU\Sharp\LH79520\LH79520_gpio.h"
#include <math.h>
/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

static  OS_STK  AppTaskStartStk[APP_START_TASK_STK_SIZE];
static  OS_STK  TCPRxStk[TCP_RX_TASK_STK_SIZE];
static  OS_STK  TCPTxStk[TCP_TX_TASK_STK_SIZE];

OS_FLAG_GRP* tcpFlag;
int curr_sock_id;
struct sockaddr_in MySockAddr;
int flag = 0;

void  UDP_SendData(char);
void  UDP_CloseSocket();
void  UDP_Initialize(char*, char*);


int rx_sock_id;
int tx_sock_id;
struct sockaddr_in MyRxSockAddr;
struct sockaddr_in MyTxSockAddr;



/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void    AppTaskStart(void *p_arg);

/*
*********************************************************************************************************
*                                             C ENTRY POINT
*********************************************************************************************************
*/

int  main (void)
{
    INT8U err;

    extern char _bss;
    extern char _ebss;

    BSP_IntDisAll();                            /* Disable ALL interrupts to the interrupt controller  */

    memset (&_bss, 0, &_ebss-&_bss);            /* Clear bss                                           */    
    BSP_SetStackPointers();                     /* Initialize the stacks                               */ 

    BSP_Init();                                     /* Initialize the BSP                              */


    DBG_PRINT("Initializing: uC/OS-II\n\r");

    OSInit();
                      /* Initialize uC/OS-II                                 */

                                                /* Create start task                                   */
    OSTaskCreateExt(AppTaskStart,
                    NULL,
                    (OS_STK *)&AppTaskStartStk[APP_START_TASK_STK_SIZE - 1],
                    APP_START_TASK_PRIO,
                    APP_START_TASK_PRIO,
                    (OS_STK *)&AppTaskStartStk[0],
                    APP_START_TASK_STK_SIZE,
                    NULL,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    OSTaskCreateExt(TCP_Tx,
                    NULL,
                    (OS_STK *)&TCPTxStk[TCP_TX_TASK_STK_SIZE - 1],
                    TCP_TX_TASK_PRIO,
                    TCP_TX_TASK_PRIO,
                    (OS_STK *)&TCPTxStk[0],
                    TCP_TX_TASK_STK_SIZE,
                    NULL,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    OSTaskCreateExt(TCP_Rx,
                    NULL,
                    (OS_STK *)&TCPRxStk[TCP_RX_TASK_STK_SIZE - 1],
                    TCP_RX_TASK_PRIO,
                    TCP_RX_TASK_PRIO,
                    (OS_STK *)&TCPRxStk[0],
                    TCP_RX_TASK_STK_SIZE,
                    NULL,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	tcpFlag = OSFlagCreate(0x00, &err);

                                                /* Assign names to created tasks                       */
#if OS_TASK_NAME_SIZE > 11                    
    OSTaskNameSet(APP_START_TASK_PRIO, "Start Task", &err);
#endif

    DBG_PRINT("Starting    : uC/OS-II\n\r");
    OSStart();                                  /* Start uC/OS-II                                      */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                              STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
* Arguments   : p_arg is the argument passed to 'AppStartTask()' by 'OSTaskCreate()'.
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*               2) Interrupts are enabled once the task start because the I-bit of the CCR register was
*                  set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
	INT8U err2;
	OSFlagPost(tcpFlag, TCP_INIT_DONE, OS_FLAG_CLR, &err2);
    NET_IP_ADDR  ip;
    NET_IP_ADDR  msk;
    NET_IP_ADDR  gw;

    NET_ERR      err;
                                    

    (void)p_arg;                                    /* Prevent compiler warning                        */

    DBG_PRINT("Initializing: Timers\n\r");
    Tmr_Init();                                     /* Start timers                                    */

#if OS_TASK_STAT_EN > 0
    OSStatInit();                                   /* Start stats task                                */
#endif  
  
    DBG_PRINT("Initializing: uC/TCP-IP\n\r");                                                    
    Net_Init();                                     /* Initialize uC/TCP-IP                            */
	FS_Init(); 
                                                    /* Set the target's IP address and mask            */
	CPU_INT08U param_req_list[] = {DHCP_OPT_HOST_NAME, DHCP_OPT_DOMAIN_NAME, DHCP_OPT_DOMAIN_NAME_SERVER, DHCP_OPT_TIME_SERVER, DHCP_OPT_TIME_OFFSET};
	
	DHCPc_SetMacAddr(NetIF_MAC_Addr);
	DHCPc_SetClientID(0x01, NetIF_MAC_Addr, NET_IF_ADDR_SIZE);
	DHCPc_SetVendorClassID("YourCompany", 11);
	DHCPc_SetParamRequestList(param_req_list, sizeof(param_req_list) / sizeof(param_req_list[0]));
	
	DHCPc_Start();
	DHCPc_CfgStack();

	OSFlagPost(tcpFlag, TCP_INIT_DONE, OS_FLAG_SET, &err2);
	while (1) {
		OSTimeDly(100);
	}
}


 /*$PAGE*/
/*
*********************************************************************************************************
* UDP_Initialize()
*
* Description : Cette fonction initialise le socket UDP
* Arguments : Ip
* Port
*********************************************************************************************************
*/
void UDP_Initialize(char *Ip, char *Port) 
{
	// Opens the UDP socket
	curr_sock_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Build the destination address
	MySockAddr.sin_family = AF_INET;
	MySockAddr.sin_port = htons(atoi(Port));
	MySockAddr.sin_addr.s_addr = inet_addr(Ip);

	connect(curr_sock_id, (struct sockaddr *)&MySockAddr, sizeof(struct sockaddr_in));
}

 /*$PAGE*/
/*
*********************************************************************************************************
* UDP_SendData()
*
* Description : Cette fonction envoit des données selon le protocole UDP
* Arguments : value
* 
*********************************************************************************************************
*/
void UDP_SendData(char value) 
{
	sendto(curr_sock_id, &value, sizeof(value), 0, (struct sockaddr *)&MySockAddr, sizeof(struct sockaddr_in));
}

/*$PAGE*/
/*
*********************************************************************************************************
* UDP_CloseSocket()
*
* Description : Cette fonction ferme le socket UDP
*
*********************************************************************************************************
*/

void UDP_CloseSocket() 
{
	close(curr_sock_id );
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      RouesEncodeuse_Convert()
*
* Description : Cette fonction sert a convertir les valeurs fournies par les roues codeuses du format
				binaire au format decimal. La valeur des registres GPIOB, GPIOC et GPIOD sont prealablement
				decalees de facon a ce que la fonction RouesEncodeuse_Convert() ne recoivent que des
				valeurs de 0 ou 1. L'argument position indique a quelle puissance de deux cette valeur
				binaire doit etre elevee. Prenons comme exemple le registre GPIOB qui contiendrait la
				valeur 0x1000000. Cette valeur sera decalee de 6 bits vers la droite avant d'etre passee
				a la fonction RouesEncodeuse_Convert(). Si cette valeur represente le 3e bit (bit 2) de la
				roue codeuse de droite, la valeur de l'argument postition sera 2 puisque la valeur 
				represente 1 x 2^2 en decimal.

* Arguments   : value
*               position
*********************************************************************************************************
*/
int RouesEncodeuse_Convert(int value, int position) {
	//int ret = 1;
	//if (position > 0 && value == 1) {
	//	int i=0;
	//	for (i=0; i<position; i++) {
	//		ret *= 2;
	//	}
	//}
	//if (value == 1) {
	//	return ret;
	//} else {
	//	return 0;
	//}
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetUDP_RxAppDataHandler()
*
* Description : This function is called by UDP when a packet is received.
* Arguments   : pbuf
*               src_addr
*               src_port
*               dest_addr
*               dest_port
*               perr
*********************************************************************************************************
*/

void   NetUDP_RxAppDataHandler (NET_BUF           *pbuf,
                                NET_IP_ADDR        src_addr,
                                NET_UDP_PORT_NBR   src_port,
                                NET_IP_ADDR        dest_addr,
                                NET_UDP_PORT_NBR   dest_port,
                                NET_ERR           *perr)
{
    (void)pbuf;
    (void)src_addr;
    (void)src_port;
    (void)dest_addr;
    (void)dest_port;
    (void)perr;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      HTTPs_ValRx()
*
* Description : This function handles an HTTP Post request.
* Arguments   : Variable
*               Val
*********************************************************************************************************
*/
CPU_BOOLEAN HTTPs_ValRx (CPU_CHAR *Variable,
						 CPU_CHAR *Val)
{
	//
	//if (Str_Cmp(Variable, "IP") == 0) {
	//	// Mise a jour de l'adresse IP
	//	strcpy(ServeurIP,Val);
	//	flag = 1;
	//
	//} else if (Str_Cmp(Variable, "PORT") == 0) {
	//	// Mise a jour du port UDP
	//	strcpy(ServeurPORT,Val);
	//	flag = 1;
	//
	//} else if (Str_Cmp(Variable, "DELAY" ) == 0) {
	//	// Mise a jour du delais de transmission des données
	//	strcpy(ServeurDELAY,Val);
	//}

	//return (DEF_OK);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      HTTPs_ValReq()
*
* Description : This function handles an HTTP Get request
* Arguments   : Variable
*               Val
*               MaxSize
*********************************************************************************************************
*/
CPU_BOOLEAN HTTPs_ValReq (CPU_CHAR *Variable,
						  CPU_CHAR **Val,
						  CPU_INT32U MaxSize)
{
	//// Permet de mettre a jour le formulaire avec les valeurs courantes

	//if (Str_Cmp(Variable, "IP") == 0) {
	//	*Val = ServeurIP;
	//} else if (Str_Cmp(Variable, "PORT") == 0) {
	//	*Val = ServeurPORT;
	//} else if (Str_Cmp(Variable, "DELAY") == 0) {
	//	*Val = ServeurDELAY;
	//}

	//return (DEF_OK);
}
