#include <REGX52.H>
#include "Delay.h"
#include "LCD1602.h"
#include "Matrixkey.h"

unsigned int Password,count,number;
int realkey=7355;

void main()
{
							LCD_Init();
							LCD_ShowString(1,1,"yes or no :");
      while(1)
        {			
					number=Matrixkey();
					if(number!=0)
					{
								if(number<=10)
								{
										count++;
									if(count<=4)
									{
											Password=Password*10;
											Password+=number%10;											
											LCD_ShowNum(2,1,Password,4);
									}
								}
									if(number==11)	
									{
										if(Password==realkey)
										{
											LCD_ShowString(1,14,"yes");
											Password=0;
											count=0;
											LCD_ShowNum(2,1,Password,4);											
										}
											else
											{
												LCD_ShowString(1,14,"no");
												Password=0;
												count=0;
												LCD_ShowNum(2,1,Password,4);
											}
									}
									
								
					
					
					}
					
					
        }

}