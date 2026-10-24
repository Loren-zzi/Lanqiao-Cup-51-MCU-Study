#include <STC15F2K60S2.H>
#include "Delay.h"
#include "CBuzz.h"

char Num=1;
sbit Key7=P3^0;
sbit Key6=P3^1;
sbit Key5=P3^2;
sbit Key4=P3^3;
void main()
{
	Buzz_Close();
	P2&=0x1f;
	P2|=0x80;
	P0=~0x01; //第一个LED亮
	while(1)
  {
			if(Key4==0||Key5==0||Key6==0||Key7==0)
			{
				Delay(20);
				while(Key4==0||Key5==0||Key6==0||Key7==0);
				Delay(30);
				P0=~(0x01<<Num); 
				Num++;
				if(Num==8)
				{
					Num=0;
				}
			}
	}
}