#include <STC15F2K60S2.H>
#include "Delay.h"
#include "CBuzz.h"
#include "CLED.h"

sbit Key7=P3^0;
sbit Key6=P3^1;
sbit Key5=P3^2;
sbit Key4=P3^3;
sbit LED1=P0^0;
void main()
{
	Buzz_Close();
	LED_Close();//LED全灭初始化
	P2&=0x1f;
	P2|=0x80;
	while(1)
  {
			if(Key4==0||Key5==0||Key6==0||Key7==0)
			{
				Delay(20);
				while(Key4==0||Key5==0||Key6==0||Key7==0)
				{					
					LED1=0; 
				}
				Delay(30);
				LED1=1; 
			}
	}
}