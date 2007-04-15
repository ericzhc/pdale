#ifndef BCR_H
#define BCR_H

#define TASK_BCR_SIZE 1024

#define MAX_BARCODE_LENGTH 20

void BCR_Init();
void BCRUpdateTask();
void ReadCode();
void BCR_Disable();
void BCR_Enable();
void ReceivedCode();

// Barcode value read
char BCRValue[MAX_BARCODE_LENGTH];
OS_FLAG_GRP* bcFlag;
#define BAR_CODE_AVAILABLE			1
#define BAR_CODE_CONSUMED			2

#endif
