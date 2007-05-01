#include <keyboard.h>
/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                            (c) Copyright 2003, 2004, Micrium, Inc., 
Weston, FL
*                                           All Rights Reserved
*
*                                              LogicPD Zoom
*                                               Sample code
*
* File    : keyboard.c
* By      : Jean J. Labrosse
* Modif by: P2
*
*********************************************************************************************************
*/

// Defines the lower Case
char lowerCase[51] = {F1,F2,'a','b','c','d',POWER,0,0,0,
					'e','f','g','h','i','j','k',0,0,0,
					'l','m','n','o','p','q','r',0,0,0,
					's','t','u','v','w','x','y',0,0,0,
					'z',SHIFT,CAPSCTRL,BACKSPACE,NUMCUR,'\\',ENTER,0,0,0};

// Defines the upper Case
char uperCase[51] = {F1,F2,'A','B','C','D',POWER,0,0,0,
					'E','F','G','H','I','J','K',0,0,0,
					'L','M','N','O','P','Q','R',0,0,0,
					'X','Y','S','T','U','V','W',0,0,0,
					'Z',SHIFT,CAPSCTRL,BACKSPACE,NUMCUR,'\\',ENTER,0,0,0};

// Defines the shift Case
char shift[51] = {F1,F2,'1','2','3','4',POWER,0,0,0,
					'5','6','7','8','9','0','@',0,0,0,
					'+','-','*','/','=',UP,'\"',0,0,0,
					ESCAPE,',','.','?',LEFT,':',RIGHT,0,0,0,
					TAB,SHIFT,CAPSCTRL,SPACE,NUMCUR,'\\',ENTER,0,0,0};

/*
*********************************************************************************************************
* ReadFromKeyboard()
*
* Description : Cette fonction lit un charactère sur le keyboard du PDA
*
* Argument(s) : 
*
* Return(s) : Le caractère lu
*********************************************************************************************************
*/
char ReadFromKeyboard (void)
{
    char err;
	char key;
	
	curmap =  lowerCase;
	
	GEDR_SF |= 0x01f00000;
	
	SER3_UTCR0 &= 0xf7;
	//GAFR_SF &= 0xfe0fffff;			// GAFR.3 = 0 (disable Alternate function on pin 3)
	

	printf("??????????????????????");

	char keybline =0;
	char keypress =0;
	char oldkeybline = -1;
	char x = 0;
	char y = 0;
	char columb = -1;
	int antirebonCount = -1;
	int iii;

	do {
		antirebonCount = 0;						/* Delay task execution for 500 ms                     */
		
		while (antirebonCount < 4)
		{	
			
			oldkeybline = 0;
			keybline = 0;
			columb = 0;
			x = 0;

			for(x=0;x<7;x++)
			{
				keybline = scanColumn(x);	

				if(keybline!=0)
				{				
					keypress = keybline;
					oldkeybline = keybline;
					antirebonCount = 0;

					for(y =0; y<8; y++)
					{
						keybline = scanColumn(x);	

						if(oldkeybline == keybline)
						{
							antirebonCount++;								
						}					
						oldkeybline = keybline;
					}

					if(antirebonCount > 4)
					{
						columb = x;	
						break;
					}
					else
					{
						antirebonCount = 0;
					}
				}
			}
		}
		//printf("Key %d press on columb %d \r\n",keypress,columb);
		
		keypress = getkeyNumber(keypress);

		//printf("Key %d press on columb %d \r\n",keypress,columb);
		
		key = getKey(keypress,columb);

		switch (key)
		{
		case SHIFT:	curmap = shift;
					break;
		case CAPSCTRL: curmap = uperCase;
						break;	
		default:	printf("%c \n\r",key);
					for(iii=0;iii<1000;iii++);
					curmap = lowerCase;
					break;
		
		}
	} while((key == CAPSCTRL) || (key == SHIFT));

	
	//GPCR = 0xffffffff;
	return key;
}

/*
*********************************************************************************************************
* ReadLine()
*
* Description : Cette fonction lit une série de caractère sur le clavier pour produire une ligne
*
* Argument(s) : line		  : La ligne de caractèree qui sera retournée.
*				Max_Line_Size : La longueur maximale que peut prendre la chaîne de caractères.
* Return(s) :
*********************************************************************************************************
*/
void ReadLine(char * line, int Max_Line_Size) {

	char buffer[Max_Line_Size];

	int i = 0;

	char key;

	while ((key != ENTER) && i < Max_Line_Size) {
		
		key = ReadFromKeyboard();
		
		switch (key)
		{
			case F1:
				break;
			case F2:
				break;
			case SHIFT:
				break;
			case ESCAPE:
				break;
			case ENTER:
				break;
			case CAPSCTRL:
				break;
			case NUMCUR:
				break;
			case UP:
				break;
			case DOWN:
				break;
			case LEFT:
				break;
			case RIGHT:
				break;
			case POWER:
				break;
			case TAB:
				break;
			case BACKSPACE:
				i--;
				break;

			default:	buffer[i] = key;
						i++;
						break;
			
		}
	}

	buffer[i] = '\0';
	strcpy(line, buffer);
}

/*
*********************************************************************************************************
* getKey()
*
* Description : Trouve la clé appuyée sur le keyboard sur la mapping courant du keyboard (upper, lower, shift)
*
* Argument(s) : key		      : La rangée du keyboard
*				columb		  : La colonne du keyboard
*
* Return(s) :	La clé appuyé sur le keyboard
*********************************************************************************************************
*/
char getKey(int key, int columb)
{
	return curmap[(key-1)*10 + (columb)];
}

/*
*********************************************************************************************************
* getkeyNumber()
*
* Description : Retourne en char la rangée de la clé appuyé 
*
* Argument(s) : key		      : La clé appuyé
*				
* Return(s) : La rangée de la clé
*********************************************************************************************************
*/
char getkeyNumber(int key)
{
	char newkey = 0;

	switch(key)
	{
		case 1: newkey = 1;
				break;
		case 2: newkey = 2;
				break;
		case 4: newkey = 3;
				break;
		case 8: newkey = 4;
				break;
		case 16: newkey = 5;
				break;
	}

	return newkey;
}

/*
*********************************************************************************************************
* scanColumn()
*
* Description : Retourne en char la colonne de la clé qui a été appuyé
*
* Argument(s) : columb		      : La colonne de la clé qui a été appuyé en int.
*				
* Return(s) : Le char correspondant à la colonne.
*********************************************************************************************************
*/
char scanColumn(int columb)
{	
	int	 ii;
	char masque;
	char tst;

	masque = 1;
	masque <<=columb;
	
	CERF_PDA_CPLD_KEYB_SCNLOW = (masque&0x0f);
	
	CERF_PDA_CPLD_KEYB_SCNHIGH = ((masque&0xf0)>>4);
	
	for(ii = 0; ii < 100; ii++);

	//printf("Low  : %x \n",CERF_PDA_CPLD_KEYB_SCNLOW);
	//printf("High : %x \n",CERF_PDA_CPLD_KEYB_SCNHIGH);

	tst = (( KEYBLINE >> 20) & 0x1F);
	// erD_sndValByteLbl("\r\nkey", (unsigned char) tst);
	CERF_PDA_CPLD_KEYB_SCNHIGH = 0x00;
	CERF_PDA_CPLD_KEYB_SCNLOW  = 0x00;
	//return (( KEYBLINE >> 20) & 0x1F);
	return tst;
}
