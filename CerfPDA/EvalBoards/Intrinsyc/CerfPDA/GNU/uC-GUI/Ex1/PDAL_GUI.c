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
#include <stdio.h>
#include <math.h>
//#include "COMM.h" SERVER
#include "GUI.h"
#include "GUI_Utilities.h"
#include "FRAMEWIN.h"
#include "MULTIPAGE.h"
#include "LISTVIEW.h"

#define MAX_CODEBARRE_LENGTH 11
#define STATE_UNPICKED 0
#define STATE_PICKED 1
#define STATE_UNDELIVERED 2
#define STATE_DELIVERED 3
/*********************************************************************
*
*       Define of all the GUI IDs.
*		
**********************************************************************/
/*********************************************************************
*
* Standard for an ID define goes as follow :
* "TYPEOFGUIOBJECT"_"NAMEOFRELATEDTAB"_"NAMEOFOBJECT"_ID
*
**********************************************************************/

// Main Tabs Buttons ID
#define PB_MODIFCOLIS_TAB_ID           3000
#define PB_MAP_TAB_ID                  3001
#define PB_LISTCOLIS_TAB_ID            3002
#define PB_MESSAGE_TAB_ID              3003

// Initialisation
#define DD_INITIALISATION_CAMION_ID	   2151
#define PB_INITIALISATION_OK_ID		   2152

// Attente Lecture Code Barre
#define EDIT_CODEBARRE_NUMEROCOLIS_ID  2155
#define PB_CODEBARRE_OK_ID			   2156

// Modif Colis
#define EDIT_MODIFCOLIS_NUMEROCOLIS_ID 2157
#define EDIT_MODIFCOLIS_NOM_ID		   2158
#define EDIT_MODIFCOLIS_ADRESSE1_ID    2159
#define EDIT_MODIFCOLIS_ADRESSE2_ID    2159
#define EDIT_MODIFCOLIS_PDELDEBUT_ID   2161
#define EDIT_MODIFCOLIS_PDELFIN_ID     2162
#define EDIT_MODIFCOLIS_REMARQUES_ID   2163
#define CB_MODIFCOLIS_NONCUEILLI_ID    2164
#define CB_MODIFCOLIS_CUEILLI_ID       2165
#define CB_MODIFCOLIS_LIVRAISON_ID     2166
#define CB_MODIFCOLIS_LIVRE_ID         2167
#define PB_MODIFCOLIS_SAUVEGARDER_ID   2168

// Map

// List Colis
#define PB_LISTCOLIS_OK_ID			   2051
#define PB_LISTCOLIS_CANCEL_ID		   2052	
#define DD_LISTCOLIS_CAMION_ID		   2053
#define LV_LISTCOLIS_INFO_ID		   2054

// Message
#define EDIT_MESSAGE_ENVOI_ID		   2174
#define PB_MESSAGE_ENVOYER_ID		   2175
#define TEXT_MESSAGE_RECUS_ID		   2176

/*********************************************************************
*
*            Label Message
*
**********************************************************************/

// Initialisation
#define INITIALISATION_MSG1 "Veuillez vous identifier: "

// Code Barre
#define CODEBARRE_MSG1 "Veuillez maintenant utiliser le lecteur de "
#define CODEBARRE_MSG2 "   code barre sur le colis à consulter.    "
#define CODEBARRE_MSG3 "  Entrer manuellement le numéro du colis   "
#define CODEBARRE_MSG4 "  l'espace ci-dessous et appuyer sur OK.   "
#define CODEBARRE_MSG5 "Le numéro de colis entré\nest innexistant dans la\nbase de données."

// Modif Colis
#define MODIFCOLIS_MSG1 "Information sur la prochaine destination: "
#define MODIFCOLIS_MSG2 "Modification de l'état d'un colis"

// Message
#define MESSAGE_MSG1 "Messages reçus"
#define MESSAGE_MSG2 "Envoi d'un message (Max 50 char)"

/*********************************************************************
*
*       Prototype de Fonctions
*		
**********************************************************************/
void  GetIdColis(int, char*);
void  BuildTable(void);
void  BuildList();

void  PdaleInterface(void);

void  ShowModifColis();
void  ShowAttenteCodeBarre();
void  ShowListColis(void);

