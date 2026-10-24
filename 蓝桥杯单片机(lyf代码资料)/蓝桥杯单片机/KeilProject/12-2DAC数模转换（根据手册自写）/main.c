#include <STC15F2K60S2.H>
#include "CBuzz.h"
#include "CLED.h"
#include "Timer0.h"
#include "Nixie.h"
#include "PCF8591.h"

sbit Key_1=P3^3;
sbit Key_2=P3^2;
sbit Key_3=P3^1;
sbit Key_4=P3^0;

unsigned char Temp,Data,DianWeiQi,GuangMin;
unsigned char KeyNum;

unsigned char Key_State()
{
	unsigned char Key=0;
	if(Key_1==0)
	Key=1;
	if(Key_2==0)
	Key=2;
	if(Key_3==0)
	Key=3;
	if(Key_4==0)
	Key=4;
	return Key;
}

void Key_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Key_State();
	if(NowState==0&&LastState==1)
	KeyNum=1;
	if(NowState==0&&LastState==2)
	KeyNum=2;
	if(NowState==0&&LastState==3)
	KeyNum=3;
	if(NowState==0&&LastState==4)
	KeyNum=4;
}

void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count0=0,T0Count1=0;
	TH0=(65536-917)/256;		//重装初值
	TL0=(65536-917)%256;		
	T0Count0++;
	T0Count1++;
	if(T0Count0>=2)
	{
		T0Count0=0;
		Nixie_Loop();
	}
	if(T0Count1>=20)
	{
		T0Count1=0;
		Key_Loop();
	}
}

void main()
{
	Timer0_Init();
	LED_Close();
	Buzz_Close();
	while(1)
	{
		switch(KeyNum)
		{
			case 1:
			{
				KeyNum=0;
				PCF8591_DAC(127);
				Nixie(1,1);
			}break;
			case 2:
			{
				KeyNum=0;
				PCF8591_DAC(153);
				Nixie(1,2);
			}break;
			case 3:
			{
				KeyNum=0;
				PCF8591_DAC(178);
				Nixie(1,3);
			}break;
			case 4:
			{
				KeyNum=0;
				PCF8591_DAC(255);
				Nixie(1,4);
			}break;
		}
	}
}
	
	
