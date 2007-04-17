#include <keyboard.h>


/*$PAGE*/
/* 
*********************************************************************************************************
*                                              STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the 
book's text, you MUST
*               initialize the ticker only once multitasking has started.
* Arguments   : p_arg is the argument passed to 'AppStartTask()' by 
'OSTaskCreate()'.
* Notes       : 1) The first line of code is used to prevent a compiler 
warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this 
statement.
*               2) Interrupts are enabled once the task start because the 
I-bit of the CCR register was
*                  set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

char lowerCase[51] = {POWER,F1,F2,'a','b','c','d',0,0,0,
					'k','e','f','g','h','i','j',0,0,0,
					'r','l','m','n','o','p','q',0,0,0,
					'y','s','t','u','v','w','x',0,0,0,
					ENTER,'z',SHIFT,CAPSCTRL,BACKSPACE,NUMCUR,'\\',0,0,0};

char uperCase[51] = {POWER,F1,F2,'A','B','C','D',0,0,0,
					'K','E','F','G','H','I','J',0,0,0,
					'R','L','M','N','O','P','Q',0,0,0,
					'Y','S','T','U','V','W','X',0,0,0,
					ENTER,'Z',SHIFT,CAPSCTRL,BACKSPACE,NUMCUR,'\\',0,0,0};

char shift[51] = {POWER,F1,F2,'1','2','3','4',0,0,0,
					'@','5','6','7','8','9','0',0,0,0,
					'\"','+','-','*','/','=',UP,0,0,0,
					RIGHT,ESCAPE,',','.','?',LEFT,':',0,0,0,
					ENTER,TAB,SHIFT,CAPSCTRL,SPACE,NUMCUR,'\\',0,0,0};


char ReadFromKeyboard (void)
{
    char err;
	char key;
	
	curmap =  lowerCase;
	
	GAFR_SF &= 0xfe0fffff;			// GAFR.3 = 0 (disable Alternate function on pin 3)
	GPDR_SF &= 0xfe0fffff;	


	char keybline =0;
	char keypress =0;
	char oldkeybline = -1;
	char x = 0;
	char y = 0;
	char columb = -1;
	int antirebonCount = -1;

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
					curmap = lowerCase;
					break;
		
		}
	} while((key == CAPSCTRL) || (key == SHIFT));

	//GPCR = 0xffffffff;
	return key;
}

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

char getKey(int key, int columb)
{
	return curmap[(key-1)*10 + (columb)];
}


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



char scanColumn(int columb)
{	
	int	 ii;
	char masque = 1;
	char tst;

	masque = 1;
	masque <<=columb;


	
	CERF_PDA_CPLD_KEYB_SCNLOW = (masque&0x0f);
	
	CERF_PDA_CPLD_KEYB_SCNHIGH = ((masque&0xf0)>>4);
	
	//for(ii = 0; ii < 8000; ii++);

	//printf("Low  : %x \n",CERF_PDA_CPLD_KEYB_SCNLOW);
	//printf("High : %x \n",CERF_PDA_CPLD_KEYB_SCNHIGH);

	tst = (( KEYBLINE >> 20) & 0x1F);
	// erD_sndValByteLbl("\r\nkey", (unsigned char) tst);
	CERF_PDA_CPLD_KEYB_SCNHIGH = 0x00;
	CERF_PDA_CPLD_KEYB_SCNLOW  = 0x00;
	//return (( KEYBLINE >> 20) & 0x1F);
	return tst;
}
