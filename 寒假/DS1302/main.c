#include <REGX52.H>
#include "LCD1602.h"
#include "DS1302.h"
#include "Delay.h"

unsigned char sec;
void main()
{
	LCD_Init();
	DS1302_Init();
	DS1302_SetTime();
			while(1)
			{
						DS1302_ReadTime();
						LCD_ShowNum(1,1,DS1302_Time[5],2);
			}



}
