/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                            (c) Copyright 2003, 2004, Micrium, Inc., Weston, FL
*                                           All Rights Reserved
*
*                                              LogicPD Zoom
*                                               Sample code
*
* Fichier : apptest.cs
* Par     : Marc-Étienne Lebeau, Julien Beaumier-Ethier, Richard Labonté, Francis Robichaud, Julien Marchand
* Date    : 2007/04/13
* Description : Application qui teste tous les modules relies au CerfPDA à tour de role.
*
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE        4096
void initimage();
/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

        OS_STK  AppStartTaskStk[TASK_STK_SIZE];
        OS_STK  GuiTaskStk[TASK_STK_SIZE];
		OS_STK  RFDriverInitStk[TASK_STK_SIZE];

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppStartTask(void *p_arg);
static  void  GuiTask(void *p_arg);

/*
*********************************************************************************************************
*                                             C ENTRY POINT
*********************************************************************************************************
*/

int  main (void)
{
    INT8U err;

    BSP_Init();                                 /* Initialize BSP                                      */

    erD_sndstr("\r\nInitialize uC/OS-II...");
    OSInit();                                   /* Initialize uC/OS-II                                 */

                                                /* Create start task                                   */
    OSTaskCreateExt(AppStartTask,
                    NULL,
                    (OS_STK *)&AppStartTaskStk[TASK_STK_SIZE-1],
                    TASK_START_APP_PRIO,
                    TASK_START_APP_PRIO,
                    (OS_STK *)&AppStartTaskStk[0],
                    TASK_STK_SIZE,
                    NULL,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

                                                /* Give a name to tasks                                */
#if 0                                                
#if OS_TASK_NAME_SIZE > 10
    OSTaskNameSet(OS_IDLE_PRIO,        "Idle task",  &err);
    //OSTaskNameSet(OS_STAT_PRIO,        "Stat task",  &err);
    OSTaskNameSet(TASK_START_APP_PRIO, "Start task", &err);
#endif
#endif

    erD_sndstr("\r\nStart uC/OS-II...");
    OSStart();                                  /* Start uC/OS-II                                      */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                              STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
* Arguments   : p_arg is the argument passed to 'AppStartTask()' by 'OSTaskCreate()'.
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*               2) Interrupts are enabled once the task start because the I-bit of the CCR register was
*                  set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

static void  AppStartTask (void *p_arg)
{
    INT8U err;
    p_arg = p_arg;                              /* Prevent compiler warning                            */

	Tmr_TickInit();                             /* Start timer tick                                    */
    erD_sndstr("\r\nStart timer tick...");
                                         /* Give a name to tasks                                */
#if OS_TASK_NAME_SIZE > 10
    OSTaskNameSet(TASK_GUI_PRIO,        "GUI task",  &err);
#endif

	erD_sndstr("Testing BarCode\n\r");
	TestBarCode();

	erD_sndstr("Testing GPS\n\r");
	TestGPS();

	erD_sndstr("Testing Cell\n\r");
	TestCell();

	erD_sndstr("Testing GUI\n\r");
	TestGui();

	erD_sndstr("...Tests done...\n\r");
}

/*
* Essaie de lire une valeur avec le lecteur de code-barre
* L'utilisateur doit reagir dans un delai de 5 secondes.
*/
void TestBarCode() 
{
	char buff[50];
	BCR_Enable();
	CodeBarreRead(buff);
	erD_sndstr(buff);
	BCR_Disable();
}

/*
* Recoit les donnees du module GPS et les conserve
*/
void TestGPS() 
{
	OSTaskCreateExt(GPSUpdateTask,
					NULL,
					(OS_STK *)&GPSUpdateTaskStk[TASK_GPS_SIZE-1],
					TASK_GPS_PRIO,
					TASK_GPS_PRIO,
					(OS_STK *)&GPSUpdateTaskStk[0],
					TASK_GPS_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskCreateExt(GPSSendDataTask,
					NULL,
					(OS_STK *)&GPSSendDataTaskTsk[TASK_GPS_SIZE-1],
					TASK_GPS_SEND_PRIO,
					TASK_GPS_SEND_PRIO,
					(OS_STK *)&GPSSendDataTaskTsk[0],
					TASK_GPS_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	GPS_Init();
	// Delai de 10 secondes pour la réception d'au moins une donnée GPS
	OSTimeDlyHMSM(0,0,10,0);
	char tempbuff[25];
	sprintf(tempbuff, "%c", GPSPosition.Latitude);
	erD_sndstr("Lat:");
	erD_sndstr(tempbuff);
	erD_snd_cr();

	sprintf(tempbuff, "%c", GPSPosition.Longitude);
	erD_sndstr("Long:");
	erD_sndstr(tempbuff);
	erD_snd_cr();

	GPSDisable();
	// Tester la fonction GPSEnable
	GPSEnable();
	// Conclure le test
	GPSDisable();
}

/*
* Effectue tous les tests qui requiert le module cellulaire
*/
void TestCell() 
{
	RFDriverInit();
	OSTimeDlyHMSM(0,0,2,0);

	// Tester toutes les fonctions utilisant le module cellulaire
	SendMessage(1, "hello world\0");
	erD_sndstr("Envoi message done\n\r");
	OSTimeDlyHMSM(0,0,10,0);

	char buff[200];
	// Recevoir tous les messages
	GetMessages(buff);
	erD_sndstr("Reception messages done\n\r");
	erD_sndstr(buff);
	OSTimeDlyHMSM(0,0,10,0);

	// Lire tous les noms de camions disponibles
	GetTruckNames(buff);
	erD_sndstr(buff);

	// Lire tous les colis du premier camion
	GetAllPackages(1, buff);
	erD_sndstr("Reception packages done\n\r");
	erD_sndstr(buff);

	// Remplacer le numero du colis avec un numero valide
	char colis[12];
	strcpy(colis, "11111111");

	// Verifier que le colis existe dans la base de donnees
	IsValidPackage(colis);

	// Mettre a jour le statut d un colis
	SetPacketState(colis, 2);
	// Recevoir les informations d un colis
	GetPacketInfos(colis, buff);
	erD_sndstr(buff);

	// Tester la reception d'une carte
	TestMap();

	OSTaskDel(TASK_RFSERIAL_PRIO);
}

/*
* Fait la demande d'envoi d une carte au serveur
*/
void TestMap() 
{
	char tempBuff[MAX_MAP_SIZE];	
	char data[] = {COMMAND_GETMAP, ';',COMMAND_EOL};

	memset(CurrentMap, 0x0, MAX_MAP_SIZE);
	TransmitRfBuffer(data);
	ReceiveData(COMMAND_GETMAP, CurrentMap);
	int length = getMap(tempBuff);
	erD_sndstr("Map received\n\r");
	erD_sndstr((char)length);
}

/*
* Test l'initialisation de l'interface
*/
void TestGui() 
{
	OSTaskCreateExt(GuiTask,
					NULL,
					(OS_STK *)&GuiTaskStk[TASK_STK_SIZE-1],
					TASK_GUI_PRIO,
					TASK_GUI_PRIO,
					(OS_STK *)&GuiTaskStk[0],
					TASK_STK_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	PdaleInterface();
	
	// Montre le dialog d'initialisation
	ShowInitDialog();

	// Delai de 10 secondes pour laisser l'interface s'initialiser
	OSTimeDlyHMSM(0,0,10,0);
	OSTaskDel(TASK_GUI_PRIO);
}

/*
* Task requise pour l interface
*/
static void  GuiTask (void *p_arg)
{
	// Initialisation d'un GUI
	GUI_Init();
    p_arg = p_arg;                              /* Prevent compiler warning                            */
    while (1)
    {
        GUI_TOUCH_Exec();
        GUI_Exec();
		OSTimeDly(50);
    }
}
