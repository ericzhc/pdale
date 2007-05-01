#ifndef BCR_H
#define BCR_H

#define TASK_BCR_SIZE 1024

#define MAX_BARCODE_LENGTH 20
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void BCR_Init();
void BCRUpdateTask();
void ReadCode();
void BCR_Disable();
void BCR_Enable();
void ReceivedCode();
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

char BCRValue[MAX_BARCODE_LENGTH];   // Barcode value read
OS_FLAG_GRP* bcFlag;
#define BAR_CODE_AVAILABLE			1
#define BAR_CODE_CONSUMED			2

#endif
