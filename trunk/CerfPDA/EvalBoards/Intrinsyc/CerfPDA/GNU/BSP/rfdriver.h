#ifndef RFDRIVER_H
#define RFDRIVER_H

#define RFSERIAL_BUFF_SIZE 1024

#define TASK_RFSERIAL_PRIO 10
#define TASK_RFSERIAL_SIZE 11

void RFDriverInit();
void ISR_Serial_RF();
void TransmitRfBuffer(char*, int);
void BufferRfTransmissionTask();
COM_BUFF_INFO GetTaskRxRfBuff();
void open_socket(char* port,char* ipaddress);

#endif
