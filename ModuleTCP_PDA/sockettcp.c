#include "sockettcp.h"
#define DEBUG 1

/*
*********************************************************************************************************
*                                          GLOBALES VARIABLES
*********************************************************************************************************
*/

int rx_sock_id;
int tx_sock_id;
struct sockaddr_in MyRxSockAddr;
struct sockaddr_in MyTxSockAddr;


/*$PAGE*/
/*
*********************************************************************************************************
*                                              TCP_Rx TASK
*
* Description : This is the receiver tcp task used to receive messages from distant server.
*
*********************************************************************************************************
*/
void  TCP_Rx()
{
	OS_FLAGS flags;
	INT8U err;

	// Attendre que l'initalisation de uTCP/IP soit terminee
	flags = OSFlagPend(tcpFlag, 
			TCP_INIT_DONE, 
			OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
			0,
			&err);

	// Ouverture du socket qui servira a recevoir des donnees du serveur
	TCP_RxInitialize(ServeurIP, ServeurRxPORT);

	char RxBuf[100];
	int rx_data_len;
	while (1) {
		rx_data_len = recv(rx_sock_id, (void *)&RxBuf[0], 100, 0);
		if (rx_data_len > 0) { /* See if we received data */
			/* Process data received */
#if DEBUG
			DBG_PRINT("Data Received : %c\n\r", RxBuf[0]);
#endif
			int i;
			for (i=0; i < rx_data_len; i++) {
				#if DEBUG
				DBG_PRINT("%c", RxBuf[i]);
				#endif
			}
			#if DEBUG
			DBG_PRINT("\n\r");
			#endif

			SendMessage(TCP_DEST_MAP, RxBuf, rx_data_len);
		} 
		else if (rx_data_len == 0) { /* See if connection has been lost */
			close(rx_sock_id);
		} 
		else {
		/* Data not received */
	
			OSTimeDly(90);
		}
		OSTimeDly(10);
	}
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                              TCP_Tx TASK
*
* Description : This is the transmitter tcp task used to send messages to distant server.
*
*********************************************************************************************************
*/
void  TCP_Tx ()
{
	OS_FLAGS flags;
	INT8U err;

	// Attendre que l'initalisation de uTCP/IP soit terminee
	flags = OSFlagPend(tcpFlag, 
			TCP_INIT_DONE, 
			OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
			0,
			&err);

	// Ouverture du socket qui servira a transmettre des donnees au serveur
	TCP_TxInitialize(ServeurIP, ServeurTxPORT);
	
	while (1) {
		// Attendre qu'il y est des donnees a envoyer
		flags = OSFlagPend(tcpFlag, 
			TCP_DATA_READY_TO_SEND, 
			OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
			0,
			&err);
		int i = 0;
		while (i < validDataLength) {
			TCP_SendData(tcpTxBuffer[i]);
			i++;
		}
		OSFlagPost( tcpFlag, 
					TCP_DATA_SENT, 
					OS_FLAG_SET, 
					&err);

		OSTimeDly(10);
	}
}

 /*$PAGE*/
/*
*********************************************************************************************************
* TCP_RxInitialize()
*
* Description : Cette fonction initialise le socket UDP
* Arguments : Ip
* Port
*********************************************************************************************************
*/
void TCP_RxInitialize(char *Ip, char *Port) 
{
	// Opens the TCP socket
	rx_sock_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Build the destination address
	MyRxSockAddr.sin_family = AF_INET;
	MyRxSockAddr.sin_port = htons(atoi(Port));
	MyRxSockAddr.sin_addr.s_addr = inet_addr(Ip);

	connect(rx_sock_id, (struct sockaddr *)&MyRxSockAddr, sizeof(struct sockaddr_in));
}
 /*$PAGE*/
/*
*********************************************************************************************************
* TCP_TxInitialize()
*
* Description : Cette fonction initialise le socket TCP
* Arguments : Ip
* Port
*********************************************************************************************************
*/
void TCP_TxInitialize(char *Ip, char *Port) 
{
	// Semaphore to protect multiple entry in transmission
	SendMsgSem = OSSemCreate(1);
	// Semaphore to protect access to transmission buffer
	SendMsgBuffSem = OSSemCreate(1);

	// Opens the TCP socket
	tx_sock_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Build the destination address
	MyTxSockAddr.sin_family = AF_INET;
	MyTxSockAddr.sin_port = htons(atoi(Port));
	MyTxSockAddr.sin_addr.s_addr = inet_addr(Ip);

	connect(tx_sock_id, (struct sockaddr *)&MyTxSockAddr, sizeof(struct sockaddr_in));
}
 /*$PAGE*/
/*
*********************************************************************************************************
* TCP_SendData()
*
* Description : Cette fonction envoit des données selon le protocole UDP
* Arguments : value
* 
*********************************************************************************************************
*/
void TCP_SendData(char value) 
{
	DBG_PRINT("TCPSending : %c\n\r", value);
	sendto(tx_sock_id, &value, sizeof(value), 0, (struct sockaddr *)&MyTxSockAddr, sizeof(struct sockaddr_in));
}
/*$PAGE*/
/*
*********************************************************************************************************
* TCP_CloseSocket()
*
* Description : Cette fonction ferme le socket UDP
*
*********************************************************************************************************
*/

void TCP_CloseSocket(int curr_sock_id) 
{
	close(curr_sock_id );
}

/*$PAGE*/
/*
*********************************************************************************************************
* SendMessage()
*
* Description : Cette fonction envoie les donnees via le socket tcp au serveur distant
*
*********************************************************************************************************
*/
void SendMessage(int destinateur, char* message, int length) {
	INT8U err;
	OS_FLAGS flags;
	OSSemPend(SendMsgSem, 0, &err);		// protects dual entry in this function
	OSSemPend(SendMsgBuffSem, 0, &err); // protects the transmission buffer


	int dataLeft = length;
	while (dataLeft > TCP_TX_BUFFER_SIZE) {
		memcpy (tcpTxBuffer, message, TCP_TX_BUFFER_SIZE);
		validDataLength = TCP_TX_BUFFER_SIZE;

		OSFlagPost(tcpFlag, TCP_INIT_DONE, OS_FLAG_SET, &err);

		flags = OSFlagPend( tcpFlag, 
							TCP_DATA_SENT, 
							OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
							0,
							&err);

		dataLeft = dataLeft - TCP_TX_BUFFER_SIZE;
	} 

	memcpy (tcpTxBuffer, message, dataLeft);
	tcpTxBuffer[dataLeft] = '\0';
	validDataLength = dataLeft+1;
	OSFlagPost( tcpFlag, 
				TCP_DATA_READY_TO_SEND, 
				OS_FLAG_SET, 
				&err);
	flags = OSFlagPend( tcpFlag, 
						TCP_DATA_SENT, 
						OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
						0,
						&err);

	OSSemPost(SendMsgSem);
	OSSemPost(SendMsgBuffSem);
}
