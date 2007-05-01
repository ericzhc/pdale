#ifndef MAP_H
#define MAP_H
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/
#define TASK_MAP_SIZE 1024
#define MAX_MAP_SIZE 30720 // 30 Ko for map
OS_STK MapUpdateTaskStk[TASK_MAP_SIZE];
#define MAPDELAY 83 // Time to update the map in minutes

// The Map
char CurrentMap[MAX_MAP_SIZE];
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void MapUpdateTask();
int getMap(char*);
//void ReceiveMap(char, char*);


#endif
