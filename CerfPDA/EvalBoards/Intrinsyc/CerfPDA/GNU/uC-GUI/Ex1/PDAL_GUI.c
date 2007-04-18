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
#include <includes.h>
//#include "COMM.h" SERVER
#include "GUI.h"
#include "FRAMEWIN.h"
#include "MULTIPAGE.h"
#include "LISTVIEW.h"

#define STATE_UNPICKED		  0
#define STATE_PICKED		  1
#define STATE_UNDELIVERED	  2
#define STATE_DELIVERED		  3

#define INFO_STATE			  0
#define INFO_NOMCLIENT		  1
#define INFO_NOMDESTINATAIRE  2
#define INFO_ADRESSECLIENT1	  3
#define INFO_ADRESSECLIENT2	  4
#define INFO_ADRESSEDEST1	  5
#define INFO_ADRESSEDEST2	  6
#define INFO_PDELDEBUTCUEILLE 7
#define INFO_PDELFINCUEILLE	  8
#define INFO_PDELDEBUTLIVRE	  9
#define INFO_PDELFINLIVRE	  10
#define INFO_REMARQUES		  11

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
#define PB_CODEBARRE_LECTEUR_ID		   2169

// Modif Colis
#define TEXT_MODIFCOLIS_NUMEROCOLIS_ID 2157
#define TEXT_MODIFCOLIS_NOM_ID		   2158
#define TEXT_MODIFCOLIS_ADRESSE1_ID    2159
#define TEXT_MODIFCOLIS_ADRESSE2_ID    2159
#define TEXT_MODIFCOLIS_PDELDEBUT_ID   2161
#define TEXT_MODIFCOLIS_PDELFIN_ID     2162
#define TEXT_MODIFCOLIS_REMARQUES_ID   2163
#define CB_MODIFCOLIS_NONCUEILLI_ID    2164
#define CB_MODIFCOLIS_CUEILLI_ID       2165
#define CB_MODIFCOLIS_LIVRAISON_ID     2166
#define CB_MODIFCOLIS_LIVRE_ID         2167
#define PB_MODIFCOLIS_SAUVEGARDER_ID   2168

// List Colis
#define PB_LISTCOLIS_OK_ID			   2051
#define PB_LISTCOLIS_CANCEL_ID		   2052	
#define DD_LISTCOLIS_CAMION_ID		   2053
#define LV_LISTCOLIS_INFO_ID		   2054

// Message
#define EDIT_MESSAGE_ENVOI_ID		   2174
#define PB_MESSAGE_ENVOYER_ID		   2175
#define TEXT_MESSAGE_RECUS_ID		   2176
#define PB_MESSAGE_ENTRETEXT_ID		   2177

/*********************************************************************
*
*            Text Messages in Dialog
*
**********************************************************************/

// Initialisation
#define INITIALISATION_MSG1 "Veuillez vous identifier: "

// Code Barre
#define CODEBARRE_MSG1 "Veuillez maintenant utiliser le lecteur de "
#define CODEBARRE_MSG2 "   code barre sur le colis à consulter.    "
#define CODEBARRE_MSG3 "Appuyer sur Lecteur, lire, appuyer sur OK. "
#define CODEBARRE_MSG5 "Le numéro de colis entré\nest innexistant dans la\nbase de données."

// Modif Colis
#define MODIFCOLIS_MSG1 "Information sur la prochaine destination: "
#define MODIFCOLIS_MSG2 "Modification de l'état d'un colis"

// Message
#define MESSAGE_MSG1 "Messages reçus"
#define MESSAGE_MSG2 "Envoi d'un message (Max 50 char)"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               VARIABLES
***************************************«******************************************************************
*/


/*********************************************************************
*
*       Prototype de Fonctions
*		
**********************************************************************/
static void AppStartTask(void*);

void	GetIdColis(int, char*);
void	BuildTable(void);
void	BuildList(WM_HWIN, int);

void	PdaleInterface(void);

void	ShowModifColis();
void	ShowAttenteCodeBarre();
void    ShowMap(void);
void    ShowMessage(void);
void	ShowListColis(void);

void    StringCopy(char*, char*);
WM_HWIN ShowLoadingDialog();

/*********************************************************************
*
*       Variables globales
*		
**********************************************************************/
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

char    SQLCOLISNUMBER[MAX_BARCODE_LENGTH];
int		CAMIONCOURANT = 0;
char    MessagesRecus[300];