/*********************************************************************
*
*       Variables globales
*		
**********************************************************************/
WM_HWIN MAINWINDOW;
WM_HWIN INITIALISATIONWINDOW;
WM_HWIN CODEBARREWINDOW;
WM_HWIN MODIFCOLISWINDOW;
WM_HWIN MAPWINDOW;
WM_HWIN LISTECOLISWINDOW;
WM_HWIN MESSAGEWINDOW;
WM_HWIN CURRENTWINDOW;

//image
FILE*  OUVERTURE;
char*  BUFFER;
long   IMAGESIZE;
size_t SIZERESULT;

char    SQLCOLISNUMBER[MAX_CODEBARRE_LENGTH];
int		CAMIONCOURANT;

// Table pour test listView
char * Table[8][2];
char * test[20][2];

static const char * TestTable1[][2] = {
  { "623499-0010001", "Item 1" },
  { "623499-0010002", "Item 2" },
  { "623499-0010003", "Item 3" },
  { "1111111111", "Item 4" },
  { "623499-0010005", "Item 5" },
  { "623499-0010006", "Item 6" },
  { "623499-0010007", "Item 7" },
  { "623499-0010008", "Item 8" }
};

static const char * TestTable2[][2] = {
  { "0010001-623499", "va" },
  { "0010002-623499", "te" },
  { "0010003-623499", "faire" },
  { "0010004-623499", "mettre" },
  { "0010005-623499", "sale" },
  { "0010006-623499", "pédale" },
  { "0010007-623499", "osti" },
  { "0010008-623499", "dcave" }
};

// Construction de Initialisation Dialog
static const GUI_WIDGET_CREATE_INFO InitialisationDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "Identification", 0, 0, 0, 240, 270, 0, 0 },
	{ TEXT_CreateIndirect, INITIALISATION_MSG1, 0, 70, 50, 220, 15, 0, GUI_TA_CENTER },
	{ DROPDOWN_CreateIndirect, "Camion", DD_INITIALISATION_CAMION_ID, 50, 80, 80, 120, 0, 0},
	{ BUTTON_CreateIndirect, "OK", PB_INITIALISATION_OK_ID, 140, 80, 60, 20, 0, 0 }
};

// Construction de Wait Code Barre Dialog
static const GUI_WIDGET_CREATE_INFO WaitCodeBarreDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "Attente Lecture Code Barre...", 0, 0, 0, 240, 248, 0, 0 },
	{ TEXT_CreateIndirect, CODEBARRE_MSG1, 0, 10, 0, 220, 15, 0, GUI_TA_CENTER },
	{ TEXT_CreateIndirect, CODEBARRE_MSG2, 0, 10, 20, 220, 15, 0, GUI_TA_CENTER },
	{ TEXT_CreateIndirect, "OU", 0, 95, 40, 20, 0, 0, GUI_TA_CENTER },
	{ TEXT_CreateIndirect, CODEBARRE_MSG3, 0, 10, 60, 220, 15, 0, GUI_TA_CENTER },
	{ TEXT_CreateIndirect, CODEBARRE_MSG4, 0, 10, 80, 220, 15, 0, GUI_TA_CENTER },
	{ EDIT_CreateIndirect, NULL, EDIT_CODEBARRE_NUMEROCOLIS_ID, 50, 100, 70, 20, 0, 0 },
	{ BUTTON_CreateIndirect, "OK", PB_CODEBARRE_OK_ID, 125, 100, 40, 20, 0, 0 }
};

