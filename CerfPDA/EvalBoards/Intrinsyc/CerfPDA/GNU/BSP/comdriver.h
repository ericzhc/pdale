#ifndef COMDRIVER_H
#define COMDRIVER_H

#define SERIAL_BUFF_SIZE 1024 // more than twice the worst-case from TSIP doc

#define TASK_SERIAL_PRIO 7
#define TASK_SERIAL_SIZE 8

typedef struct {
	char* Buffer;
	int* ptrCurrent;
	int* ptrEnd;
} COM_BUFF_INFO;

void ComDriverInit(short);
void ISR_Serial();
void BufferTransmissionTask();
COM_BUFF_INFO GetTaskRxComBuff();

#endif
