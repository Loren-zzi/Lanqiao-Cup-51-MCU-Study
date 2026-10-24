#include <STC15F2K60S2.H>
#include "CBuzz.h"
#include "CLED.h"
#include "Timer0.h"
#include "Nixie.h"
sbit Key_1=P3^3;
sbit Key_2=P3^2;
sbit Key_3=P3^1;
sbit Key_4=P3^0;

unsigned char Count,Compare;

unsigned char KeyNum,Speed;

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
	static unsigned char Count0,Count1;
	TL0 = 0xA4;		          //设置定时初值
	TH0 = 0xFF;		          //设置定时初值
	Count%=100;//高明算法
	if(Count<Compare)
	{
		P2=((P2&0x1f)|0xA0);
		P0=0x20;
		P2&=0x1f;
	}
	if(Count>=Compare)
	{
		P2=((P2&0x1f)|0xA0);
		P0=0x00;
		P2&=0x1f;
	}
	Count++;
	if(++Count0==20)
	{
		Count0=0;
		Nixie_Loop();
	}
	if(++Count1==200)
	{
		Count1=0;
		Key_Loop();
	}
}


void main()
{
	Buzz_Close();
	LED_Close();
	Timer0_Init();
	while(1)
	{
		if(KeyNum==1)
		{
			KeyNum=0;
			Speed=0;			
			Compare=0;
			Count=0;
		}
		if(KeyNum==2)
		{
			KeyNum=0;
			Speed=1;			
			Compare=50;
			Count=0;
		}
		if(KeyNum==3)
		{
			KeyNum=0;
			Speed=2;			
			Compare=75;
			Count=0;
		}
		if(KeyNum==4)
		{
			KeyNum=0;
			Speed=3;			
			Compare=100;
			Count=0;
		}
		Nixie(1,Speed);
	}
}