// Construction de Modification de Colis Dialog
static const GUI_WIDGET_CREATE_INFO ModifColisDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, MODIFCOLIS_MSG2, 0, 0, 0, 240, 248, 0, 0 },
	{ TEXT_CreateIndirect, "Colis # ", 0, 5, 5, 85, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "test", 0, 90, 5, 120, 19, 0 },
	{ TEXT_CreateIndirect, MODIFCOLIS_MSG1, 0, 5, 25, 220, 20, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "Nom: ", 0, 5, 50, 85, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "test", 0, 90, 50, 120, 19, 0 },
	{ TEXT_CreateIndirect, "Adresse: ", 0, 5, 70, 85, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "test", 0, 90, 70, 120, 19, 0 },
	{ TEXT_CreateIndirect, "test", 0, 90, 90, 120, 19, 0 },
	{ TEXT_CreateIndirect, "Plage horaire de:", 0, 5, 110, 85, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "test", 0, 90, 110, 120, 19, 0 },
	{ TEXT_CreateIndirect, "", 0, 5, 130, 70, 19, 0, GUI_TA_RIGHT },
	{ TEXT_CreateIndirect, "À:", 0, 75, 130, 10, 19, 0, GUI_TA_RIGHT },
	{ TEXT_CreateIndirect, "test", 0, 90, 130, 120, 19, 0 },
	{ TEXT_CreateIndirect, "Remarques:", 0, 5, 150, 85, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "test", 0, 90, 150, 120, 19, 0 },
	{ TEXT_CreateIndirect, "État Actuel:", 0, 5, 170, 70, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "Non Cueilli", 0, 80, 170, 60, 19, 0, GUI_TA_RIGHT },
	{ CHECKBOX_CreateIndirect, NULL, CB_MODIFCOLIS_NONCUEILLI_ID, 140, 170, 15, 15, 0, 0 },
	{ TEXT_CreateIndirect, "Cueilli", 0, 170, 170, 30, 15, 0, GUI_TA_RIGHT },
	{ CHECKBOX_CreateIndirect, NULL, CB_MODIFCOLIS_CUEILLI_ID, 200, 170, 15, 15, 0, 0 },
	{ TEXT_CreateIndirect, "En livraison", 0, 80, 190, 60, 15, 0, GUI_TA_RIGHT },
	{ CHECKBOX_CreateIndirect, NULL, CB_MODIFCOLIS_LIVRAISON_ID, 140, 190, 15, 15, 0, 0 },
	{ TEXT_CreateIndirect, "Livré", 0, 170, 190, 30, 15, 0, GUI_TA_RIGHT },
	{ CHECKBOX_CreateIndirect, NULL, CB_MODIFCOLIS_LIVRE_ID, 200, 190, 15, 15, 0, 0 },
	{ BUTTON_CreateIndirect, "SAUVEGARDER", PB_MODIFCOLIS_SAUVEGARDER_ID, 20, 210, 180, 20, 0, 0 }
};

// Construction de List de Colis Dialog
static const GUI_WIDGET_CREATE_INFO ListColisDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "Liste de vos colis", 0, 0, 0, 240, 248, 0, 0 },
	{ LISTVIEW_CreateIndirect, "Liste Colis", LV_LISTCOLIS_INFO_ID, 0, 0, 232, 240, 0, 0 }
};

// Construction de Message Dialog
static const GUI_WIDGET_CREATE_INFO MessageDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "Messages", 0, 0, 0, 240, 248, 0, 0 },
	{ TEXT_CreateIndirect, MESSAGE_MSG1, 0, 5, 5, 200, 15, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "", TEXT_MESSAGE_RECUS_ID, 5, 25, 200, 125, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, MESSAGE_MSG2, 0, 5, 155, 200, 15, 0, GUI_TA_LEFT },
	{ EDIT_CreateIndirect, NULL, EDIT_MESSAGE_ENVOI_ID, 0, 175, 230, 20, 0, 0 },
	{ BUTTON_CreateIndirect, "ENVOYER", PB_MESSAGE_ENVOYER_ID, 20, 205, 180, 25, 0, 0  }
};

/*********************************************************************
*
* All Callback that can happen in the Initialisation dialog
*
*********************************************************************/
static void InitialisationCallback(WM_MESSAGE * pMsg) 
{
	int NCode, Id, i, j;
	char CamionName[30];
	char AllCamionsName[400];
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN DD_Camion;
	DD_Camion = WM_GetDialogItem(hWin, DD_INITIALISATION_CAMION_ID);
	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
			// GetTruckNames(AllCamionsName); // SERVER
			j = 0;
			for (i = 0; AllCamionsName[i] == '\0'; i++)
			{
				if (AllCamionsName[i] != ';')
				{
					CamionName[j] = AllCamionsName[i];
					j++;
				}
				else
				{
					CamionName[j] = '\0';
					j = 0;
					DROPDOWN_AddString(DD_Camion, CamionName);
				}
			}
			break;

		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc); /* Id of widget */
			NCode = pMsg->Data.v; /* Notification code */
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED: /* React only if released */
					if (Id == PB_INITIALISATION_OK_ID) 
					{
						CAMIONCOURANT = DROPDOWN_GetSel(DD_Camion) + 1;
						ShowListColis();
						GUI_EndDialog(hWin, 0);
					}
					break;
			}
			break;

		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

