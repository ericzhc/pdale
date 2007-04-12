#ifndef COMDRIVER_H
#define COMDRIVER_H



#define SERIAL_BUFF_SIZE 1024 // more than twice the worst-case from TSIP doc

/*
*********************************************************************************************************
*                                        DEFINITION DES STRUCTURES
*********************************************************************************************************
*/

typedef struct {
	char* Buffer;
	int* ptrCurrent;
	int* ptrEnd;
} COM_BUFF_INFO;

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void ComDriverInit(short);
void ISR_Serial();
void TransmitBuffer(char*);
void BufferTransmissionTask();

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/
COM_BUFF_INFO GetTaskRxComBuff();
OS_FLAG_GRP* comFlag;

#define TX_SERIAL_DATA_READY_TO_SEND	1
#define TX_SERIAL_DATA_SENT				2
#define RX_SERIAL_DATA_AVAILABLE		4


#endif
