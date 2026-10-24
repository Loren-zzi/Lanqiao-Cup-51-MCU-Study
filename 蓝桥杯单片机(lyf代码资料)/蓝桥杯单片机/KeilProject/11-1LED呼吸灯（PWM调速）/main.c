#include <STC15F2K60S2.H>
#include "CBuzz.h"

//sbit LED_1=P0^0;

void Delay(unsigned char i)
{
	while(i--);
}

void main()
{
	unsigned char i,Time;
	Buzz_Close();
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	while(1)
	{
		for(Time=0;Time<100;Time++)
		{
			for(i=0;i<100;i++)
			{
				P0=0xff;
				Delay(Time);
				P0=0xff;
				Delay(100-Time);
			}
		}
		for(Time=100;Time>0;Time--)
		{
			for(i=0;i<100;i++)
			{
				P0=0x00;
				Delay(Time);
				P0=0xff;
				Delay(100-Time);
			}
		}
	}
}
