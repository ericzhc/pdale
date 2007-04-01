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
* File    : PDAL_GUI.c
* By      : P02
*
*********************************************************************************************************
*/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "GUI.h"
#include "FRAMEWIN.h"
#include "MULTIPAGE.h"
#include "BUTTON.h"
#include "includes.h"

#define MAP_ID_OK 1
#define LIST_ID_OK 2
#define MESSAGE_ID_OK 3
#define WINDOW_ID 666

/*********************************************************************
*
*       Variable globale
*		
**********************************************************************
*/

BUTTON_Handle mapButton;
BUTTON_Handle listButton;
BUTTON_Handle messageButton;

//MULTIPAGE_Handle MainWindow;
WM_HWIN MainWindow;
WM_HWIN _MapWindow;
WM_HWIN _ListWindow;
WM_HWIN _MessageWindow;

/*********************************************************************
*
*       Affichage
*		écran 240 x 320 pixel
**********************************************************************
*/
static void pdal_interface(void) {

	//LOGO
	GUI_SetBkColor(GUI_BROWN);
	GUI_Clear();
	GUI_SetColor(GUI_DARKRED);
	GUI_FillRect(0, 0, 240, 50);
	GUI_SetColor(GUI_YELLOW);
	GUI_DispStringAt("PDA Livraison Expresse", 50, 25);

	MainWindow = MULTIPAGE_CreateEx(0, 100, 240, 270, 0, WM_CF_SHOW, 0, 0);

	//MULTIPAGE_SetBkColor(MainWindow, GUI_BROWN, 1);

//	MULTIPAGE_AddPage(MainWindow, MapWindow , "Carte de la ville");

	//GUI_Delay(500);
	//MULTIPAGE_AddPage(MainWindow, ListWindow , "Liste des colis");
	//MULTIPAGE_AddPage(MainWindow, MessageWindow , "Messages");

	
	
}

/*******************************************************************
*
* _MapButton
*/
static void _Button(void) {
	
	/* Create the button
	mapButton = BUTTON_Create(0, 51, 79, 20, MAP_ID_OK, WM_CF_SHOW);
	listButton = BUTTON_Create(81, 51, 79, 20, LIST_ID_OK, WM_CF_SHOW);
	messageButton = BUTTON_Create(162, 51, 79, 20, MESSAGE_ID_OK, WM_CF_SHOW);
	/* Set the button text 
	BUTTON_SetText(mapButton, "Carte de la ville");
	BUTTON_SetText(listButton, "Liste des colis");
	BUTTON_SetText(messageButton, "Messages");
	/* Let window manager handle the button */

	

/*
	int key;
	key = GUI_WaitKey();
	switch (key){
	case MAP_ID_OK:
		_MapWindow(mapButton);
		break;
	case LIST_ID_OK:
		_ListWindow(listButton);
		break;
	case MESSAGE_ID_OK:
		_MessageWindow(messageButton);
		break;
	default:
		break;
	}
*/
	//while (GUI_WaitKey() != GUI_ID_OK);
	/*if(GUI_WaitKey() == GUI_ID_OK)
		{
		BUTTON_Delete(hButton);
		GUI_ClearRect(0, 50, 319, 239);
		GUI_Delay(1000);
	}*/
}

/*********************************************************************
*
*       Map
*
**********************************************************************
*/

void _Map(BUTTON_Handle mapButton){
	
	BUTTON_Delete(mapButton);
	GUI_ClearRect(0, 51, 79, 71);
	GUI_Delay(1000);

}

/*********************************************************************
*
*       Shipping List
*
**********************************************************************
*/

void _List(BUTTON_Handle listButton){

	BUTTON_Delete(listButton);
	GUI_ClearRect(81, 51, 161, 71);
	GUI_Delay(1000);

}

/*********************************************************************
*
*       Messages
*
**********************************************************************
*/

void _Message(BUTTON_Handle messageButton){
	
	BUTTON_Delete(messageButton);
	GUI_ClearRect(162, 51, 240, 71);
	GUI_Delay(1000);

}

/*********************************************************************
*
*       MainTask
*
**********************************************************************
*/

void MainTask(void) {

	GUI_Init();
	pdal_interface();
	/*/* Create the button
	mapButton = BUTTON_Create(0, 51, 79, 20, MAP_ID_OK, WM_CF_SHOW);
	listButton = BUTTON_Create(81, 51, 79, 20, LIST_ID_OK, WM_CF_SHOW);
	messageButton = BUTTON_Create(162, 51, 79, 20, MESSAGE_ID_OK, WM_CF_SHOW);
	/* Set the button text
	BUTTON_SetText(mapButton, "Carte de la ville");
	BUTTON_SetText(listButton, "Liste des colis");
	BUTTON_SetText(messageButton, "Messages");*/
	while (1) {
		GPS_Init();
	_Button();
	}
}
