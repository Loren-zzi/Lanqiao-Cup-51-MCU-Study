#include <STC15F2K60S2.H>
#include "Key.h"
#include "CBuzz.h"
#include "Timer0.h"
#include "Timer1.h"
#include <INTRINS.H>

unsigned char Num,Count;

void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count=0;
	TH0=(65536-917)/256;		//重装初值
	TL0=(65536-917)%256;		
	T0Count++;
	if(T0Count>=500)
	{
		T0Count=0;
		if(Count==0)
		{	
			P0=_crol_(P0,1);
		}
		if(Count==1)
		{
			P0=_cror_(P0,1);
		}
	}
}

void main()
{
	Buzz_Close();
	P2&=0x1f;
	P2|=0x80;
	P0=0xfe;  
	Timer0Init();
	while(1)
	{
		Num=Key();
		if(Num)
		{
			Count++;
			if(Count==2)
			Count=0;
		}
	}
}