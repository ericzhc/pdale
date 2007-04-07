#ifndef COMDRIVER_H
#define COMDRIVER_H

#include <includes.h>

#define SERIAL_BUFF_SIZE 350 // more than twice the worst-case from TSIP doc

#define  TASK_SERIAL_PRIO 7
#define  TASK_SERIAL_SIZE 8

typedef struct {
	char* RxBuffer;
	OS_EVENT* semaphore;
	int* ptrCurrent;
	int* ptrEnd;
} TASK_BUFF_PROTECT;

void ComInit(u32 baud);
void ISR_Serial();
void BufferTransmissionTask();
TASK_BUFF_PROTECT GetTaskBuffProtectStruct();

#endif
