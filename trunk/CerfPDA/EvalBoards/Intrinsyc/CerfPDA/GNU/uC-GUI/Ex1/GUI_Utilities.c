/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                            (c) Copyright 2003, 2004, Micrium, Inc., Weston, FL
*                                           All Rights Reserved
*
*                                              LogicPD Zoom
*                                              
*
* File    : GUI_Utilities.c
* By      : P02
*
* Usage   : A few fonctions used by PDAL_GUI.c to make the code clearer
*
*********************************************************************************************************
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/*********************************************************************
*
* Fonction de base pour faire la copie d'une chaîne de caractères
*
*********************************************************************/
char* StringCopy(char* opString1, char* ipString2)
{
	int i;
	for(i = 0; ipString2[i] != '\0'; i++)
	{
		opString1[i] = ipString2[i];
	}

	return opString1;
}
