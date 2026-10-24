#include <REGX52.H>
#include "Delay.h"
#include "Matrixkey.h"
#include "Lcd1602.h"

unsigned char Keynum,count;
unsigned int Password;
int realkey=7355;//设置密码
void main()
{			LCD_Init();
			LCD_ShowString(1,1,"yes or no :");
      while(1)
        {
						Keynum=Matrixkey();
						if(Keynum!=0)
						{			
										if(Keynum<=10)//十以内
										{
												count++;
												if(count<=4)//限制四位
												{
													Password=Password*10;
													Password+=Keynum%10;//取个位
													LCD_ShowNum(2,1,Password,4);//四位密码
												}
												
										}
										if(Keynum==11)//以11为确认键
												{     
													if(Password==realkey)
													{
														LCD_ShowString(1,14,"yes");
														Password=0;
														count=0;
														LCD_ShowNum(2,1,Password,4);//刷新
														
													}
												
													else
													{
														LCD_ShowString(1,14,"no ");
														Password=0;
														count=0;
														LCD_ShowNum(2,1,Password,4);//刷新
													}
												}
										if(Keynum==12)
										{
												Password=0;
												count=0;
												LCD_ShowNum(2,1,Password,4);//刷新
											
										}
										
										
							}
        }

}