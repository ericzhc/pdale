#ifndef SOCKETTCP_H
#define SOCKETTCP_H
#include <includes.h>

/*
*********************************************************************************************************
*                                          Definition des modules distants
*********************************************************************************************************
*/

#define TCP_DEST_GPS	0x01		// Envoie du message au module GPS 
#define TCP_DEST_MAP	0x02		// Envoie du message au module de generation de la map
#define TCP_DEST_COLIS	0x03		// Envoie du message au module des colis

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void	TCP_Rx (void);
void	TCP_Tx (void);
void	TCP_SendData(char);
void	TCP_CloseSocket(int);
void	TCP_RxInitialize(char*, char*);
void	TCP_TxInitialize(char*, char*);
void	SendMessage(int, char*, int);

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

static char ServeurIP[17] = "132.210.78.70";
static char ServeurRxPORT[5] = "2165";
static char ServeurTxPORT[5] = "2166";
static char ServeurDELAY[4] = "100";

#define TCP_TX_BUFFER_SIZE 1024
static char tcpTxBuffer[TCP_TX_BUFFER_SIZE];
static int  validDataLength = 0;

extern OS_FLAG_GRP* tcpFlag;

#define TCP_INIT_DONE				1
#define TCP_DATA_READY_TO_SEND		2
#define TCP_DATA_SENT				4

OS_EVENT* SendMsgSem;			// Semaphore to protect multiple entry in transmission
OS_EVENT* SendMsgBuffSem;		// Semaphore to protect access to transmission buffer

#endif