// Table pour listView
char TABLECOLIS[50][2][11];

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
	{ TEXT_CreateIndirect, CODEBARRE_MSG3, 0, 10, 40, 220, 15, 0, GUI_TA_CENTER },
	{ EDIT_CreateIndirect, NULL, EDIT_CODEBARRE_NUMEROCOLIS_ID, 20, 120, 150, 20, 0, 0 },
	{ BUTTON_CreateIndirect, "OK", PB_CODEBARRE_OK_ID, 175, 120, 40, 20, 0, 0 },
	{ BUTTON_CreateIndirect, "LECTEUR", PB_CODEBARRE_LECTEUR_ID, 80, 65, 80, 40, 0, 0 },
};

// Construction de Modification de Colis Dialog
static const GUI_WIDGET_CREATE_INFO ModifColisDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, MODIFCOLIS_MSG2, 0, 0, 0, 240, 248, 0, 0 },
	{ TEXT_CreateIndirect, "Colis # ", 0, 5, 5, 85, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "test", TEXT_MODIFCOLIS_NUMEROCOLIS_ID, 90, 5, 120, 19, 0 },
	{ TEXT_CreateIndirect, MODIFCOLIS_MSG1, 0, 5, 25, 220, 20, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "Nom: ", 0, 5, 50, 85, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "test", TEXT_MODIFCOLIS_NOM_ID, 90, 50, 120, 19, 0 },
	{ TEXT_CreateIndirect, "Adresse: ", 0, 5, 70, 85, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "test", TEXT_MODIFCOLIS_ADRESSE1_ID, 90, 70, 120, 19, 0 },
	{ TEXT_CreateIndirect, "test", TEXT_MODIFCOLIS_ADRESSE2_ID, 90, 90, 120, 19, 0 },
	{ TEXT_CreateIndirect, "Plage horaire de:", 0, 5, 110, 85, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "test", TEXT_MODIFCOLIS_PDELDEBUT_ID, 90, 110, 120, 19, 0 },
	{ TEXT_CreateIndirect, "", 0, 5, 130, 70, 19, 0, GUI_TA_RIGHT },
	{ TEXT_CreateIndirect, "À:", 0, 75, 130, 10, 19, 0, GUI_TA_RIGHT },
	{ TEXT_CreateIndirect, "test", TEXT_MODIFCOLIS_PDELFIN_ID, 90, 130, 120, 19, 0 },
	{ TEXT_CreateIndirect, "Remarques:", 0, 5, 150, 85, 19, 0, GUI_TA_LEFT },
	{ TEXT_CreateIndirect, "test", TEXT_MODIFCOLIS_REMARQUES_ID, 90, 150, 120, 19, 0 },
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
	{ BUTTON_CreateIndirect, "ENVOYER", PB_MESSAGE_ENVOYER_ID, 110, 205, 85, 25, 0, 0 },
	{ BUTTON_CreateIndirect, "ENTRER TEXT", PB_MESSAGE_ENTRETEXT_ID, 20, 205, 85, 25, 0, 0 }
};

// Construction dun Loading Dialog
static const GUI_WIDGET_CREATE_INFO LoadingDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "CHARGEMENT...", 0, 0, 0, 240, 270, 0, 0 },
	{ TEXT_CreateIndirect, "EN CHARGEMENT...", 0, 75, 75, 100, 20, 0, GUI_TA_LEFT }
};

