#include <REGX52.H>
#include "KEY.h"
#include "DELAY.h"
#include "LCD1602.h"
#include "AT24C02.h"
#include "I2C.h"

unsigned char keynum;
void main()
{
		LCD_Init();		
      while(1)
        {
						unsigned char num;

						keynum=Key();
						if(keynum==1)
						{
									num++;
									LCD_ShowNum(1,1,num,3);
						}
						if(keynum==2)
						{
									num--;
									LCD_ShowNum(1,1,num,3);
						}
						if(keynum==3)
						{
								  AT24C02_WriteByte(1,num);
								  Delay(5);
									LCD_ShowString(2,1,"WRITE OK");
									Delay(1000);
									LCD_ShowString(2,1,"        ");
						}		
						if(keynum==4)
						{
								   num=AT24C02_ReadByte(1);
									 LCD_ShowNum(1,1,num,3);
									 LCD_ShowString(2,1,"READ BACK");
									 Delay(1000);
									 LCD_ShowString(2,1,"         ");

						}		
				
				
				
				}

}