/*********************************************************************
*
* All Callback that can happen in the Wait for CodeBarre read dialog
*
*********************************************************************/
static void CodeBarreWaitCallback(WM_MESSAGE * pMsg) 
{
	int NCode, Id;
	char CodeBarre[MAX_CODEBARRE_LENGTH];
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN Edit_NumeroColis;
	Edit_NumeroColis = WM_GetDialogItem(hWin, EDIT_CODEBARRE_NUMEROCOLIS_ID);		
	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			EDIT_SetMaxLen(Edit_NumeroColis, MAX_CODEBARRE_LENGTH);
		//	CodeBarreRead(CodeBarre); SERVER
			EDIT_SetText(Edit_NumeroColis, CodeBarre);
			break;

		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc); /* Id of widget */
			NCode = pMsg->Data.v; /* Notification code */
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED: /* React only if released */
					if (Id == PB_CODEBARRE_OK_ID) 
					{
						/* TODO: SQL Check the DataBase to see if the colis exist */
						EDIT_GetText(Edit_NumeroColis, SQLCOLISNUMBER, 1000);
						/* if (IsValidPackage(SQLCOLISNUMBER) == '1') SERVER
						{
							GUI_EndDialog(hWin, 0);
							//CodeBarreDisable(); SERVER
							ShowModifColis();
						} */
						if (strcmp(SQLCOLISNUMBER, "1111111111") == 0) // TO REMOVE
						{
							GUI_EndDialog(hWin, 0);
							//CodeBarreDisable(); SERVER
							ShowModifColis();
						}
						else
						{
							GUI_MessageBox(CODEBARRE_MSG5, "ERREUR", GUI_MESSAGEBOX_CF_MOVEABLE);
						}
					}
					break;
			}
			break;

		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

/*********************************************************************
*
* All Callback that can happen in the Modification de Colis dialog
*
*********************************************************************/
static void ModifColisCallback(WM_MESSAGE * pMsg) 
{
	static int CheckedBoxIndex;
	int NCode, Id, NewState;
	char AllPacketInfo[400];
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN CB_NonCueilli, CB_Cueilli, CB_EnLivraison, CB_Livre;

	CB_NonCueilli	 = WM_GetDialogItem(hWin, CB_MODIFCOLIS_NONCUEILLI_ID);
	CB_Cueilli		 = WM_GetDialogItem(hWin, CB_MODIFCOLIS_CUEILLI_ID);
	CB_EnLivraison	 = WM_GetDialogItem(hWin, CB_MODIFCOLIS_LIVRAISON_ID);
	CB_Livre		 = WM_GetDialogItem(hWin, CB_MODIFCOLIS_LIVRE_ID);

	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			
			// GetPacketInfos(SQLCOLISNUMBER, AllPacketInfo); // SERVER TODO DEFINE ORDER GODDAMMITNITNBTAOJH

			CheckedBoxIndex = CB_MODIFCOLIS_NONCUEILLI_ID; // TO REMOVE
			CHECKBOX_Check(CB_NonCueilli);
			break;

		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc); /* Id of widget */
			NCode = pMsg->Data.v; /* Notification code */
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED: /* React only if released */
					if (Id == PB_MODIFCOLIS_SAUVEGARDER_ID) 
					{
						GUI_EndDialog(hWin, 0);
						ShowAttenteCodeBarre();
						if (CHECKBOX_IsChecked(CB_NonCueilli))
						{
							NewState = STATE_UNPICKED; 
						}
						else if (CHECKBOX_IsChecked(CB_Cueilli))
						{
							NewState = STATE_PICKED; 
						}
						else if (CHECKBOX_IsChecked(CB_EnLivraison))
						{
							NewState = STATE_UNDELIVERED; 
						}
						else if (CHECKBOX_IsChecked(CB_Livre))
						{
							NewState = STATE_DELIVERED; 
						}

						// SetPacketState(SQLCOLISNUMBER, NewState); // SERVER
					}
					break;
				
				case WM_NOTIFICATION_CLICKED: /* React when a CB is clicked */
					if (Id == CB_MODIFCOLIS_NONCUEILLI_ID) 
					{
						if (CheckedBoxIndex == CB_MODIFCOLIS_NONCUEILLI_ID)
						{
							CHECKBOX_Check(CB_NonCueilli);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_CUEILLI_ID)
						{
							CHECKBOX_Uncheck(CB_Cueilli);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_LIVRAISON_ID)
						{
							CHECKBOX_Uncheck(CB_EnLivraison);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_LIVRE_ID)
						{
							CHECKBOX_Uncheck(CB_Livre);
						}

						CheckedBoxIndex = CB_MODIFCOLIS_NONCUEILLI_ID;
					}
					else if (Id == CB_MODIFCOLIS_CUEILLI_ID) 
					{
						if (CheckedBoxIndex == CB_MODIFCOLIS_NONCUEILLI_ID)
						{
							CHECKBOX_Uncheck(CB_NonCueilli);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_CUEILLI_ID)
						{
							CHECKBOX_Check(CB_Cueilli);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_LIVRAISON_ID)
						{
							CHECKBOX_Uncheck(CB_EnLivraison);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_LIVRE_ID)
						{
							CHECKBOX_Uncheck(CB_Livre);
						}

						CheckedBoxIndex = CB_MODIFCOLIS_CUEILLI_ID;
					}
					else if (Id == CB_MODIFCOLIS_LIVRAISON_ID) 
					{
						if (CheckedBoxIndex == CB_MODIFCOLIS_NONCUEILLI_ID)
						{
							CHECKBOX_Uncheck(CB_NonCueilli);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_CUEILLI_ID)
						{
							CHECKBOX_Uncheck(CB_Cueilli);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_LIVRAISON_ID)
						{
							CHECKBOX_Check(CB_EnLivraison);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_LIVRE_ID)
						{
							CHECKBOX_Uncheck(CB_Livre);
						}

						CheckedBoxIndex = CB_MODIFCOLIS_LIVRAISON_ID;
					}
					else if (Id == CB_MODIFCOLIS_LIVRE_ID) 
					{
						if (CheckedBoxIndex == CB_MODIFCOLIS_NONCUEILLI_ID)
						{
							CHECKBOX_Uncheck(CB_NonCueilli);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_CUEILLI_ID)
						{
							CHECKBOX_Uncheck(CB_Cueilli);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_LIVRAISON_ID)
						{
							CHECKBOX_Uncheck(CB_EnLivraison);
						}
						else if (CheckedBoxIndex == CB_MODIFCOLIS_LIVRE_ID)
						{
							CHECKBOX_Check(CB_Livre);
						}

						CheckedBoxIndex = CB_MODIFCOLIS_LIVRE_ID;
					}
					break;
			}
			break;

		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

