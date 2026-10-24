#include <STC15F2K60S2.H>
#include "CBuzz.h"
#include "CLED.h"
#include "DS1302.h"
#include "Nixie.h"
#include "Timer0.h"

sbit Key_1=P3^3;
sbit Key_2=P3^2;
sbit Key_3=P3^1;
sbit Key_4=P3^0;


unsigned char KeyNum,ShiJian=1,RiQi=0,XingQi=0,Secg,Secs,Ming,Mins,Hourg,Hours,Yearg,Years,Monthg,Months,Dateg,Dates,Day;

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
	if(T0Count0>=21)
	{
		T0Count0=0;
		Key_Loop();
	}
	if(T0Count1>=2)
	{
		T0Count1=0;
		Nixie_Loop();
	}
}

void main()
{
	Timer0Init();
	LED_Close();
	Buzz_Close();
	DS1302_Init();
	DS1302_SetTime();
	while(1)
	{
		DS1302_ReadTime();		
		if(KeyNum==1)
		{
			KeyNum=0;
			ShiJian=1;
			RiQi=0;
			XingQi=0;
		}		
		
		if(KeyNum==2)
		{
			KeyNum=0;
			ShiJian=0;
			RiQi=1;
			XingQi=0;
		}	
		
		if(KeyNum==3)
		{
			KeyNum=0;
			ShiJian=0;
			RiQi=0;
			XingQi=1;
		}
	
		
		if(ShiJian)
		{
			Secg=DS1302_Time[5]%10;
			Secs=DS1302_Time[5]/10;
			Ming=DS1302_Time[4]%10;
			Mins=DS1302_Time[4]/10;
			Hourg=DS1302_Time[3]%10;
			Hours=DS1302_Time[3]/10;
			Nixie(6,11);
			Nixie(3,11);
			Nixie(8,Secg);
			Nixie(7,Secs);
			Nixie(5,Ming);
			Nixie(4,Mins);
			Nixie(2,Hourg);
			Nixie(1,Hours);
		}
		
		if(RiQi)
		{
			Dateg=DS1302_Time[2]%10;
			Dates=DS1302_Time[2]/10;
			Monthg=DS1302_Time[1]%10;
			Months=DS1302_Time[1]/10;
			Yearg=DS1302_Time[0]%10;
			Years=DS1302_Time[0]/10;
			Nixie(6,11);
			Nixie(3,11);
			Nixie(8,Dateg);
			Nixie(7,Dates);
			Nixie(5,Monthg);
			Nixie(4,Months);
			Nixie(2,Yearg);
			Nixie(1,Years);
		}	
		if(XingQi)
		{
			Day=DS1302_Time[6]%10;
			Nixie(7,0);
			Nixie(6,0);
			Nixie(8,Day);
			Nixie(5,10);
			Nixie(4,10);
			Nixie(3,10);
			Nixie(2,10);
			Nixie(1,10);
		}
	}
}