/*
*********************************************************************************************************
* InitialisationCallback()
*
* Description : Cette fonction gère tous les callbacks du dialog d'Initialisation
*
* Argument(s) : pMsg: est l'indice permettant de déterminer d'où vient et quelle
*				est la notification.
*
* Return(s) : 
*********************************************************************************************************
*/
static void InitialisationCallback(WM_MESSAGE * pMsg) 
{
	// Initialisation de variables
	int NCode, Id, i, j;
	char CamionName[30];
	char AllCamionsName[400];
	WM_HWIN hWin = pMsg->hWin;

	// Acquisition du DropDown
	WM_HWIN DD_Camion;
	DD_Camion = WM_GetDialogItem(hWin, DD_INITIALISATION_CAMION_ID);
	switch (pMsg->MsgId)
	{
		// Initialisation du dialog
		case WM_INIT_DIALOG:
			// GetTruckNames(AllCamionsName); // SERVER REQUEST
			
			// Remplir le DropDown avec le nom des camions
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
					// Conserver le ID du Camion choisi et faire apparaitre la liste de colis
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

/*
*********************************************************************************************************
* CodeBarreWaitCallback()
*
* Description : Cette fonction gère tous les callbacks du dialog AttenteCodeBarre
*
* Argument(s) : pMsg: est l'indice permettant de déterminer d'où vient et quelle
*				est la notification.
*
* Return(s) : 
*********************************************************************************************************
*/
static void CodeBarreWaitCallback(WM_MESSAGE * pMsg) 
{
	// Initialisation de variables
	int NCode, Id, i;
	WM_HWIN hWin = pMsg->hWin;
	char buffer_codebarre[MAX_BARCODE_LENGTH];

	// Acquisition du Edit "NumeroColis"
	WM_HWIN Edit_NumeroColis;
	Edit_NumeroColis = WM_GetDialogItem(hWin, EDIT_CODEBARRE_NUMEROCOLIS_ID);		
	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			// Initialisation du dialog
			EDIT_SetMaxLen(Edit_NumeroColis, MAX_BARCODE_LENGTH);
			break;

		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc); /* Id of widget */
			NCode = pMsg->Data.v; /* Notification code */
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED: /* React only if released */
					// Verifier si le colis est valid et conserver son numero avant dafficher
					// le dialog de Modification du colis
					if (Id == PB_CODEBARRE_OK_ID) 
					{
						/* TODO: SQL Check the DataBase to see if the colis exist */
						EDIT_GetText(Edit_NumeroColis, SQLCOLISNUMBER, 1000);
						/* if (IsValidPackage(SQLCOLISNUMBER) == '1') SERVER REQUEST
						{
							GUI_EndDialog(hWin, 0);
							//CodeBarreDisable(); SERVER REQUEST
							ShowModifColis();
						} */
						if (strcmp(SQLCOLISNUMBER, "1111111111") == 0) // TO REMOVE
						{
							GUI_EndDialog(hWin, 0);
							ShowModifColis();
						}
						else
						{
							// Si le numero nest pas valide il y aura une erreur
							WM_HWIN MBox = GUI_MessageBox(CODEBARRE_MSG5, "ERREUR", GUI_MESSAGEBOX_CF_MOVEABLE);
							OSTimeDly(3000);
							GUI_Clear();
							PdaleInterface();
						}
					}
					else if (Id == PB_CODEBARRE_LECTEUR_ID)
					{
						memset(buffer_codebarre,0x00,MAX_BARCODE_LENGTH);
						CodeBarreInit(); // SERVER REQUEST
						CodeBarreRead(buffer_codebarre); // SERVER REQUEST
						CodeBarreDisable(); // SERVER REQUEST
						EDIT_SetText(Edit_NumeroColis, buffer_codebarre);
					}
					break;
			}
			break;

		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

/*
*********************************************************************************************************
* ModifColisCallback()
*
* Description : Cette fonction gère tous les callbacks du dialog ModifColis
*
* Argument(s) : pMsg: est l'indice permettant de déterminer d'où vient et quelle
*				est la notification.
*
* Return(s) : 
*********************************************************************************************************
*/
static void ModifColisCallback(WM_MESSAGE * pMsg) 
{
	// Initialisation des variables
	static int CheckedBoxIndex;
	int NCode, Id, NewState, i, j, k;
	char AllPacketInfo[] = "1;test;test;test;test;test;test;test;test;test;test;test;";
	char PacketInfoList[13][40];
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN CB_NonCueilli, CB_Cueilli, CB_EnLivraison, CB_Livre;
	WM_HWIN TEXT_NumeroColis, TEXT_Nom, TEXT_Adresse1, TEXT_Adresse2;
	WM_HWIN TEXT_PDeLDebut, TEXT_PDeLFin, TEXT_Remarques;
	WM_HWIN LoadingDialog;

	// Acquisition des objets CheckBox
	CB_NonCueilli	 = WM_GetDialogItem(hWin, CB_MODIFCOLIS_NONCUEILLI_ID);
	CB_Cueilli		 = WM_GetDialogItem(hWin, CB_MODIFCOLIS_CUEILLI_ID);
	CB_EnLivraison	 = WM_GetDialogItem(hWin, CB_MODIFCOLIS_LIVRAISON_ID);
	CB_Livre		 = WM_GetDialogItem(hWin, CB_MODIFCOLIS_LIVRE_ID);

	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			// Initialisation du dialog
			// Acquisition des objets TEXT
			TEXT_NumeroColis = WM_GetDialogItem(hWin, TEXT_MODIFCOLIS_NUMEROCOLIS_ID);
			TEXT_Nom	     = WM_GetDialogItem(hWin, TEXT_MODIFCOLIS_NOM_ID);
			TEXT_Adresse1	 = WM_GetDialogItem(hWin, TEXT_MODIFCOLIS_ADRESSE1_ID);
			TEXT_Adresse2	 = WM_GetDialogItem(hWin, TEXT_MODIFCOLIS_ADRESSE2_ID);
			TEXT_PDeLDebut	 = WM_GetDialogItem(hWin, TEXT_MODIFCOLIS_PDELDEBUT_ID);
			TEXT_PDeLFin	 = WM_GetDialogItem(hWin, TEXT_MODIFCOLIS_PDELFIN_ID);
			TEXT_Remarques	 = WM_GetDialogItem(hWin, TEXT_MODIFCOLIS_REMARQUES_ID);
					
			TEXT_SetText(TEXT_NumeroColis, SQLCOLISNUMBER);

			// Obtenir linformation pour le numero de colis 
			LoadingDialog = ShowLoadingDialog();
			// GetPacketInfos(SQLCOLISNUMBER, AllPacketInfo); // SERVER
			GUI_EndDialog(LoadingDialog, 0);
			
			j = 0;
			k = 0;
			for(i = 0; AllPacketInfo[i] != '\0'; i++)
			{
				if (AllPacketInfo[i] != ';')
				{
					PacketInfoList[j][k] = AllPacketInfo[i];
					k++;
				}
				else
				{
					PacketInfoList[j][k] = '\0';
					j++;
					k = 0;
				}
			}

			// Construire les TEXT selon l'information recue
			if (PacketInfoList[INFO_STATE][0] == '0' || PacketInfoList[INFO_STATE][0] == '1')
			{
				TEXT_SetText(TEXT_Nom,       PacketInfoList[INFO_NOMCLIENT]);
				TEXT_SetText(TEXT_Adresse1,  PacketInfoList[INFO_ADRESSECLIENT1]);
				TEXT_SetText(TEXT_Adresse2,  PacketInfoList[INFO_ADRESSECLIENT2]);
				TEXT_SetText(TEXT_PDeLDebut, PacketInfoList[INFO_PDELDEBUTCUEILLE]);
				TEXT_SetText(TEXT_PDeLFin,   PacketInfoList[INFO_PDELFINCUEILLE]);
				TEXT_SetText(TEXT_Remarques, PacketInfoList[INFO_REMARQUES]);
				if (PacketInfoList[INFO_STATE][0] == '0')
				{
					CheckedBoxIndex = CB_MODIFCOLIS_NONCUEILLI_ID; 
					CHECKBOX_Check(CB_NonCueilli);
				}
				else
				{
					CheckedBoxIndex = CB_MODIFCOLIS_CUEILLI_ID; 
					CHECKBOX_Check(CB_Cueilli);
				}
			}
			else
			{
				TEXT_SetText(TEXT_Nom,       PacketInfoList[INFO_NOMDESTINATAIRE]);
				TEXT_SetText(TEXT_Adresse1,  PacketInfoList[INFO_ADRESSEDEST1]);
				TEXT_SetText(TEXT_Adresse2,  PacketInfoList[INFO_ADRESSEDEST2]);
				TEXT_SetText(TEXT_PDeLDebut, PacketInfoList[INFO_PDELDEBUTLIVRE]);
				TEXT_SetText(TEXT_PDeLFin,   PacketInfoList[INFO_PDELFINLIVRE]);
				if (PacketInfoList[INFO_STATE][0] == '2')
				{
					CheckedBoxIndex = CB_MODIFCOLIS_LIVRAISON_ID; 
					CHECKBOX_Check(CB_EnLivraison);
				}
				else
				{
					CheckedBoxIndex = CB_MODIFCOLIS_LIVRE_ID; 
					CHECKBOX_Check(CB_Livre);
				}
			}

			TEXT_SetText(TEXT_Remarques, PacketInfoList[INFO_REMARQUES]);
			break;

		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc); /* Id of widget */
			NCode = pMsg->Data.v; /* Notification code */
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED: /* React only if released */
					// Sauvegarder le nouvel etat du colis et revenir a Attente de Code Barre
					if (Id == PB_MODIFCOLIS_SAUVEGARDER_ID) 
					{
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
						GUI_EndDialog(hWin, 0);
						ShowAttenteCodeBarre();
					}
					break;
				
				case WM_NOTIFICATION_CLICKED: /* React when a CB is clicked */
					// Algorithme interpretant les CB comme des RadioButton
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

/*
*********************************************************************************************************
* ListColisCallback()
*
* Description : Cette fonction gère tous les callbacks du dialog ListColis
*
* Argument(s) : pMsg: est l'indice permettant de déterminer d'où vient et quelle
*				est la notification.
*
* Return(s) : 
*********************************************************************************************************
*/
static void ListColisCallback(WM_MESSAGE * pMsg)
{
	int NCode, Id;
	static int OldTimer = 0;
	static int NewTimer = 0;
	static int Flag;
	int Sentinel = 0;
	static char Colis1[MAX_BARCODE_LENGTH];
	static char Colis2[MAX_BARCODE_LENGTH];
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN LoadingDialog;
	static WM_HWIN ListView;
	
	//Update du timer
	NewTimer = GUI_GetTime();

		
	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			Flag = 0;
			//Initialisation du timer
			OldTimer = GUI_GetTime();
			//Acquistion du ListView
			ListView = WM_GetDialogItem(hWin, LV_LISTCOLIS_INFO_ID);

			LISTVIEW_AddColumn(ListView, 90, "# identification", GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(ListView, 138, "Statut du colis", GUI_TA_HCENTER | GUI_TA_VCENTER);
			
			//Construction du ListView
			LoadingDialog = ShowLoadingDialog();
			BuildList(ListView, CAMIONCOURANT);
			GUI_EndDialog(LoadingDialog, 0);
			
			break;

		case WM_NOTIFY_PARENT:
			
			//Vérification si 5 minutes ont passées
			if ((NewTimer - OldTimer) > 300000)
			{
				NewTimer = GUI_GetTime();
				OldTimer = GUI_GetTime();
				Sentinel = 1;
			}
			
			Id = WM_GetId(pMsg->hWinSrc); /* Id of widget */
			NCode = pMsg->Data.v; /* Notification code */
			switch (NCode) 
			{
				case WM_NOTIFICATION_CLICKED:
					//Si moins de 5 minutes ont passées
					if (Sentinel != 1)
					{
						//Acquisition de l'ID du colis lors d'un premier clique
						if (Flag == 0)
						{
							GetIdColis(LISTVIEW_GetSel(ListView), Colis1);
							Flag = 1;
						}
						//Acquisition de l'ID du colis lors d'un deuxième clique
						else if (Flag == 1)
						{
							GetIdColis(LISTVIEW_GetSel(ListView), Colis2);
							Flag = 0;

							//Si l'ID du colis est le même lors des deux clique, chargement des caractéristiques du colis 
							if (strcmp(Colis1, Colis2) == 0)
							{
								StringCopy(SQLCOLISNUMBER, Colis2);
								GUI_EndDialog(hWin, 0);
								ShowModifColis();
							}
						}
					}
					//Si 5minutes ont passées, on recharge le dialogue
					else
					{
						GUI_EndDialog(hWin, 0);
						ShowListColis();
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


/*
*********************************************************************************************************
* MessageCallback()
*
* Description : Cette fonction gère tous les callbacks du dialog Message
*
* Argument(s) : pMsg: est l'indice permettant de déterminer d'où vient et quelle
*				est la notification.
*
* Return(s) : 
*********************************************************************************************************
*/
static void MessageCallback(WM_MESSAGE * pMsg) 
{
	// Initialisation des variables
	int NCode, Id, FirstTimeFlag, i;
	char MessageToSend[51];
	char key;
	char EditText[51];
	char TempBufMsg[51];
	memset(EditText, 0x00, 51);
	memset(MessageToSend, 0x00, 51);
	
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN LoadingDialog;
	WM_HWIN Edit_Envoi, Text_Recu;

	// Acquisiton du EDIT et du TEXT
	Text_Recu  = WM_GetDialogItem(hWin, TEXT_MESSAGE_RECUS_ID);
	Edit_Envoi = WM_GetDialogItem(hWin, EDIT_MESSAGE_ENVOI_ID);

	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			// Initialisation du dialog
			EDIT_SetMaxLen(Edit_Envoi, 50);
			memset(MessagesRecus, 0x00, 300);
			
			// Aller chercher les 5 derniers messages recus et les afficher
			LoadingDialog = ShowLoadingDialog();
			GetMessages(CAMIONCOURANT, MessagesRecus); // SERVER REQUEST
			GUI_EndDialog(LoadingDialog, 0);
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
						// Envoyer le message au server
						EDIT_GetText(Edit_Envoi, MessageToSend, 50);
						//printf("Message :%s", MessageToSend);
						SendMessage(CAMIONCOURANT, MessageToSend); // SERVER REQUEST
						EDIT_SetText(Edit_Envoi, "");
					}
					else if (Id == PB_MESSAGE_ENTRETEXT_ID)
					{
						printf("EnterText \n\r");
						key = ' ';
						while (key != ENTER && i < 51) 
						{
							key = ReadFromKeyboard();
							switch (key)
							{
								case F1:
						printf("1 \n\r");

									break;
								case F2:
						printf("2 \n\r");

									break;
								case SHIFT:
						printf("3 \n\r");
									break;
								case ESCAPE:
						printf("4 \n\r");
									break;
								case ENTER:
						printf("5 \n\r");
									break;
								case CAPSCTRL:
						printf("6 \n\r");
									break;
								case NUMCUR:
						printf("7 \n\r");
									break;
								case UP:
						printf("8 \n\r");
									break;
								case DOWN:
						printf("9 \n\r");
									break;
								case LEFT:
						printf("10 \n\r");
									break;
								case RIGHT:
						printf("11 \n\r");
									break;
								case POWER:
						printf("12 \n\r");
									break;
								case TAB:
						printf("13 \n\r");
									break;
								case BACKSPACE:
						printf("14 \n\r");

									i--;
									memset(TempBufMsg, 0x00, 51);
									strncpy(TempBufMsg, EditText, strlen(EditText)-2);
									StringCopy(EditText, TempBufMsg);
									EDIT_SetText(Edit_Envoi, EditText);
									break;

								default:
									printf("Key Pressed : %c", key);
									strcat(EditText, &key);
									printf("Buffer : %s", EditText);
									EDIT_SetText(Edit_Envoi, EditText);
									i++;
									break;
							}

							OSTimeDly(300);
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

/*
*********************************************************************************************************
* LoadingCallback()
*
* Description : Cette fonction gère tous les callbacks du dialog Loading
*
* Argument(s) : pMsg: est l'indice permettant de déterminer d'où vient et quelle
*				est la notification.
*
* Return(s) : 
*********************************************************************************************************
*/
static void LoadingCallback(WM_MESSAGE * pMsg)
{
	/* This function does nothing but is necessary for creating
	the loading dialog */
}

/*
*********************************************************************************************************
* ShowInitDialog()
*
* Description : Cette fonction crée le Dialog d'initialisation
*
* Argument(s) : 
*
* Return(s) : 
*********************************************************************************************************
*/
void ShowInitDialog(void)
{
	INITIALISATIONWINDOW = GUI_CreateDialogBox(InitialisationDialogCreate, GUI_COUNTOF(InitialisationDialogCreate), &InitialisationCallback, 0, 0, 50);
	CURRENTWINDOW = INITIALISATIONWINDOW;
}

/*
*********************************************************************************************************
* ShowAttenteCodeBarre()
*
* Description : Cette fonction crée le Dialog d'attente de Code Barre
*
* Argument(s) : 
*
* Return(s) : 
*********************************************************************************************************
*/
void ShowAttenteCodeBarre(void)
{
	CODEBARREWINDOW = GUI_CreateDialogBox(WaitCodeBarreDialogCreate, GUI_COUNTOF(WaitCodeBarreDialogCreate), &CodeBarreWaitCallback, 0, 0, 72);
	CURRENTWINDOW = CODEBARREWINDOW;
}

/*
*********************************************************************************************************
* ShowModifColis()
*
* Description : Cette fonction crée le Dialog d'attente de Modification d'un Colis
*
* Argument(s) : 
*
* Return(s) : 
*********************************************************************************************************
*/
void ShowModifColis(void)
{
	MODIFCOLISWINDOW = GUI_CreateDialogBox(ModifColisDialogCreate, GUI_COUNTOF(ModifColisDialogCreate), &ModifColisCallback, 0, 0, 72);
	CURRENTWINDOW = MODIFCOLISWINDOW;
}

/*
*********************************************************************************************************
* ShowMap()
*
* Description : Cette fonction affiche la carte de la ville
*
* Argument(s) : 
*
* Return(s) : 
*********************************************************************************************************
*/
void ShowMap(void)
{
	//ToDO : Requête map, besoin adresse de l'image ( pour buffer) et de son size
	
	
	//GUI_JPEG_Draw(BUFFER, sizeof(char)*IMAGESIZE, 0, 72);
}

/*
*********************************************************************************************************
* ShowListColis()
*
* Description : Cette fonction affiche le dialog de la liste des colis
*
* Argument(s) : 
*
* Return(s) : 
*********************************************************************************************************
*/
void ShowListColis(void)
{
	LISTECOLISWINDOW = GUI_CreateDialogBox(ListColisDialogCreate, GUI_COUNTOF(ListColisDialogCreate), &ListColisCallback, 0, 0, 72);
	CURRENTWINDOW = LISTECOLISWINDOW;
}

/*
*********************************************************************************************************
* ShowMessage()
*
* Description : Cette fonction affiche le dialog de Message
*
* Argument(s) : 
*
* Return(s) : 
*********************************************************************************************************
*/
void ShowMessage(void)
{	
//	GUI_EndDialog(CURRENTWINDOW, 0);
	MESSAGEWINDOW = GUI_CreateDialogBox(MessageDialogCreate, GUI_COUNTOF(MessageDialogCreate), &MessageCallback, 0, 0, 72);
	CURRENTWINDOW = MESSAGEWINDOW;
}

/*
*********************************************************************************************************
* ShowLoadingDialog()
*
* Description : Cette fonction affiche le dialog de Loading
*
* Argument(s) : 
*
* Return(s) : WM_HWIN : Retourne l'objet dialog associé à la création de ce dialog
*********************************************************************************************************
*/
WM_HWIN ShowLoadingDialog(void)
{
	return GUI_CreateDialogBox(LoadingDialogCreate, GUI_COUNTOF(LoadingDialogCreate), &LoadingCallback, 0, 0, 50);
}

/*
*********************************************************************************************************
* BuildList()
*
* Description : Cette fonction construit la liste de colis et leur état pour un camion donné
*
* Argument(s) : opListView: L'objet ListView à inclure dans le dialog qui se fera remplir par 
*				la fonction
*
*				ipCamion: Le ID du camion pour lequel on veut l'information des colis
*
* Return(s)   : 
*********************************************************************************************************
*/
void BuildList(WM_HWIN opListView, int ipCamion)
{
    char String[] = {"1111111111;aaaa;2222222222;bbbb;3333333333;cccc;4444444444;dddd*"};
	char StringLu[11] = "0";
	int i = 0; 
	int j = 0;
	int Row = 0;

	//GetAllPackages(int truckid, char* buffer)  //SERVER
	
	//Boucle jusqu'à la fin du string reçu
	while(String[i] != '*')
	{
		//Copie dans le string temporaire jusqu'à un délimiteur de string
		while( String[i] != ';')
		{
			StringLu[j] = String[i];
			i++;
			j++;
		}
		i++;
		j++;
		//Copie de l'identificateur dans TABLECOLIS
		StringCopy(TABLECOLIS[Row][0], StringLu);
		//Nettoyage du string temporaire
		for(j = 0; StringLu[j] != '\0'; j++)
		{
			StringLu[j] = ' ';
		}
		j = 0;
		//Copie dans le string temporaire jusqu'à un délimiteur de string
		while(String[i] != ';' && String[i] != '*')
		{
			StringLu[j] = String[i];
			i++;
			j++;
		}
		//Vérification si on est sur le dernier caractère du string
		if (String[i] != '*')
		{
			i++;
		}
		j++;
		//Copie du status dans TABLECOLIS
		StringCopy(TABLECOLIS[Row][1], StringLu);
		//Nettoyage du string temporaire
		for(j = 0; StringLu[j] != '\0'; j++)
		{
			StringLu[j] = ' ';
		}
		j = 0;
		Row++;
	}
	//Création du ListView à partir de TABLECOLIS
	for (i = 0; i < GUI_COUNTOF(TABLECOLIS); i++) 
	{
		GUI_ConstString temp[2];
		temp[0] = TABLECOLIS[i][0];
		temp[1] = TABLECOLIS[i][1];
		LISTVIEW_AddRow(opListView, temp);
	}
}

/*
*********************************************************************************************************
* GetIdColis()
*
* Description : Cette fonction prend une ligne de la table des colis et en sort son ID
*
* Argument(s) : ColisID: L'ID que du Colis que l'on cherche
*
*				ipLigne: La ligne correspondante au colis dont on veut le ID 
*
* Return(s)   : 
*********************************************************************************************************
*/
void GetIdColis(int ipLigne, char* ColisID)
{
	StringCopy(ColisID, TABLECOLIS[ipLigne][0]);
}

/*
*********************************************************************
* StringCopy()
*
* Description : Cette fonction prend String2 et le copie dans String1
*
* Argument(s) : opString1 : String dans lequel le string2 sera copié
*				ipString2 : String qui sera copié dans le string1
*
* Return(s)   : 
*********************************************************************************************************
*/
void StringCopy(char* opString1, char* ipString2)
{
	int i;
	for(i = 0; ipString2[i] != '\0'; i++)
	{
		opString1[i] = ipString2[i];
	}
	opString1[i] = '\0';
}

/*
*********************************************************************
* CheckButtonState()
*
* Description : Cette fonction regarde l'état des 4 buttons principaux (PDA TAB)
*
* Argument(s) : 
*
* Return(s)   : 
*********************************************************************************************************
*/
void CheckButtonState(void)
{
	int Key;
	static int InMap = 0;
	static int InMsg = 0;
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
		if (InMap == 0) BUTTON_SetText(MapButton, "Map");
		else BUTTON_SetText(MapButton, "Refresh");
		BUTTON_SetText(ListColisButton,  "Liste colis");
		if (InMsg == 0)
		{	
			BUTTON_SetText(MessageButton,    "Messages");
		}
		else
		{
			BUTTON_SetText(MessageButton,    "New Msg?");
		}
	}

	/* Wait for the button that is being pressed */
	Key = GUI_WaitKey();
	switch (Key)
	{
		case PB_MODIFCOLIS_TAB_ID:
			InMap = 0;
			InMsg = 0;
			GUI_EndDialog(CURRENTWINDOW, 0);
			ShowAttenteCodeBarre();
			break;
		case PB_MAP_TAB_ID:
			InMap = 1;
			InMsg = 0;
			GUI_EndDialog(CURRENTWINDOW, 0);
			GUI_Clear();
			PdaleInterface();
			ShowMap();
			break;
		case PB_LISTCOLIS_TAB_ID:
			InMap = 0;
			InMsg = 0;
			GUI_EndDialog(CURRENTWINDOW, 0);
			ShowListColis();
			break;
		case PB_MESSAGE_TAB_ID:
			InMap = 0;
			if (InMsg == 0)
			{
				BUTTON_SetText(MessageButton, "New Msg?");
				GUI_EndDialog(CURRENTWINDOW, 0);
				InMsg = 1;
				ShowMessage();
			}
			else
			{
				WM_HWIN Text_Recu;
				char NewMsg[300];
				memset(NewMsg, 0x00, 300);
				Text_Recu  = WM_GetDialogItem(CURRENTWINDOW, TEXT_MESSAGE_RECUS_ID);
				GetMessages(0, NewMsg);
				if (strcmp(MessagesRecus, NewMsg))
				{
					//startPlay(); // SERVER REQUEST
					TEXT_SetText(Text_Recu, NewMsg);
				}
			}
			break;
		default:
			break;
	}
}

/*
*********************************************************************************************************
* PdaleInterface()
*
* Description : Cette fonction crée la base du GUI du PDA
*
* Argument(s) : 
*
* Return(s)   : 
*********************************************************************************************************
*/
void PdaleInterface(void) 
{
	//LOGO
	GUI_SetBkColor(GUI_BROWN);
	GUI_Clear();
	GUI_SetColor(GUI_DARKRED);
	GUI_FillRect(0, 0, 240, 50);
	GUI_SetColor(GUI_YELLOW);
	GUI_DispStringAt("PDA Livraison Expresse", 50, 25);
}

/*
*********************************************************************************************************
* main()
*
* Description : Base d'initialisation du programme (PDA TAB)
*
* Argument(s) : 
*
* Return(s)   : 
*********************************************************************************************************
*/

void MainTask (void *p_arg)
{

	// Initialisation d'un GUI
	GUI_Init();
	PdaleInterface();
	
	// Montre le dialog d'initialisation
	ShowInitDialog();
	/*printf("show init\n\r");*/
	while (1) 
	{
		CheckButtonState(); // Regarde l'état des boutons constamment
		OSTimeDly(100);
	}
}
