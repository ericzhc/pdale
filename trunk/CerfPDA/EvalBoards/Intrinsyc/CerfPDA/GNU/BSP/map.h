#ifndef MAP_H
#define MAP_H

#define TASK_MAP_SIZE 1024
#define MAX_MAP_SIZE 30720 // 30 Ko for map

#define MAPDELAY 5 // Time to update the map in minutes

// The Map
char CurrentMap[MAX_MAP_SIZE];

void MapUpdateTask();
void ReceiveMap(char, char*);

#endif
