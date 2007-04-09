#ifndef BCR_H
#define BCR_H

#define TASK_BCR_PRIO 9
#define TASK_BCR_SIZE 1024

#define MAX_BARCODE_LENGTH 10

void BCR_Init();
void BCRUpdateTask();
void ReadCode();
void BCR_Disable();
void BCR_Enable();
void ReceivedCode();

#endif
