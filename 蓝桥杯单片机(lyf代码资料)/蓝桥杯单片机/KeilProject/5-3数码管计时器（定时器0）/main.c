#include <STC15F2K60S2.H>
#include "CBuzz.h"
#include "Timer0.h"
#include "Nixie.h"
#include "CLED.h"

unsigned char Secg,Secs,Ming,Mins,Hourg,Hours;
unsigned char Sec=0;
unsigned char Min=0;
unsigned char Hour=0;
void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count=0;
	TH0=(65536-917)/256;		
	TL0=(65536-917)%256;		
	T0Count++;
	if(T0Count>=1000)
	{
		T0Count=0;
		Sec++;
		if(Sec==60)
		{
			Min++;
			Sec=0;
			if(Min==60)
			{
				Hour++;
				Min=0;
				if(Hour==24)
				{
					Hour=0;
				}
			}
		}
	}
}

void main()
{
	Buzz_Close();
	LED_Close();
	Timer0Init();
	while(1)
	{
		Secg=Sec%10;
		Secs=Sec/10;
		Ming=Min%10;
		Mins=Min/10;
		Hourg=Hour%10;
		Hours=Hour/10;
		Nixie(8,Secg);
		Nixie(7,Secs);
		Nixie(6,10);
		Nixie(5,Ming);
		Nixie(4,Mins);
		Nixie(3,10);
		Nixie(2,Hourg);
		Nixie(1,Hours);		
	}
}