#include <STC15F2K60S2.H>
#include "Matrixkey.h"
#include "CBuzz.h"
#include "Timer1.h"
#include <INTRINS.H>

unsigned char Num,Count;

void Timer1_Routine() interrupt 3
{
	static unsigned int T1Count=0;
	TH1=(65536-917)/256;		
	TL1=(65536-917)%256;		
	T1Count++;
	if(T1Count>=500)
	{
		T1Count=0;
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
	Timer1Init();
	while(1)
	{
		Num=Matrixkey();
		if(Num)
		{
			Count++;
			if(Count==2)
			Count=0;
		}
	}
}