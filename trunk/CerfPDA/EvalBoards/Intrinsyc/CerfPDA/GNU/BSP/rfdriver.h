#ifndef RFDRIVER_H
#define RFDRIVER_H

#define RFSERIAL_BUFF_SIZE 1024
#define TASK_RFSERIAL_SIZE 1024

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void RFDriverInit();
void ISR_Serial_RF();
void TransmitRfBuffer(char*);
void BufferRfTransmissionTask();
int open_socket(char* port,char* ipaddress);
int DonneeRecue(char* buffer, INT16U timeout);
void checkNetwork();
int cell_init();

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/
COM_BUFF_INFO GetTaskRxRfBuff();
//OS_EVENT* TxRfSerialSem;
OS_EVENT* TransmitRfFctSem;
OS_STK BufferRfTransmissionTaskStk[TASK_RFSERIAL_SIZE];

OS_FLAG_GRP* RfFlag; // Flag to announce data was received after issuing a command

#define TX_RFSERIAL_DATA_READY_TO_SEND		1
#define	TX_RFSERIAL_DATA_SENT				2
#define	TCP_TRANSFER_RECEIVED				4


// Circular buffer pointers
int ptrRfTxBuffCurr;
int ptrRfTxBuffEnd;
int ptrRfRxBuffCurr;
int ptrRfRxBuffEnd;

// Transmission Buffers
char TxRfSerialBuffer[SERIAL_BUFF_SIZE];
char RxRfSerialBuffer[SERIAL_BUFF_SIZE];

#endif
