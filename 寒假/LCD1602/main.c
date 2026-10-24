#include <REGX52.H>
#include "Lcd1602.h"
#include "Delay.h"

unsigned int Time=0;

void main()
{     
	    LCD_Init();
			while(1)
			{	
				
				Time++;
				LCD_ShowNum(1,1,Time,3);
				Delay(100);
			
				
			}


}