/*********************************************************************
*
* All Callback that can happen in the Liste de Colis dialog
*
*********************************************************************/
static void ListColisCallback(WM_MESSAGE * pMsg)
{
	int NCode, Id;
	static int Flag;
	static char Colis1[MAX_CODEBARRE_LENGTH];
	static char Colis2[MAX_CODEBARRE_LENGTH];
	WM_HWIN hWin = pMsg->hWin;
	static WM_HWIN ListView;
	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			Flag = 0;

			ListView = WM_GetDialogItem(hWin, LV_LISTCOLIS_INFO_ID);

			LISTVIEW_AddColumn(ListView, 90, "# identification", GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(ListView, 138, "Statut du colis", GUI_TA_HCENTER | GUI_TA_VCENTER);
			BuildList(ListView, CAMIONCOURANT);
			break;

		case WM_NOTIFY_PARENT:
			
			Id = WM_GetId(pMsg->hWinSrc); /* Id of widget */
			NCode = pMsg->Data.v; /* Notification code */
			switch (NCode) 
			{
				case WM_NOTIFICATION_CLICKED:
					if (Flag == 0)
					{
						GetIdColis(LISTVIEW_GetSel(ListView), Colis1);
						Flag = 1;
					}
					else if (Flag == 1)
					{
						GetIdColis(LISTVIEW_GetSel(ListView), Colis2);
						Flag = 0;

						if (strcmp(Colis1, Colis2) == 0)
						{
							StringCopy(SQLCOLISNUMBER, Colis2);
							GUI_EndDialog(hWin, 0);
							ShowModifColis();
						}
					}
					break;
			
				default:
					break;
			}

		break;

		default:
			WM_DefaultProc(pMsg);
	}
}


