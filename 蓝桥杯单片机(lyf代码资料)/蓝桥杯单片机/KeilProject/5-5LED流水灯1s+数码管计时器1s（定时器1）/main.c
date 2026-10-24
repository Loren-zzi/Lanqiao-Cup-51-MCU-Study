#include <STC15F2K60S2.H>
#include "CBuzz.h"
#include "Timer1.h"
#include <INTRINS.H>
#include "Nixie.h"
#include "Delay.h"

unsigned char Secg,Secs,Ming,Mins,Hourg,Hours;
unsigned char Sec=0;
unsigned char Min=0;
unsigned char Hour=0;
unsigned char Num=0;

void Timer1_Routine() interrupt 3
{
	static unsigned int T1Count=0;
	TH1=(65536-917)/256;		
	TL1=(65536-917)%256;		
	T1Count++;
	if(T1Count>=1000)
	{
		T1Count=0;
		Num++;
		if(Num==8)
		Num=0;
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
	P2&=0x1f;
	P2|=0x80;
	P0=~0x01;  
	P2&=0x1f;
	Timer1Init();
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
		//P0=0xff;//关键
		P2&=0x1f;
		P2|=0x80;
		P0=~(0x01<<Num);
		P2&=0x1f;
	}
}