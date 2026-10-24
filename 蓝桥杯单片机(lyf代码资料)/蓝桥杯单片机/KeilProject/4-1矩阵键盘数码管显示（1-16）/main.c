#include <STC15F2K60S2.H>
#include "CBuzz.h"
#include "Nixie.h"
#include "Matrixkey.h"
#include "CLED.h"
#include "Timer0.h"

unsigned char Temp,Num;
unsigned char Shi=10;
unsigned char Ge=10;	

void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count1=0,T0Count2=0;
	TH0=(65536-917)/256;		//重装初值
	TL0=(65536-917)%256;		
	T0Count1++;
	T0Count2++;
	if(T0Count1>=21)
	{
		T0Count1=0;
		MKey_Loop();
	}
	if(T0Count2>=2)
	{
		T0Count2=0;
		Nixie_Loop();
	}
}

void main()
{
	Timer0_Init();
	Buzz_Close();
	LED_Close();  
	while(1)
	{
		Num=MKey();
		if(Num)
		{
			Shi=Num/10;
			Ge=Num%10;
			if(Shi==0)
			{
				Shi=10;
			}
		}
		Nixie(7,Shi);
		Nixie(8,Ge);			
	}
}