/*********************************************************************
*
* All Callback that can happen in the Message dialog
*
*********************************************************************/
static void MessageCallback(WM_MESSAGE * pMsg) 
{
	int NCode, Id;
	char MessagesRecus[400];
	char MessageToSend[51];
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN Edit_Envoi, Text_Recu;

	Edit_Envoi = WM_GetDialogItem(hWin, EDIT_MESSAGE_ENVOI_ID);
	Text_Recu  = WM_GetDialogItem(hWin, TEXT_MESSAGE_RECUS_ID);

	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			EDIT_SetMaxLen(Edit_Envoi, 50);
			//GetMessages(CAMIONCOURANT, MessagesRecus); SERVER
			TEXT_SetText(Text_Recu, MessagesRecus); 
			break;

		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc); /* Id of widget */
			NCode = pMsg->Data.v; /* Notification code */
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED: /* React only if released */
					if (Id == PB_MESSAGE_ENVOYER_ID) 
					{
						EDIT_GetText(Edit_Envoi, MessageToSend, 50);
						//SendMessage(CAMIONCOURANT, Message); SERVER
						EDIT_SetText(Edit_Envoi, "");
					}
					break;
			}
			break;

		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

/*********************************************************************
*
*       Montre le dialog d'initialisation du PDA
*
**********************************************************************/
void ShowInitDialog(void)
{
	INITIALISATIONWINDOW = GUI_CreateDialogBox(InitialisationDialogCreate, GUI_COUNTOF(InitialisationDialogCreate), &InitialisationCallback, 0, 0, 50);
	CURRENTWINDOW = INITIALISATIONWINDOW;
}

/*********************************************************************
*
*       Shows the Attente de lecture Code Barre Dialog
*
**********************************************************************/
void ShowAttenteCodeBarre(void)
{
	CODEBARREWINDOW = GUI_CreateDialogBox(WaitCodeBarreDialogCreate, GUI_COUNTOF(WaitCodeBarreDialogCreate), &CodeBarreWaitCallback, 0, 0, 72);
	CURRENTWINDOW = CODEBARREWINDOW;
	//CodeBarreInit(); SERVER
}

/*********************************************************************
*
*       Shows the Modification de Colis Dialog
*
**********************************************************************/
void ShowModifColis(void)
{
	MODIFCOLISWINDOW = GUI_CreateDialogBox(ModifColisDialogCreate, GUI_COUNTOF(ModifColisDialogCreate), &ModifColisCallback, 0, 0, 72);
	CURRENTWINDOW = MODIFCOLISWINDOW;
}

/*********************************************************************
*
*       Shows the Map
*
**********************************************************************/
void ShowMap(void)
{
	//ToDO : Requête map tous les 5 sec
	
	//OUVERTURE du fichier jpeg
	OUVERTURE = fopen("C:/images.jpg", "rb");
	if (OUVERTURE==NULL) { fputs("File error", stderr); exit(1); }

	//determine size du stream
	fseek (OUVERTURE , 0 , SEEK_END);
	IMAGESIZE = ftell(OUVERTURE);
	rewind(OUVERTURE);
	
	//allocation de la mémoire
	BUFFER = (char*) malloc(sizeof(char)*IMAGESIZE);
	if (BUFFER == NULL) { fputs("Memory error", stderr); exit(2); }

	// copie du fichier dans le BUFFER
	SIZERESULT = fread (BUFFER, 1, IMAGESIZE, OUVERTURE);
	if (SIZERESULT != IMAGESIZE) { fputs("Reading error", stderr); exit(3); }

	//fermeture du fichier jpeg
	fclose(OUVERTURE);

	GUI_JPEG_Draw(BUFFER, sizeof(char)*IMAGESIZE, 0, 72);
}

/*********************************************************************
*
*       Shows the Shipping List
*
**********************************************************************/
void ShowListColis(void)
{
	LISTECOLISWINDOW = GUI_CreateDialogBox(ListColisDialogCreate, GUI_COUNTOF(ListColisDialogCreate), &ListColisCallback, 0, 0, 72);
	CURRENTWINDOW = LISTECOLISWINDOW;
}

/*********************************************************************
*
*       Shows the Messages Dialog
*
*********************************************************************/
void ShowMessage(void)
{	
	MESSAGEWINDOW = GUI_CreateDialogBox(MessageDialogCreate, GUI_COUNTOF(MessageDialogCreate), &MessageCallback, 0, 0, 72);
	CURRENTWINDOW = MESSAGEWINDOW;
}


/*********************************************************************
*
*       Fonction construisant la liste des colis en fonction du 
*		camion sélectionné
*
*********************************************************************/
void BuildList(WM_HWIN opListView, int ipCamion)
{
    /* SQL REQUEST TODO: */
	unsigned int i;
	switch (ipCamion)
	{
		case 1:
			for (i = 0; i < GUI_COUNTOF(TestTable1); i++)
			{
				memcpy(Table[i], TestTable1[i], GUI_COUNTOF(TestTable1));
			}
			break;

		case 2:
			for (i = 0; i < GUI_COUNTOF(TestTable1); i++)
			{
				memcpy(Table[i], TestTable2[i], GUI_COUNTOF(TestTable2));
			}
			break;
	}
	

	/*for (i = 0; i < GUI_COUNTOF(TestTable1); i++) 
	{
		LISTVIEW_DeleteRow(listView, 0);
	}
*/

	for (i = 0; i < GUI_COUNTOF(Table); i++) 
	{
		LISTVIEW_AddRow(opListView, Table[i]);
	}
	
}

/*********************************************************************
*
* Construction d'un tableau à partir d'un string
*
*********************************************************************/
void BuildTable()
{
		
}

/*******************************************************************
*
* Recherche du numéro d'identification du colis celon sa position dans la liste
*
/******************************************************************/
void GetIdColis(int piLigne, char* ColisID)
{
	StringCopy(ColisID, Table[piLigne][0]);
}

/*******************************************************************
*
* The method which checks the status of the 4 main buttons (PDA TAB)
*
/******************************************************************/
static void CheckButtonState(void)
{
	int Key;
	BUTTON_Handle ModifColisButton;
	BUTTON_Handle MapButton;
	BUTTON_Handle ListColisButton;
	BUTTON_Handle MessageButton;

	/* Create the button*/
	if (CURRENTWINDOW != INITIALISATIONWINDOW)
	{
		ModifColisButton = BUTTON_Create(0,   51, 59, 20, PB_MODIFCOLIS_TAB_ID, WM_CF_SHOW);
		MapButton		 = BUTTON_Create(60,  51, 59, 20, PB_MAP_TAB_ID,        WM_CF_SHOW);
		ListColisButton  = BUTTON_Create(120, 51, 59, 20, PB_LISTCOLIS_TAB_ID,  WM_CF_SHOW);
		MessageButton    = BUTTON_Create(180, 51, 59, 20, PB_MESSAGE_TAB_ID,    WM_CF_SHOW);
		
		/* Set the button text*/
		BUTTON_SetText(ModifColisButton, "Modif colis");
		BUTTON_SetText(MapButton,        "Map");
		BUTTON_SetText(ListColisButton,  "Liste colis");
		BUTTON_SetText(MessageButton,    "Messages");
	}

	/* Wait for the button that is being pressed */
	Key = GUI_WaitKey();
	switch (Key)
	{
		case PB_MODIFCOLIS_TAB_ID:
			GUI_EndDialog(CURRENTWINDOW, 0);
			ShowAttenteCodeBarre();
			break;
		case PB_MAP_TAB_ID:
			GUI_EndDialog(CURRENTWINDOW, 0);
			GUI_Clear();
			PdaleInterface();
			ShowMap();
			break;
		case PB_LISTCOLIS_TAB_ID:
			GUI_EndDialog(CURRENTWINDOW, 0);
			ShowListColis();
			break;
		case PB_MESSAGE_TAB_ID:
			GUI_EndDialog(CURRENTWINDOW, 0);
			ShowMessage();
			break;
		case PB_INITIALISATION_OK_ID:
			break;
		default:
			break;
	}
}

/*********************************************************************
*
*       Affichage écran 240 x 320 pixel
*		
**********************************************************************/
static void PdaleInterface(void) 
{
	//LOGO
	GUI_SetBkColor(GUI_BROWN);
	GUI_Clear();
	GUI_SetColor(GUI_DARKRED);
	GUI_FillRect(0, 0, 240, 50);
	GUI_SetColor(GUI_YELLOW);
	GUI_DispStringAt("PDA Livraison Expresse", 50, 25);
}

/*********************************************************************
*
*       MainTask
*
***********************************************************************/
void MainTask(void) 
{
	GUI_Init();
	PdaleInterface();
	ShowInitDialog();
	while (1) 
	{
		CheckButtonState();
	}
}